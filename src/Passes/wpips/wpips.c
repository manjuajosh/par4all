#include <stdio.h>
#include <types.h>
#include <malloc.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/svrimage.h>

#include "genC.h"
#include "ri.h"
#include "database.h"
#include "graph.h"
#include "makefile.h"

#include "misc.h"
#include "ri-util.h"
#include "pipsdbm.h"
#include "properties.h"
#include "constants.h"
#include "resources.h"
#include "pipsmake.h"

#include "top-level.h"

#include "wpips.h"
#include "xv_sizes.h"

/* If we are in the Emacs mode, the log_frame is no longer really used: */
Frame main_frame, 
    schoose_frame, 
    mchoose_frame,
    log_frame, 
    edit_frame[MAX_NUMBER_OF_WPIPS_WINDOWS], 
    help_frame, 
    query_frame,
    options_frame;

Panel main_panel,
    status_panel,
    query_panel,
    mchoose_panel,
    schoose_panel,
    help_panel;

void static
create_menus()
{
   create_select_menu();
   create_edit_menu();
/*    create_analyze_menu();*/
   create_transform_menu();
   create_compile_menu();
   /* The option panel use the definition of the edit menu and so
      needs to be create after it: */
   create_options_menu_and_window();
   /* Gone in create_menus_end(): ...No ! */
   create_help_menu(); 
   /* In the Emacs mode, no XView log window: */
   /* In fact, create it but disabled to keep the same frame layout: */
   create_log_menu();
   create_quit_button();
}


/*
void static
create_menus_end()
{
   create_help_menu();
}
*/


static int first_mapping = TRUE;

void main_event_proc(window, event)
Xv_Window window;
Event *event;
{
    if (first_mapping == TRUE && event_id(event)==32526) {
		first_mapping = FALSE;

		/* we place all frames */
		place_frames();
    };
}


void
create_main_window()
{
    /* Xv_window main_window; */

    main_panel = xv_create(main_frame, PANEL,
			   NULL);

    /* The following mask is necessary to avoid that key events occure twice.
     */
    /*    main_window = (Xv_Window) xv_find(main_frame, WINDOW, 0);
	  xv_set(main_window,
	  WIN_IGNORE_EVENT, WIN_UP_ASCII_EVENTS, 
	  NULL);
	  */
    /* commented out as we shifted to xview.3, 92.04 */
/*    xv_set(canvas_paint_window(main_panel),
	   WIN_IGNORE_EVENT, WIN_UP_ASCII_EVENTS,
	   WIN_CONSUME_EVENT, 32526, 
	   WIN_EVENT_PROC, main_event_proc,
	   NULL);
*/
}


static unsigned short pips_bits[] = {
#include "pips.icon"
};

void create_icon()
{
    Server_image pips_image;
    Icon icon;
    Rect rect;

    pips_image = (Server_image)xv_create(NULL, SERVER_IMAGE, 
					 XV_WIDTH, 64,
					 XV_HEIGHT, 64,
					 SERVER_IMAGE_BITS, pips_bits, 
					 NULL);
    icon = (Icon)xv_create(NULL, ICON, 
			   ICON_IMAGE, pips_image,
			   NULL);
    rect.r_width= (int)xv_get(icon, XV_WIDTH);
    rect.r_height= (int)xv_get(icon, XV_HEIGHT);
    rect.r_left= 0;
    rect.r_top= 0;

    xv_set(main_frame, FRAME_ICON, icon, 
	   FRAME_CLOSED_RECT, &rect,
	   NULL);
}


void parse_arguments(argc, argv)
int argc;
char *argv[];
{
    string pname = NULL, mname = NULL, *files = NULL;
    int iarg = 1, nfiles;

    while (iarg < argc) {
	if (same_string_p(argv[iarg], "-emacs")) {
	    argv[iarg] = NULL;
            /* Wpips is called from emacs. RK. */
            wpips_emacs_mode = 1;
	}
	else if (same_string_p(argv[iarg], "-workspace")) {
	    argv[iarg] = NULL;
	    pname = argv[++iarg];
	}
	else if (same_string_p(argv[iarg], "-module")) {
	    argv[iarg] = NULL;
	    mname = argv[++iarg];
	}
	else if (same_string_p(argv[iarg], "-files")) {
	    argv[iarg] = NULL;
	    files = &argv[++iarg];
	    nfiles = argc-iarg;
	}
	else {
	    if (argv[iarg][0] == '-') {
		fprintf(stderr, "Usage: %s ", argv[0]);
		fprintf(stderr, "[ X-Window options ] [ -emacs ]");
		fprintf(stderr, "[ -workspace name [ -module name ] ");
		fprintf(stderr, "[ -files file1.f file2.f ... ] ]\n");
		exit(1);
	    }
	}

	iarg += 1;
    }

    if (pname != NULL) {
	if (files != NULL) {
	    db_create_program(pname);
	    create_program(&nfiles, files);
	}
	else {
	    open_program(pname);
	}

	if (mname != NULL) {
	    open_module(mname);
	}
    }
}


/* Try to inform the user about an XView error. For debug, use the
   WPIPS_DEBUG_LEVEL to have an abort on this kind
   of error. */
int static
wpips_xview_error(Xv_object object,
                  Attr_avlist avlist)
{
   debug_on("WPIPS_DEBUG_LEVEL");

   fprintf(stderr, "wpips_xview_error caught an error:\n%s\n",
           xv_error_format(object, avlist));
   /* Cannot use pips_assert since it uses XView, neither
      get_bool_property for the same reason: */
   if (get_debug_level() > 0) {
      fprintf(stderr, "wpips_xview_error is aborting as requested since WPIPS_DEBUG_LEVEL > 0...\n");
      abort();
   }
   
   debug_off();
   
   return XV_OK;
}


int
main(int argc,
     char * argv[])
{
   pips_warning_handler = wpips_user_warning;
   pips_error_handler = wpips_user_error;
   pips_log_handler = wpips_user_log;
   pips_update_props_handler = update_options;

   /* Added for debug. RK, 8/06/93. */
   malloc_debug(1);

   initialize_newgen();
   initialize_sc((char*(*)(Variable))entity_local_name);

   debug_on("WPIPS_DEBUG_LEVEL");

   /* we parse command line arguments */
   /* XV_ERROR_PROC unset as we shifted to xview.3, Apr. 92 */
   xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 
           XV_ERROR_PROC, wpips_xview_error,
           0);

   /* we parse remaining command line arguments */
   parse_arguments(argc, argv);

   /* we create all frames */
   create_frames();

   create_main_window();

   create_help_window();

   create_schoose_window();

   create_mchoose_window();

   create_query_window();

   if (! wpips_emacs_mode)
      /* Create the edit/view windows only if we are not in the Emacs
         mode: */
      create_edit_window();

   /* In the Emacs mode, no XView log window but we need it to compute
      the size of some other frames... */
   create_log_window();

   create_menus();

   create_status_subwindow();

   /* create_menus_end(); */

   create_icons();
   /*    create_icon();*/

   /* Call added. RK, 9/06/1993. */
   place_frames();

   /* If we are in the emacs mode, initialize some things: */
   initialize_emacs_mode();
   
   display_memory_usage();

   enable_workspace_create_or_open();
   enable_change_directory();
   disable_workspace_close();
   disable_module_selection();
   disable_view_selection();
   disable_transform_selection();
   disable_compile_selection();
   disable_option_selection();

   xv_main_loop(main_frame);

   close_log_file();
   
   debug_off();

   exit(0);
}
