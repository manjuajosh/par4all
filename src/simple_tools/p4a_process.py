#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Authors:
# - Grégoire Péan <gregoire.pean@hpc-project.com>
# - Ronan Keryell <ronan.keryell@hpc-project.com>
#
import p4a_util
import optparse
import subprocess
import sys
import os
import re
import shutil

'''
Par4All processing
'''



# Basic properties to be used in Par4All:
default_properties = dict(
    # Useless to go on if something goes wrong... :-(
    #ABORT_ON_USER_ERROR = True,
    ABORT_ON_USER_ERROR = False,
    # Compute the intraprocedural preconditions at the same
    # Compute the intraprocedural preconditions at the same
    # time as transformers and use them to improve the
    # accuracy of expression and statement transformers:
    SEMANTICS_COMPUTE_TRANSFORMERS_IN_CONTEXT = True,
    # Use the more precise fix point operator to cope with
    # while loops:
    SEMANTICS_FIX_POINT_OPERATOR = "derivative",
    # Try to restructure the code for more precision:
    UNSPAGHETTIFY_TEST_RESTRUCTURING = True,
    UNSPAGHETTIFY_RECURSIVE_DECOMPOSITION = True,
    # Simplify for loops into Fortran do-loops internally for
    # better precision of analysis:
    FOR_TO_DO_LOOP_IN_CONTROLIZER = True,
    # Warning: assume that there is no aliasing between IO
    # streams ('FILE *' variables):
    ALIASING_ACROSS_IO_STREAMS = False,
    # Warning: this is a work in progress. Assume no weird
    # aliasing
    CONSTANT_PATH_EFFECTS = False,
    # Prevents automatic addition of OpenMP directives when
    # unslitting.  We will add them manually using ompify if
    # requested.
    PRETTYPRINT_SEQUENTIAL_STYLE = "do"
)

# The default values of some PIPS propeties are ok for C but has to be
# redifined for FORTRAN
default_fortran_cuda_properties = dict(
    GPU_KERNEL_PREFIX                     = "P4A_KERNEL",
    GPU_WRAPPER_PREFIX                    = "P4A_KERNEL_WRAPPER",
    GPU_LAUNCHER_PREFIX                   = "P4A_KERNEL_LAUNCHER",
    GPU_FORTRAN_WRAPPER_PREFIX            = "P4A_FORTRAN_WRAPPER",
    CROUGH_SCALAR_BY_VALUE_IN_FCT_DECL    = True,
    CROUGH_SCALAR_BY_VALUE_IN_FCT_CALL    = True,
    PRETTYPRINT_STATEMENT_NUMBER          = False,
    CROUGH_FORTRAN_USES_INTERFACE         = True,
    KERNEL_LOAD_STORE_LOAD_FUNCTION_2D    = "P4A_COPY_TO_ACCEL_2D",
    KERNEL_LOAD_STORE_ALLOCATE_FUNCTION   = "P4A_ACCEL_MALLOC",
    KERNEL_LOAD_STORE_STORE_FUNCTION_2D   = "P4A_COPY_FROM_ACCEL_2D",
    KERNEL_LOAD_STORE_DEALLOCATE_FUNCTION = "P4A_ACCEL_FREE",
    KERNEL_LOAD_STORE_VAR_SUFFIX          = "_num"
)

# Import of pyps will be done manually.
# Module instance will be held in following variable.
pyps = None


class p4a_processor_output(object):
    files = []
    database_dir = ""
    exception = None


class p4a_processor_input(object):
    """Store options given to the process engine, mainly digested by PyPS.
    Some of the options are used during the output file generation.
    """
    project_name = ""
    accel = False
    cuda = False
    com_optimization = False
    fftw3 = False
    openmp = False
    fine = False
    select_modules = ""
    exclude_modules = ""
    cpp_flags = ""
    files = []
    recover_includes = True
    native_recover_includes = False
    properties = {}
    output_dir=None
    output_suffix=""
    output_prefix=""
    # To store some arbitrary Python code to be executed inside p4a_process:
    execute_some_python_code_in_process = None


def add_module_options(parser):
    "Add to an optparse option parser the options related to this module"
    group = optparse.OptionGroup(parser, "Processor Options")

    group.add_option("--input-file", metavar = "FILE", default = None,
        help = "Input file (as created using the pickle module on a p4a_processor_input instance).")

    group.add_option("--output-file", metavar = "FILE", default = None,
        help = "Output file (to be created using the pickle module on a p4a_processor_output instance).")

    parser.add_option_group(group)


def process(input):
    """Process the input files with PIPS and return the list of produced files
    """
    output = p4a_processor_output()

    # Execute some arbitrary Python code here if asked:
    if input.execute_some_python_code_in_process:
        exec(input.execute_some_python_code_in_process)

    try:
        # Create a workspace with PIPS:
        processor = p4a_processor(
            project_name = input.project_name,
            cpp_flags = input.cpp_flags,
            verbose = True,
            files = input.files,
            filter_select = input.select_modules,
            filter_exclude = input.exclude_modules,
            accel = input.accel,
            cuda = input.cuda,
            openmp=input.openmp,
            com_optimization = input.com_optimization,
            fftw3 = input.fftw3,
            recover_includes = input.recover_includes,
            native_recover_includes = input.native_recover_includes,
            properties = input.properties,
        )

        output.database_dir = processor.get_database_directory()

        # First apply some generic parallelization:
        processor.parallelize(fine = input.fine)

        if input.accel:
            # Generate code for a GPU-like accelerator. Note that we can
            # have an OpenMP implementation of it if OpenMP option is set
            # too:
            processor.gpuify()

        if input.openmp and not input.accel:
            # Parallelize the code in an OpenMP way:
            processor.ompify()

        # Write the output files.
        output.files = processor.save(input.output_dir,
                                      input.output_prefix,
                                      input.output_suffix)

    except:
        # Get the exception description:
        e = sys.exc_info()
        exception = e[1]
        if exception.__class__.__name__ == "RuntimeError" and str(exception).find("pips") != -1:
            output.exception = p4a_util.p4a_error("An error occurred in PIPS while processing " + ", ".join(input.files))
        else:
            # Since the traceback object cannot be serialized, display
            # here the exec_info for more information on stderr:
            sys.excepthook(*e)
            # And only push the exception further for information:
            output.exception = exception

    return output


def process_file(input_file, output_file):
    input = p4a_util.load_pickle(input_file)
    output = process(input)
    p4a_util.save_pickle(output_file, output)


def main(options, args = []):
    """Process the workspace with PIPS

    The description of the input and the output are given as 2 pickled
    object files with names given as parameters
    """
    if not options.input_file:
        p4a_util.die("Missing --input-file")

    if not options.output_file:
        p4a_util.die("Missing --output-file")

    if not os.path.exists(options.input_file):
        p4a_util.die("Input file does not exist: " + options.input_file)

    process_file(options.input_file, options.output_file)


class p4a_processor(object):
    """Process program files with PIPS and other tools
    """
    # If the main language is Fortran, set to True:
    fortran = None

    workspace = None

    main_filter = None

    # The project name:
    project_name = None

    # Set to True to try to do some #include tracking and recovering
    recover_includes = None
    native_recover_includes = None

    # initialize some lists of files
    # the list of the input files
    files = []
    # the list of the p4a stub files
    accel_files = []

    # the list of the "on the fly" ressources generated by PIPS
    # generated modules (by pips)
    generated_modules = []
    # the list of the crough modules
    crough_modules = []
    # the list of module with interfaces
    interface_modules = []
    # the set of cuda modules
    cuda_modules = set ()
    # the set of c modules
    c_modules = set ()
    # the list of fortran modules
    fortran_modules = set ()

    # the typedef to be used in cuda
    kernel_return_type  = "P4A_accel_kernel"
    wrapper_return_type = "P4A_accel_kernel_wrapper"

    def __init__(self, workspace = None, project_name = "", cpp_flags = "",
                 verbose = False, files = [], filter_select = None,
                 filter_exclude = None, accel = False, cuda = False, openmp = False,
                 com_optimization = False, fftw3 = False,
                 recover_includes = True, native_recover_includes = False,
                 properties = {}, activates = []):

        self.recover_includes = recover_includes
        self.native_recover_includes = native_recover_includes
        self.accel = accel
        self.cuda = cuda
        self.openmp = openmp
        self.com_optimization = com_optimization
        self.fftw3 = fftw3,

        if workspace:
            # There is one provided: use it!
            self.workspace = workspace
        else:
            # This is because pyps.workspace.__init__ will test for empty
            # strings...
            if cpp_flags is None:
                cpp_flags = ""

            if not project_name:
                raise p4a_util.p4a_error("Missing project_name")

            self.project_name = project_name

            if self.recover_includes and not self.native_recover_includes:
                # Use a special preprocessor to track #include by a
                # man-in-the-middle attack :-) :
                os.environ['PIPS_CPP'] = 'p4a_recover_includes --simple -E'

            for file in files:
                if self.fortran is None:
                    # Track the language for an eventual later compilation
                    # by a back-end target compiler. The first file type
                    # select the type for all the workspace:
                    if p4a_util.fortran_file_p(file):
                        self.fortran = True
                    else:
                        self.fortran = False

                if not os.path.exists(file):
                    raise p4a_util.p4a_error("File does not exist: " + file)

            self.files = files

            if accel:
                accel_stubs_name = None
                # Analyze this stub file so that PIPS interprocedurality
                # is happy about the run-time we use:
                if self.fortran:
                    accel_stubs_name = "p4a_stubs.f"
                else:
                    accel_stubs_name = "p4a_stubs.c"
                # The stubs are here in our distribution:
                accel_stubs = os.path.join(os.environ["P4A_ACCEL_DIR"],
                                           accel_stubs_name)
                # Add the stubs file to the list to use in PIPS:
                self.files += [ accel_stubs ]
                # Mark this file as a stub to avoid copying it out later:
                self.accel_files += [ accel_stubs ]

            # Late import of pyps to avoid importing it until
            # we really need it.
            global pyps
            try:
                pyps = __import__("pyps")
            except:
                raise

            # If we have #include recovery
            # and want to use the native
            # one:
            recover_Include = self.recover_includes and self.native_recover_includes
            # Create the PyPS workspace:
            self.workspace = pyps.workspace(self.files,
                                            name = self.project_name,
                                            verbose = verbose,
                                            cppflags = cpp_flags,
                                            recoverInclude = recover_Include)

            # Array regions are a must! :-) Ask for most precise array
            # regions:
            self.workspace.activate("MUST_REGIONS")

            # set the workspace properties
            self.set_properties (properties)


        # Skip the compilation units and the modules of P4A runtime, they
        # are just here so that PIPS has a global view of what is going
        # on, not to be parallelized :-)
        skip_p4a_runtime_and_compilation_unit_re = re.compile("P4A_.*|.*!$")

        # Also filter out modules based on --include-modules and
        # --exclude-modules.
        filter_select_re = None
        if filter_select:
            filter_select_re = re.compile(filter_select)
        filter_exclude_re = None
        if filter_exclude:
            filter_exclude_re = re.compile(filter_exclude)
        # Combine the 3 filters in one:
        self.main_filter = (lambda module: not skip_p4a_runtime_and_compilation_unit_re.match(module.name)
            and (filter_exclude_re == None or not filter_exclude_re.match(module.name))
            and (filter_select_re == None or filter_select_re.match(module.name)))

    def set_properties (self, user_properties):
        """ Initialize the properties according to the default defined properties
        and to the user defined ones.
        """
        global default_properties
        global default_fortran_cuda_properties
        all_properties = default_properties
        # if accel (might be cuda) and fortran add some properties
        if ((self.accel == True) and (self.fortran == True)):
            for k in default_fortran_cuda_properties:
                all_properties[k] = default_fortran_cuda_properties[k]
        # overwrite default properties with the user defined ones
        for k in user_properties:
            all_properties[k] = properties[k]
        # apply the properties to the workspace
        for k in all_properties:
            p4a_util.debug("Property " + k + " = " + str(all_properties[k]))
            self.workspace.props[k] = all_properties[k]
        return

    def get_database_directory(self):
        "Return the directory of the current PIPS database"
        return os.path.abspath(self.workspace.dirname())


    def filter_modules(self, filter_select = None, filter_exclude = None, other_filter = lambda x: True):
        """Filter modules according to their names and select them if they
        match all the 3 following conditions.

        If filter_exclude regex if defined, then matching modules are
        filtered out.

        If filter_select regex if defined, the matching modules are kept.

        If other_filter regex if defined, select also according to this
        matching.

        """
        filter_select_re = None
        if filter_select:
            filter_select_re = re.compile(filter_select)

        filter_exclude_re = None
        if filter_exclude:
            filter_exclude_re = re.compile(filter_exclude)

        filter = (lambda module: self.main_filter(module)
            and (filter_exclude_re == None or not filter_exclude_re.match(module.name))
            and (filter_select_re == None or filter_select_re.match(module.name))
            and other_filter(module.name))
        # Select the interesting modules:
        return self.workspace.filter(filter)


    def parallelize(self, fine = False, filter_select = None, filter_exclude = None):
        """Apply transformations to parallelize the code in the workspace
        """
        all_modules = self.filter_modules(filter_select, filter_exclude)

        # Try to privatize all the scalar variables in loops:
        all_modules.privatize_module()

        if fine:
            # Use a fine-grain parallelization à la Allen & Kennedy:
            all_modules.internalize_parallel_code()
        else:
            # Use a coarse-grain parallelization with regions:
            all_modules.coarse_grain_parallelization()

    def post_process_fortran_wrapper (self, file_name, subroutine_name):
        """ All the dirty thing about C and Fortran interoperability is hidden
        in one unique file, the fortran wrapper. This method does the last
        modification to make the file compilable by gfortran.
        Those steps are done
        1 - insert the needed "use" statement
        2 - insert the pointer declaration for P4A inserted variable
        3 - insert the 64 bit integer var
        3 - types are wrtitten with a () in the size_of function
        4 - remove the f77 multiline. Also remove the first blank chars
        5 - replace f77 comments with f95 comments
        6 - remove the (void **) & that is not usefull in fortran
        7 - remove the * in front of the inserted variables
        8 - declare the origin_var_name as a target
        9 - make DMA transfer variable to be 64 bit variables
        """
        p4a_util.debug ("Processing fortran_wrapper " + file_name)
        indent = "  "
        # get the code to be post process
        code = p4a_util.read_file (file_name, True)

        # step 1
        # insert the needed use statement right after the subroutine declaration
        # common interface to be used
        use_string  = indent + "use iso_c_binding\n"
        use_string += indent + "use p4a_runtime_interface\n"
        # add the dedicated interface i.e use the KERNEL_LAUNCHER prefix
        # instead of the FORTRAN_WRAPPER prefix
        use_string += indent + "use "
        use_string += subroutine_name.replace(self.get_fortran_wrapper_prefix(),
                                              self.get_launcher_prefix())

        use_string += "_interface\n"

        # step 2
        # first identify the inserted variable
        var_prefix = self.get_kernel_load_store_var_prefix ()
        var_suffix = self.get_kernel_load_store_var_suffix ()
        var_re = re.compile(var_prefix + "\\w+" + var_suffix + "\\d+")
        inserted_var_l = var_re.findall (code)
        origin_var_s = set ()
        inserted_var_s = set ()
        p4a_util.add_list_to_set (inserted_var_l, inserted_var_s)
        inserted_var_decl = indent + "type (c_ptr) ::"
        first = True
        for var in inserted_var_s:
            if (first == True):
                first = False
            else:
                inserted_var_decl += ","
            inserted_var_decl += " " + var
            # extract the original variable name
            origin_var_s.add (var.replace (var_prefix, "").replace (var_suffix, ""))
        inserted_var_decl += "\n" + indent + "integer (c_size_t), target :: p4a_zero"

        #step 3
        c_sizeof_replace = dict()
        c_sizeof_replace ["CHARACTER()"] = "c_char"
        c_sizeof_replace ["LOGICAL()"]   = "c_bool"
        c_sizeof_replace ["INTEGER()"]   = "c_int"
        c_sizeof_replace ["INTEGER*4()"] = "c_int"
        c_sizeof_replace ["INTEGER*8()"] = "c_long_long"
        c_sizeof_replace ["REAL()"]      = "c_float"
        c_sizeof_replace ["REAL*4()"]    = "c_float"
        c_sizeof_replace ["REAL*8()"]    = "c_double"
        for k, v in c_sizeof_replace.iteritems ():
            code = code.replace (k, v)

        # step 4
        F77_INDENTATION  = "\n      "
        F95_INDENTATION  = "\n" + indent
        F77_CONTINUATION = "\n     &"
        F95_CONTINUATION = "&\n      "
        code = code.replace (F77_CONTINUATION, " ")
        code = code.replace (F77_INDENTATION, F95_INDENTATION)

        # step 5
        F77_CONTINUATION = "\nC"
        F95_CONTINUATION = "\n!"
        code = code.replace (F77_CONTINUATION, F95_CONTINUATION)

        # step 6
        code = code.replace ("(void **) &", "")

        # step 7
        for var in inserted_var_s:
            code = code.replace ("*" + var, var)

        # step 8
        # insert the target attribute for all declared variables
        types_l = ["CHARACTER", "LOGICAL", "INTEGER*4", "INTEGER*8", "INTEGER",
                   "REAL*4", "REAL*8","REAL"]
        for t in types_l:
            code = code.replace (t, t.lower () + ", target ::")

        # step 9
        function_l = ["P4A_COPY_FROM_ACCEL_2D", "P4A_COPY_TO_ACCEL_2D"]
        for func in function_l:
            # this re match the full line where the CALL to the dma transfer happends
            func_line_re = re.compile("^ *CALL " + func + "\(.*\)",
                                           re.MULTILINE)
            # this re match the same line and extract the parameters
            func_sig_re = re.compile("^ *CALL " + func + "\((.*)\)",
                                          re.MULTILINE)
            # this re match the function name
            func_name_re = re.compile("^ *(CALL " + func + ")\(.*\)",
                                           re.MULTILINE)
            new_line_l = []
            line_l = func_line_re.findall (code)
            name_l = func_name_re.findall (code)
            sig_l = func_sig_re.findall (code)
            insert_init = True
            for index in range (len (line_l)):
                #for each match we need to ensure that parameter 2..7 are 64 bits
                no_space = sig_l[index].replace (" ", "")
                arg_l = no_space.split (",")
                for arg_num in range (1,7):
                    arg_l[arg_num] += "+p4a_zero"
                    new_line = indent + name_l [index] +"("
                    first = True
                for arg in arg_l:
                    if first:
                        first = False
                    else:
                        new_line += ","
                    new_line += arg
                new_line += ")"
                if insert_init:
                    insert_init = False
                    code = code.replace (line_l[index], indent + "p4a_zero = 0\n" + line_l[index])
                code = code.replace (line_l[index], new_line)

        # step 10
        # identify where to insert the use string and the inserted variable
        # declaration
        subroutine_line_re = re.compile("SUBROUTINE " + subroutine_name + ".*$",
                                        re.MULTILINE)
        subroutine_l = subroutine_line_re.findall (code)
        assert (len (subroutine_l) == 1)
        code = code.replace (subroutine_l[0], subroutine_l[0] + "\n" +
                             use_string + inserted_var_decl)

        # write the post processed code
        p4a_util.write_file(file_name, code, True)

        return

    def get_launcher_prefix (self):
        return self.workspace.props.GPU_LAUNCHER_PREFIX

    def get_kernel_prefix (self):
        return self.workspace.props.GPU_KERNEL_PREFIX

    def get_wrapper_prefix (self):
        return self.workspace.props.GPU_WRAPPER_PREFIX

    def get_fortran_wrapper_prefix (self):
        return self.workspace.props.GPU_FORTRAN_WRAPPER_PREFIX

    def get_kernel_load_store_var_prefix (self):
        return self.workspace.props.KERNEL_LOAD_STORE_VAR_PREFIX

    def get_kernel_load_store_var_suffix (self):
        return self.workspace.props.KERNEL_LOAD_STORE_VAR_SUFFIX

    def fortran_wrapper_p (self, file_name):
        prefix = self.get_fortran_wrapper_prefix()
        fortran_wrapper_file_name_re = re.compile(prefix + "_[0-9]+.f[0-9]*")
        m = fortran_wrapper_file_name_re.match (os.path.basename (file_name))
        return (m != None)

    def gpuify(self, filter_select = None, filter_exclude = None):
        """Apply transformations to the parallel loop nested found in the
        workspace to generate GPU-oriented code
        """
        all_modules = self.filter_modules(filter_select, filter_exclude)

        # First, only generate the launchers to work on them later. They are
        # generated by outlining all the parallel loops. If in the fortran case
        # we want the launcher to be wrappe in an independant fortran function
        # to ease future post processing.
        all_modules.gpu_ify(GPU_USE_WRAPPER = False,
                            GPU_USE_KERNEL = False,
                            GPU_USE_FORTRAN_WRAPPER = self.fortran,
                            GPU_USE_LAUNCHER = True,
                            concurrent=True)

        # Select kernel launchers by using the fact that all the generated
        # functions have their names beginning with the launcher prefix:
        launcher_prefix = self.get_launcher_prefix ()
        kernel_launcher_filter_re = re.compile(launcher_prefix + "_.*[^!]$")
        kernel_launchers = self.workspace.filter(lambda m: kernel_launcher_filter_re.match(m.name))

        # Normalize all loops in kernels to suit hardware iteration spaces:
        if (self.fortran == False):
            kernel_launchers.loop_normalize(
                # Loop normalize for the C language and GPU friendly
                LOOP_NORMALIZE_ONE_INCREMENT = True,
                # Arrays start at 0 in C, so the iteration loops:
                LOOP_NORMALIZE_LOWER_BOUND = 0,
                # It is legal in the following by construction (...Hmmm to verify)
                LOOP_NORMALIZE_SKIP_INDEX_SIDE_EFFECT = True,
                concurrent=True)
        else:
            kernel_launchers.loop_normalize(
                # Loop normalize for the C language and GPU friendly
                LOOP_NORMALIZE_ONE_INCREMENT = True,
                # Arrays start at 0 in C, so the iteration loops:
                LOOP_NORMALIZE_LOWER_BOUND = 1,
                # It is legal in the following by construction (...Hmmm to verify)
                LOOP_NORMALIZE_SKIP_INDEX_SIDE_EFFECT = True,
                concurrent=True)

        # Unfortunately the information about parallelization and
        # privatization is lost by the current outliner, so rebuild
        # it... :-( But anyway, since we've normalized the code, we
        # changed it so it is to be parallelized again...
        #kernel_launchers.privatize_module()

        # Since the privatization of a module does not change
        # privatization of other modules, use concurrent=True (capply) to
        # apply them without requiring pipsmake to carefully rebuild
        # dependent resources:
        kernel_launchers.privatize_module(concurrent=True)
        # Idem for this phase:
        kernel_launchers.coarse_grain_parallelization(concurrent=True)

        # In CUDA there is a limitation on 2D grids of thread blocks, in
        # OpenCL there is a 3D limitation, so limit parallelism at 2D
        # top-level loops inside parallel loop nests:
        kernel_launchers.limit_nested_parallelism(NESTED_PARALLELISM_THRESHOLD = 2, concurrent=True)

        #kernel_launchers.localize_declaration()
        # Does not work:
        #kernel_launchers.omp_merge_pragma()


        # Add iteration space decorations and insert iteration clamping
        # into the launchers onto the outer parallel loop nests:
        kernel_launchers.gpu_loop_nest_annotate(concurrent=True)

        # End to generate the wrappers and kernel contents, but not the
        # launchers that have already been generated:
        kernel_launchers.gpu_ify(GPU_USE_LAUNCHER = False,
                                 concurrent=True)

        # Select kernels by using the fact that all the generated kernels
        # have their names of this form:
        kernel_prefix = self.get_kernel_prefix ()
        kernel_filter_re = re.compile(kernel_prefix + "_\\w+$")
        kernels = self.workspace.filter(lambda m: kernel_filter_re.match(m.name))

        if not self.com_optimization :
            # Add communication around all the call site of the kernels. Since
            # the code has been outlined, any non local effect is no longer an
            # issue:
            kernel_launchers.kernel_load_store(concurrent=True,
                                               ISOLATE_STATEMENT_EVEN_NON_LOCAL = True
                                               )
        else :
            # Identify kernels first
            kernels.flag_kernel()
            #kernel for fftw3 runtime
            fftw3_kernel_filter_re = re.compile("^fftw.?_execute")
            fftw3_kernels = self.workspace.filter(lambda m: fftw3_kernel_filter_re.match(m.name))
            fftw3_kernels.flag_kernel()
            self.workspace.fun.main.kernel_data_mapping(KERNEL_LOAD_STORE_LOAD_FUNCTION="P4A_runtime_copy_to_accel",KERNEL_LOAD_STORE_STORE_FUNCTION="P4A_runtime_copy_from_accel")

        # Indeed, it is not only in kernels but also in all the CUDA code
        # that these C99 declarations are forbidden. We need them in the
        # original code for more precise analysis but we need to remove
        # them everywhere :-(
        #kernel_P4A_re = re.compile("^(P4A_.*|main)$")
        #skip_P4A = self.workspace.filter(lambda m: not kernel_P4A_re.match(m.name))
        #self.workspace.all_functions.array_to_pointer(
        #skip_P4A.array_to_pointer(
        #    ARRAY_TO_POINTER_FLATTEN_ONLY = True,
        #    ARRAY_TO_POINTER_CONVERT_PARAMETERS = "POINTER",
        #    concurrent=False
        #    )

        # Select wrappers by using the fact that all the generated wrappers
        # have their names of this form:
        wrapper_prefix = self.get_wrapper_prefix ()
        wrapper_filter_re = re.compile(wrapper_prefix  + "_\\w+$")
        wrappers = self.workspace.filter(lambda m: wrapper_filter_re.match(m.name))

        # Select fortran wrappers by using the fact that all the generated
        # fortran wrappers
        # have their names of this form:
        f_wrapper_prefix = self.get_fortran_wrapper_prefix ()
        f_wrapper_filter_re = re.compile(f_wrapper_prefix  + "_\\d+$")
        f_wrappers = self.workspace.filter(lambda m: f_wrapper_filter_re.match(m.name))
#        f_wrappers.print_call_graph ()

        # Unfortunately CUDA 3.0 does not accept C99 array declarations
        # with sizes also passed as parameters in kernels. So we degrade
        # the quality of the generated code by generating array
        # declarations as pointers and by accessing them as
        # array[linearized expression]:
        if (self.fortran == False):
            kernels.linearize_array(LINEARIZE_ARRAY_USE_POINTERS=True,LINEARIZE_ARRAY_CAST_AT_CALL_SITE=True)
            wrappers.linearize_array(LINEARIZE_ARRAY_USE_POINTERS=True,LINEARIZE_ARRAY_CAST_AT_CALL_SITE=True)
        else:
            kernels.linearize_array_fortran(LINEARIZE_ARRAY_USE_POINTERS=False,LINEARIZE_ARRAY_CAST_AT_CALL_SITE=True)
            wrappers.linearize_array_fortran(LINEARIZE_ARRAY_USE_POINTERS=False,LINEARIZE_ARRAY_CAST_AT_CALL_SITE=True)

        # set return type for wrappers && kernel
        if (self.fortran == False):
            wrappers.set_return_type_as_typedef(SET_RETURN_TYPE_AS_TYPEDEF_NEW_TYPE=self.wrapper_return_type)
            kernels.set_return_type_as_typedef(SET_RETURN_TYPE_AS_TYPEDEF_NEW_TYPE=self.kernel_return_type)
        else:
            # generate the C version of kernels, wrappers and launchers.
            # kernel and wrappers need to be prettyprinted with arrays as
            # pointers because they will be .cu files
            kernels.display ("c_printed_file",
                             CROUGH_INCLUDE_FILE_LIST="p4a_accel.h",
                             DO_RETURN_TYPE_AS_TYPEDEF=True,
                             CROUGH_ARRAY_PARAMETER_AS_POINTER=True,
                             SET_RETURN_TYPE_AS_TYPEDEF_NEW_TYPE=self.kernel_return_type)
            wrappers.display ("c_printed_file",
                             CROUGH_INCLUDE_FILE_LIST="p4a_accel.h",
                              DO_RETURN_TYPE_AS_TYPEDEF=True,
                              CROUGH_ARRAY_PARAMETER_AS_POINTER=True,
                              SET_RETURN_TYPE_AS_TYPEDEF_NEW_TYPE=self.wrapper_return_type)
            # Do the phase set_return_type_as_typedef using regular expressions
            # because the phase is not available in fortran
            # kernel_launchers will be .c file so c99 is allowed
            kernel_launchers.display ("c_printed_file",
                                      DO_RETURN_TYPE_AS_TYPEDEF=False,
                                      CROUGH_ARRAY_PARAMETER_AS_POINTER=False)

            # those newly generated modules has to be append to the dedicated list
            # for later processing
            self.crough_modules.extend (map(lambda x:x.name, kernels))
            self.crough_modules.extend (map(lambda x:x.name, wrappers))
            self.crough_modules.extend (map(lambda x:x.name, kernel_launchers))
            # Generate the interface of the wrappers. This will be used to call
            # the C functions of the wrappers from the fortran_wrapper
            # subroutines
            kernel_launchers.print_interface ()
            self.interface_modules.extend (map(lambda x:x.name, kernel_launchers))
            self.generated_modules.extend (map(lambda x:x.name, f_wrappers))

        #update some list of modules
        p4a_util.add_list_to_set (map(lambda x:x.name, kernels),
                                  self.cuda_modules)
        p4a_util.add_list_to_set (map(lambda x:x.name, wrappers),
                                  self.cuda_modules)

        if self.com_optimization :
            wrappers.wrap_kernel_argument(WRAP_KERNEL_ARGUMENT_FUNCTION_NAME="P4A_runtime_host_ptr_to_accel_ptr")
            wrappers.cast_at_call_sites()

        #self.workspace.all_functions.display()

        # To be able to inject Par4All accelerator run time initialization
        # later:
        if "main" in self.workspace:
            self.workspace["main"].prepend_comment(PREPEND_COMMENT = "// Prepend here P4A_init_accel")
        else:
            p4a_util.warn('''
            There is no "main()" function in the given sources.
            That means the P4A Accel runtime initialization can not be
            inserted and that the compiled application may not work.

            If you build a P4A executable from partial p4a output, you
            should add a
               #include <p4a_accel.h>
            at the beginning of the .c file containing the main()
            and add at the beginning of main() a line with:
               P4A_init_accel;
            ''')


    def ompify(self, filter_select = None, filter_exclude = None):
        """Add OpenMP #pragma from loop-parallel flag internal
        representation to generate... OpenMP code!"""

        modules = self.filter_modules(filter_select, filter_exclude);
        modules.ompify_code(concurrent=True)
        modules.omp_merge_pragma(concurrent=True)


    def accel_post(self, file, dest_dir = None):
        '''Method for post processing "accelerated" files'''

        p4a_util.info("Post-processing " + file)

        post_process_script = os.path.join(p4a_util.get_program_dir(), "p4a_post_processor.py")

        args = [ post_process_script ]
        if dest_dir:
            args += [ '--dest-dir', dest_dir ]
        args.append(file)

        p4a_util.run(args,force_locale = None)
        #~ subprocess.call(args)

    def save_crough (self, output_dir):
        """ Save the crough files that might have been generated by
        PIPS during the p4a process. Those files need a special handling since
        they are not produced in the standard Src folder by the unsplit phase.
        """
        result = []
        for name in self.crough_modules:
            # Where the file does well in the .database workspace:
            pips_file = os.path.join(self.workspace.dirname(),
                                     name, name + ".c")
            # set the destination file
            output_name = name + ".c"
            if name in self.cuda_modules:
                if self.cuda:
                    output_name = p4a_util.change_file_ext(output_name, ".cu")
                # generate the header file
                header_file = os.path.join(output_dir, name + ".h")
                args = ["cproto"]
                args.append ("-D")
                args.append (self.wrapper_return_type + "=void")
                args.append ("-D")
                args.append (self.kernel_return_type + "=void")
                args.append ("-o")
                args.append (header_file)
                args.append (pips_file)
                p4a_util.run (args, force_locale = None)
            # The final destination
            output_file = os.path.join(output_dir, output_name)
            # Copy the PIPS production to its destination:
            shutil.copyfile(pips_file, output_file)
            result.append(output_file)
        return result

    def save_generated (self, output_dir):
        """ Save the generated files that might have been generated by
        PIPS during the p4a process.
        """
        result = []
        for name in self.generated_modules:
            # Where the file does well in the .database workspace:
            pips_file = os.path.join(self.workspace.dirname(), "Src",
                                     name + ".f")
            output_name = name + ".f08"
            # The final destination
            output_file = os.path.join(output_dir, output_name)
            if (self.fortran_wrapper_p (pips_file) == True):
                self.post_process_fortran_wrapper (pips_file, name)
            # Copy the PIPS production to its destination:
            shutil.copyfile(pips_file, output_file)
            result.append(output_file)
        return result

    def save_interface (self, output_dir):
        """ Save the interface files that might have been generated during by
        PIPS during the p4a process. Those files need a special handling since
        they are not produced in the standard Src folder by the unsplit phase.
        """
        result = []
        flag = True
        for name in self.interface_modules:
            # Where the file does well in the .database workspace:
            pips_file = os.path.join(self.workspace.dirname(), name,
                                     name + "_interface.f08")
            output_name = name + "_interface.f08"
            # The final destination
            output_file = os.path.join(output_dir, output_name)
            # Copy the PIPS production to its destination:
            shutil.copyfile(pips_file, output_file)
            result.append(output_file)
            if flag:
                result.append (os.path.join(os.environ["P4A_ACCEL_DIR"],
                                            "p4a_runtime_interface.f95"))
                flag = False
        return result

    def save_user_file (self, dest_dir, prefix, suffix):
        """ Save the user file appended to the Workspace at the begining
        """
        result = []
        # For all the user defined files from the workspace:
        for file in self.files:
            if file in self.accel_files:
                # We do not want to remove the stubs file from the
                # distribution... :-/
                #os.remove(file)
                continue
            (dir, name) = os.path.split(file)
            # Where the file does well in the .database workspace:
            pips_file = os.path.join(self.workspace.dirname(), "Src", name)

            # Recover the includes in the given file only if the flags have
            # been previously set and this is a C program:
            if self.recover_includes and not self.native_recover_includes and p4a_util.c_file_p(file):
                subprocess.call([ 'p4a_recover_includes',
                                  '--simple', pips_file ])

            # Update the destination directory if one was given:
            if dest_dir:
                dir = dest_dir
                if not (os.path.isdir(dir)):
                    os.makedirs (dir)

            output_name = prefix + name
            if suffix:
                output_name = p4a_util.file_add_suffix(output_name, suffix)

            # The final destination
            output_file = os.path.join(dir, output_name)

            if self.accel and p4a_util.c_file_p(file):
                # We generate code for P4A Accel, so first post process
                # the output:
                self.accel_post(pips_file,
                                os.path.join(self.workspace.dirname(), "P4A"))
                # Where the P4A output file does dwell in the .database
                # workspace:
                p4a_file = os.path.join(self.workspace.dirname(), "P4A", name)
                # Update the normal location then:
                pips_file = p4a_file
                if self.cuda:
                    # To have the nVidia compiler to be happy, we need to
                    # have a .cu version of the .c file
                    output_file = p4a_util.change_file_ext(output_file, ".cu")

            # Copy the PIPS production to its destination:
            shutil.copyfile(pips_file, output_file)
            result.append (output_file)
        return result

    def save(self, dest_dir = None, prefix = "", suffix = "p4a"):
        """Final post-processing and save the files of the workspace. This
        includes the original files defined by the user and also all new
        files that might have been generated by PIPS.
        """

        output_files = []

        # do not allow None suffix or prefix
        if prefix is None:
            prefix = ""
        if suffix is None:
            suffix = ""

        #Set the suffix if needed to avoid file destruction
        if (dest_dir == None) and ( prefix == "") and (suffix == ""):
            suffix = "p4a"

        #append or prepend the . to prefix or suffix
        if not (suffix == ""):
            suffix = "." + suffix
        if not (prefix == ""):
            prefix = prefix + "."

        # Regenerate the sources file in the workspace. Do not generate
        # OpenMP-style output since we have already added OpenMP
        # decorations:
        self.workspace.props.PRETTYPRINT_SEQUENTIAL_STYLE = "do"
        # The default place is fine for us since we work later on the files:
        self.workspace.save()

        # Create the folder for p4a new files
        output_dir = os.path.join(os.getcwd(), "p4a_new_files")
        if dest_dir:
            output_dir = os.path.join(dest_dir,"p4a_new_files")
        if not (os.path.isdir(output_dir)):
            os.makedirs (output_dir)

        # save the user files
        output_files.extend (self.save_user_file (dest_dir, prefix, suffix))
        # save pips generated files in the dedicated folder
        output_files.extend (self.save_crough (output_dir))
        output_files.extend (self.save_interface (output_dir))
        output_files.extend (self.save_generated (output_dir))

        return output_files


    def __del__(self):
        # Waiting for pyps.workspace.close!
        if self.workspace:
            del self.workspace


if __name__ == "__main__":
    print(__doc__)
    print("This module is not directly executable")

# Some Emacs stuff:
### Local Variables:
### mode: python
### mode: flyspell
### ispell-local-dictionary: "american"
### tab-width: 4
### End:
