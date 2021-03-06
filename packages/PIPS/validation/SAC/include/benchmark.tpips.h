activate MUST_REGIONS
activate REGION_CHAINS
activate RICE_REGIONS_DEPENDENCE_GRAPH
activate PRECONDITIONS_INTER_FULL
activate TRANSFORMERS_INTER_FULL

setproperty LOOP_UNROLL_WITH_PROLOGUE FALSE
setproperty CONSTANT_PATH_EFFECTS FALSE
setproperty RICEDG_STATISTICS_ALL_ARRAYS TRUE
setproperty C89_CODE_GENERATION TRUE

setproperty SIMD_FORTRAN_MEM_ORGANISATION=FALSE
setproperty SAC_SIMD_REGISTER_WIDTH = 128
setproperty SIMDIZER_AUTO_UNROLL_SIMPLE_CALCULATION FALSE
setproperty SIMDIZER_AUTO_UNROLL_MINIMIZE_UNROLL FALSE
setproperty PRETTYPRINT_ALL_DECLARATIONS TRUE
setproperty COMPUTE_ALL_DEPENDENCES TRUE
#setproperty IF_CONVERSION_PHI "SIMD_PHI"

apply SPLIT_UPDATE_OPERATOR

apply IF_CONVERSION_INIT
display PRINTED_FILE

apply IF_CONVERSION
display PRINTED_FILE

apply IF_CONVERSION_COMPACT
#apply USE_DEF_ELIMINATION
display PRINTED_FILE

apply PARTIAL_EVAL
apply SIMD_ATOMIZER
display PRINTED_FILE

apply SIMDIZER_AUTO_UNROLL
apply PARTIAL_EVAL
apply CLEAN_DECLARATIONS
apply SUPPRESS_DEAD_CODE
display PRINTED_FILE
#make DOTDG_FILE
apply SIMD_REMOVE_REDUCTIONS
display PRINTED_FILE

#apply DEATOMIZER
#apply PARTIAL_EVAL
#apply USE_DEF_ELIMINATION
#display PRINTED_FILE

#make DOTDG_FILE
apply SCALAR_RENAMING

display PRINTED_FILE

#activate PRINT_DOT_DEPENDENCE_GRAPH
#make DOTDG_FILE
apply SIMDIZER

display PRINTED_FILE

#apply USE_DEF_ELIMINATION
#display PRINTED_FILE

apply DELAY_COMMUNICATIONS_INTRA
#setproperty EOLE_OPTIMIZATION_STRATEGY "ICM"
#apply OPTIMIZE_EXPRESSIONS
#apply PARTIAL_REDUNDANCY_ELIMINATION
display PRINTED_FILE

#apply USE_DEF_ELIMINATION
apply SUPPRESS_DEAD_CODE
apply REDUNDANT_LOAD_STORE_ELIMINATION
apply SUPPRESS_DEAD_CODE
apply CLEAN_DECLARATIONS
display PRINTED_FILE

