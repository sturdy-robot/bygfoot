/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#include <time.h>

/*d*/
#include "file.h"
#include "free.h"
#include "main.h"
#include "variables.h"
#include "window.h"

/**
   Initialize some global variables. Most of them get nullified.
*/
void
main_init_variables(void)
{
    ligs = cps = NULL;
    country.name = country.symbol = country.sid = NULL;
    transfer_list = NULL;
    player_names = NULL;
    my_team = NULL;
    
    font_name = g_string_new("0");
    debug = FALSE;

    live_game.units = g_array_new(FALSE, FALSE, sizeof(LiveGameUnit));
    live_game.window = NULL;
    live_game_temp.units = g_array_new(FALSE, FALSE, sizeof(LiveGameUnit));
    live_game_temp.window = NULL;

    popups_active = 0;
}

/**
   Process the command line arguments and do some things
   that have to be done at the beginning (like initializing the
   random number generator).
   @param argc Number of command line arguments.
   @param argv Command line arguments array.
*/
void
main_init(gint argc, gchar *argv[])
{
    gchar buf[SMALL];
    gchar *pwd = g_get_current_dir();

    /* initialize the random nr generator */
    srandom((unsigned)time(NULL));

    file_add_support_directory_recursive(PACKAGE_DATA_DIR "/" PACKAGE "/support_files");  
    sprintf(buf, "%s/.bygfoot", g_get_home_dir());
    file_add_support_directory_recursive(buf);
    sprintf(buf, "%s/support_files", pwd);
    g_free(pwd);
    file_add_support_directory_recursive(buf);

    main_init_variables();
}

/**
   Initialize the GTK stuff and the gettext stuff.
   Start the game.
   @param argc Number of command line arguments.
   @param argv Command line arguments array.
*/
gint
main (gint argc, gchar *argv[])
{

#ifdef ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
#endif
    
  gtk_set_locale ();
  gtk_init (&argc, &argv);
  
  main_init(argc, argv);

  window_show_startup();

  gtk_main ();

  main_exit_program(EXIT_OK, NULL);

  return 0;
}

/** Exit the program with the given exit code and message. Try to
    destroy all widgets and free all memory first.
    @param exit_code The number we return to the shell.
    @param exit_message The message we print.
    @return The exit code of the program. */
void
main_exit_program(gint exit_code, gchar *exit_message)
{
    if(gtk_main_level() > 0)
	gtk_main_quit();

    free_memory();
    
    if(exit_message != NULL)
	g_warning(exit_message);
    
    exit(exit_code);
}