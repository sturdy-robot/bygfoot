/*
   load_save.c

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <ctype.h>
#include <glib/gstdio.h>

#include "callbacks.h"
#include "cup.h"
#include "file.h"
#include "free.h"
#include "gui.h"
#include "game_gui.h"
#include "language.h"
#include "load_save.h"
#include "misc.h"
#include "option.h"
#include "support.h"
#include "team.h"
#include "user.h"
#include "variables.h"
#include "window.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_jobs.h"
#include "xml_loadsave_league.h"
#include "xml_loadsave_leagues_cups.h"
#include "xml_loadsave_misc.h"
#include "xml_loadsave_newspaper.h"
#include "xml_loadsave_season_stats.h"
#include "xml_loadsave_transfers.h"
#include "xml_loadsave_users.h"
#include "xml.h"

#define PROGRESS_MAX 9

/** Save the game to the specified file. */
void
load_save_save_game(Bygfoot *bygfoot, const gchar *filename)
{
#ifdef DEBUG
    printf("load_save_save_game\n");
#endif

    gchar buf[SMALL];
    gchar *prefix = (g_str_has_suffix(filename, const_str("string_fs_save_suffix"))) ?
                    g_strndup(filename, strlen(filename) - strlen(const_str("string_fs_save_suffix"))) :
                    g_strdup(filename);
    GString *fullname = g_string_new("");

    g_string_append(fullname,(g_str_has_suffix(filename, const_str("string_fs_save_suffix"))) ?
                    g_strdup(filename) :
                    g_strdup_printf("%s%s", filename, const_str("string_fs_save_suffix")));

    misc_string_assign(&save_file, fullname->str);

    if(g_file_test(fullname->str, G_FILE_TEST_EXISTS))
        g_remove(fullname->str);

    if(debug > 60)
        g_print("load_save_save options\n");

    bygfoot_show_progress(bygfoot, 0, _("Saving options..."),
                      PIC_TYPE_SAVE);

    sprintf(buf, "%s___options", prefix);
    file_save_opt_file(buf, &options);
    sprintf(buf, "%s___settings", prefix);
    file_save_opt_file(buf, &settings);

    if(debug > 60)
        g_print("load_save_save leagues/cups \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Saving leagues and cups..."),
        PIC_TYPE_SAVE);

    xml_loadsave_leagues_cups_write(prefix);

    if(debug > 60)
        g_print("load_save_save users \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Saving users..."),
        PIC_TYPE_SAVE);

    xml_loadsave_users_write(prefix);

    if(debug > 60)
        g_print("load_save_save transfers \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Saving transfer list..."),
        PIC_TYPE_SAVE);

    xml_loadsave_transfers_write(prefix);

    if(debug > 60)
        g_print("load_save_save stats \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Saving season stats..."),
        PIC_TYPE_SAVE);

    xml_loadsave_season_stats_write(prefix);

    if(debug > 60)
        g_print("load_save_save jobs \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        /* The 'job exchange' is a list of teams looking for a manager. */
        _("Saving job exchange..."),
        PIC_TYPE_SAVE);

    xml_loadsave_jobs_write(prefix);

    if(debug > 60)
        g_print("load_save_save newspaper \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Saving newspaper..."),
        PIC_TYPE_SAVE);

    xml_loadsave_newspaper_write(prefix);

    if(debug > 60)
        g_print("load_save_save misc \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Saving miscellaneous..."),
        PIC_TYPE_SAVE);

    xml_loadsave_misc_write(bygfoot, prefix);

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Compressing savegame..."),
        PIC_TYPE_SAVE);

    sprintf(buf, "%s___", prefix);
    file_compress_files(fullname->str, buf);

    if(debug > 60)
        g_print("load_save_save done \n");

    bygfoot_show_progress(bygfoot, 1, _("Done."),
                      PIC_TYPE_SAVE);

    file_store_text_in_saves("last_save", fullname->str);

    g_free(prefix);
    g_string_free(fullname, TRUE);

    bygfoot_show_progress(bygfoot, -1, "",
                      PIC_TYPE_SAVE);
    if (bygfoot->frontend == BYGFOOT_FRONTEND_GTK2) {
        setsav1;
    }
}

static void
update_all_cups(void)
{
    gint i;
    GPtrArray *new_acps = g_ptr_array_new();
    for (i = 0; i < country.allcups->len; i++) {
        g_ptr_array_add(new_acps, cup_from_clid(GPOINTER_TO_INT(g_ptr_array_index(country.allcups, i))));
    }
    g_ptr_array_free(country.allcups, TRUE);
    country.allcups = new_acps;
}

static void
country_adjust_competition_pointers(Country *country)
{
    gint i;

    for (i = 0; i < country->leagues->len; i++) {
        gint j;
        League *league = g_ptr_array_index(country->leagues, i);
        for (j = 0; j < league->c.teams->len; j++) {
            gint k;
            Team *team = g_ptr_array_index(league->c.teams, j); 
            for (k = 0; k < team->players->len; k++) {
                gint x;
                Player *player = &g_array_index(team->players, Player, k);
                for (x = 0; x < player->stats->len; x++) {
                    PlayerCompetitionStats *stats = &g_array_index(player->stats, PlayerCompetitionStats, x);
                    gint clid = GPOINTER_TO_INT(stats->competition);
                    stats->competition = bygfoot_get_competition_id(country->bygfoot, clid);
                }
            }
        }
    }
}

static void
bygfoot_adjust_competition_pointers(Bygfoot *bygfoot)
{
    gint i;
    country_adjust_competition_pointers(&country);

    for (i = 0; i < country_list->len; i++) {
        Country *c = g_ptr_array_index(country_list, i);
        country_adjust_competition_pointers(c);
    }
}

static void
country_adjust_first_team_ptrs(Country *country)
{
    gint i;
    for (i = 0; i < country->leagues->len; i++) {
        League *league = g_ptr_array_index(country->leagues, i);
        gint j;
        for (j = 0; j < league->c.teams->len; j++) {
            Team *team = g_ptr_array_index(league->c.teams, j);
            if (!team_is_reserve_team(team)) {
                team->first_team.team = team;
            } else {
                team->first_team.team = team_of_id(team->first_team.id);
            }
        }
    }
}

static void
bygfoot_adjust_first_team_pointers(Bygfoot *bygfoot)
{
    gint i;
    country_adjust_first_team_ptrs(&country);

    for (i = 0; i < country_list->len; i++) {
        Country *c = g_ptr_array_index(country_list, i);
        country_adjust_first_team_ptrs(c);
    }
}

/** Load the game from the specified file.
    @param create_main_window Whether to create and show the main window. */
gboolean
load_save_load_game(Bygfoot *bygfoot, const gchar* filename, gboolean create_main_window)
{
#ifdef DEBUG
    printf("load_save_load_game\n");
#endif

    GString *buf = g_string_new("");
    gchar *fullname = (g_str_has_suffix(filename, const_str("string_fs_save_suffix"))) ?
                      g_strdup(filename) :
                      g_strdup_printf("%s%s", filename, const_str("string_fs_save_suffix"));
    gchar *basename = g_path_get_basename(fullname),
                      *dirname = g_path_get_dirname(fullname);
    gchar *prefix = (g_str_has_suffix(basename, const_str("string_fs_save_suffix"))) ?
                    g_strndup(basename, strlen(basename) - strlen(const_str("string_fs_save_suffix"))) :
                    g_strdup(basename);
    gchar *pwd = g_get_current_dir();

    if (!country.bygfoot)
        country.bygfoot = bygfoot;
    if(g_str_has_suffix(filename, "last_save"))
    {
        g_free(basename);
        g_free(dirname);
        g_free(prefix);
        g_free(fullname);

        basename = file_load_text_from_saves("last_save");

        if(basename != NULL)
        {
            load_save_load_game(bygfoot, basename, create_main_window);
            g_free(basename);
            return TRUE;
        }
        else
        {
            game_gui_show_warning(_("Last save file not found."));
            return FALSE;
        }
    }

    if(window.main != NULL)
        gtk_widget_hide(window.main);

    bygfoot_show_progress(bygfoot, 0, _("Uncompressing savegame..."),
                      PIC_TYPE_LOAD);

    file_decompress(fullname);

    if(debug > 60)
        g_print("load_save_load options\n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Loading options..."),
        PIC_TYPE_LOAD);

    g_string_printf(buf, "%s%s%s___options", dirname, G_DIR_SEPARATOR_S, prefix);
    file_load_opt_file(buf->str, &options, FALSE);
    g_string_printf(buf, "%s%s%s___settings", dirname, G_DIR_SEPARATOR_S, prefix);
    file_load_opt_file(buf->str, &settings, FALSE);

    if(debug > 60)
        g_print("load_save_load leagues \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Loading leagues and cups..."),
        PIC_TYPE_LOAD);

    xml_loadsave_leagues_cups_read(&country, dirname, prefix);

    if(debug > 60)
        g_print("load_save_load misc \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Loading miscellaneous..."),
        PIC_TYPE_LOAD);

    xml_loadsave_misc_read(bygfoot, dirname, prefix);

    /* Now that all the teams have been loaded, replace the team ids with team ptrs
     * where necessary. */
    xml_loadsave_leagues_cups_adjust_team_ptrs();
    xml_loadsave_leagues_cups_adjust_team_ptrs_cups(bygfoot->international_cups);
    update_all_cups();
    bygfoot_adjust_competition_pointers(bygfoot);
    bygfoot_adjust_first_team_pointers(bygfoot);

    if(debug > 60)
        g_print("load_save_load users \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Loading users..."),
        PIC_TYPE_LOAD);

    xml_load_users(dirname, prefix);

    if(debug > 60)
        g_print("load_save_load transfers \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Loading transfer list..."),
        PIC_TYPE_LOAD);

    xml_load_transfers(dirname, prefix);

    if(debug > 60)
        g_print("load_save_load stats \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Loading season stats..."),
        PIC_TYPE_LOAD);

    xml_loadsave_season_stats_read(dirname, prefix);

    if(debug > 60)
        g_print("load_save_load jobs \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        /* The 'job exchange' is a list of teams looking for a manager. */
        _("Loading job exchange..."),
        PIC_TYPE_LOAD);

    xml_loadsave_jobs_read(dirname, prefix);

    if(debug > 60)
        g_print("load_save_load newspaper \n");

    bygfoot_show_progress(bygfoot,
        ((PROGRESS_MAX * bygfoot_get_progress_bar_fraction(bygfoot)) + 1) / PROGRESS_MAX,
        _("Loading newspaper..."),
        PIC_TYPE_LOAD);

    xml_loadsave_newspaper_read(bygfoot, dirname, prefix);

    if(debug > 60)
        g_print("load_save_load done \n");

    bygfoot_show_progress(bygfoot, 1, _("Done."),
                      PIC_TYPE_LOAD);

    chdir(dirname);
    GPtrArray *files = file_dir_get_contents(dirname, prefix, "");
    // Remove the zipfile from the list
    gint i;
    for(i=0; i<files->len; i++)
    {
        if (g_strcmp0((gchar*)g_ptr_array_index(files, i),basename)==0)
        {
            g_ptr_array_remove_index_fast(files, i);
        }
    }
    file_remove_files(files);
    chdir(pwd);
    g_free(pwd);
    free_gchar_array(&files);


    misc_string_assign(&save_file, fullname);

    file_store_text_in_saves("last_save", fullname);

    bygfoot_show_progress(bygfoot, -1, "",
                      PIC_TYPE_LOAD);

    if(create_main_window)
    {
        window_create(WINDOW_MAIN, bygfoot);
        on_button_back_to_main_clicked(NULL, bygfoot);
    }
    else if(window.main != NULL)
    {
        gtk_widget_show(window.main);
        window_main_load_geometry();
    }

    g_string_free(buf, TRUE);

    g_free(basename);
    g_free(dirname);
    g_free(prefix);
    g_free(fullname);

    return TRUE;
}


/** Write an autosave. */
void
load_save_autosave(Bygfoot *bygfoot)
{
#ifdef DEBUG
    printf("load_save_autosave\n");
#endif

    gchar buf[SMALL], name[SMALL], directory[SMALL], prefix[SMALL];
    const gchar *home = g_get_home_dir();
    FILE *fil = NULL;
    gchar *pwd = g_get_current_dir();

    if(!opt_int("int_opt_autosave"))
        return;

    counters[COUNT_AUTOSAVE] = (counters[COUNT_AUTOSAVE] + 1) % opt_int("int_opt_autosave_interval");

    if(counters[COUNT_AUTOSAVE] != 0)
        return;

    load_save_write_autosave_name(name);

    if (!file_get_saves_dir(directory, SMALL))
        return;

    sprintf(name, "%s.zip", name);
    sprintf(buf, "%s%s%s", directory, G_DIR_SEPARATOR_S, name);

    if(!file_my_fopen(buf, "w", &fil, FALSE))
        return;

    fclose(fil);
    sprintf(prefix, "autosave_%02d_", counters[COUNT_AUTOSAVE_FILE]);
    load_save_save_game(bygfoot, buf);
    chdir(directory);
    GPtrArray *files = file_dir_get_contents(directory, prefix, ".zip");
    // Remove the zipfile from the list
    gint i;
    for(i=0; i<files->len; i++)
    {
        if (g_strcmp0((gchar*)g_ptr_array_index(files, i),name)==0)
        {
            g_ptr_array_remove_index_fast(files, i);
        }
    }
    file_remove_files(files);
    chdir(pwd);
    g_free(pwd);
    free_gchar_array(&files);


    counters[COUNT_AUTOSAVE_FILE] = (counters[COUNT_AUTOSAVE_FILE] + 1) % opt_int("int_opt_autosave_files");
}

/** Write the autosave file name which is put together from
    the user name, team name etc. into the parameter string. */
void
load_save_write_autosave_name(gchar *filename)
{
    gchar teamname[SMALL];
    gint i = 0;

    while(usr(0).tm->name[i] != '\0')
    {
        teamname[i] = (isspace(usr(0).tm->name[i]) == 0) ?
                      usr(0).tm->name[i] : '_';
        i++;
    }

    teamname[i] = '\0';

    sprintf(filename, "autosave_%02d_%s_%s_%s_S%02d_W%02d",
            counters[COUNT_AUTOSAVE_FILE], usr(0).name, country.name, teamname, season, week);
}

/** Try to load a savegame given on the command line. */
gboolean
load_game_from_command_line(Bygfoot *bygfoot, const gchar *filename)
{
#ifdef DEBUG
    printf("load_game_from_command_line\n");
#endif

    gchar *fullname = NULL,
                      *support_file_name = NULL;

    if(strcmp(filename, "last_save") == 0)
        return load_save_load_game(bygfoot, filename, TRUE);

    fullname = (g_str_has_suffix(filename, const_str("string_fs_save_suffix"))) ?
               g_strdup(filename) :
               g_strdup_printf("%s%s", filename, const_str("string_fs_save_suffix"));

    if(g_file_test(fullname, G_FILE_TEST_EXISTS))
    {
        if(load_save_load_game(bygfoot, fullname, TRUE))
        {
            g_free(fullname);
            return TRUE;
        }
        else
            return FALSE;
    }

    support_file_name = file_find_support_file(fullname, FALSE);

    if(g_file_test(support_file_name, G_FILE_TEST_EXISTS))
    {
        if(load_save_load_game(bygfoot, support_file_name, TRUE))
        {
            g_free(fullname);
            g_free(support_file_name);
            return TRUE;
        }
        else
            return FALSE;
    }

    debug_print_message("Could not find file %s.\n", fullname);
    g_free(fullname);

    return FALSE;
}
