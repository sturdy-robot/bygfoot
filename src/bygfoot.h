/*
  bygfoot.h

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

#ifndef BYGFOOT_H
#define BYGFOOT_H

//#define MAC_BUILD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAC_BUILD
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#else
#  include "config.h"
#include <Carbon/Carbon.h>
#endif

#include <gtk/gtk.h>
#include "gettext_macros.h"
#include "debug.h"
#include "bygfoot_typedefs.h"
#include "user_struct.h"
#include "bygfoot_struct.h"

/**
 * Program version number and year (copyright).
 */
#ifndef VERS
#define VERS "2.4.0"
#endif
#define YEAR "2005 - 2011"

/** Home dir name */
//#define HOMEDIRNAME ".bygfoot-cvs"
#define HOMEDIRNAME ".bygfoot"

/**
 * Convenience macros, used for string sizes (typically buf[SMALL]).
 */
#define SMALL 10000
#define BIG 1000000

/** Starting numbers of league, cup and supercup numerical ids. */
#define ID_LEAGUE_START 1000
#define ID_CUP_START 7000

/** Whether we are using a Unix system or Windows. */
#ifdef G_OS_WIN32
#define os_is_unix FALSE
#else
#define os_is_unix TRUE
#endif

#define player_id_new (counters[COUNT_PLAYER_ID]++)
#define team_id_new (counters[COUNT_TEAM_ID]++)
#define cup_id_new (counters[COUNT_CUP_ID]++)
#define league_id_new (counters[COUNT_LEAGUE_ID]++)
#define fixture_id_new (counters[COUNT_FIX_ID]++)
#define lg_commentary_id_new (counters[COUNT_LG_COMM_ID]++)
#define news_title_id_new (counters[COUNT_NEWS_TITLE_ID]++)
#define news_subtitle_id_new (counters[COUNT_NEWS_SUBTITLE_ID]++)
#define news_article_id_new (counters[COUNT_NEWS_ARTICLE_ID]++)

/** Convenience abbrevs. */
#define stat1 status[1]
#define stat2 status[2]
#define stat3 status[3]
#define stat4 status[4]
#define stat5 status[5]

#define debug debug_level
#define debug_writer const_int("int_debug_writer")
/* Uncommenting this will cause each function to print its name when it's entered.
   Makes the whole program really slow, of course. */
//#define DEBUG

/**
 * Exit codes.
 */
enum ExitCodes
{
    EXIT_OK = 0, /**< Normal exit. */
    EXIT_GENERAL, /**< A general, multi-purpose exit code. */
    EXIT_NO_COUNTRY_FILES, /**< Exit when no country files have been found. */
    EXIT_FILE_OPEN_FAILED, /**< A file could not be opened. */
    EXIT_PRINT_ERROR, /**< Exit when the print_error function is called on a set error.*/
    EXIT_NO_LEAGUES, /**< The game must contain at least one league. */
    EXIT_CHOOSE_TEAM_ERROR, /**< There was a problem loading the choose_teams. @see cup_load_choose_teams() */
    EXIT_FIXTURE_WRITE_ERROR, /**< There was an error writing the fixtures. */
    EXIT_USER_FIRED, /**< Single user was fired and didn't accept the new offer. */
    EXIT_FIRST_WEEK_ERROR, /**< First week of cup was negative. */
    EXIT_OPTION_NOT_FOUND, /**< An option couldn't be found. */
    EXIT_POINTER_NOT_FOUND, /**< We didn't find a pointer needed. */
    EXIT_INT_NOT_FOUND, /**< We didn't find an integer value (mostly indices). */
    EXIT_FILE_NOT_FOUND, /**< Some file couldn't be found. */
    EXIT_NO_SUPPORT_DIR, /**< No support directory found. */
    EXIT_CUP_ROUND_ERROR, /**< Too few cup rounds. */
    EXIT_LOAD_TEAM_DEF, /**< Error loading a team definition file. */
    EXIT_DEF_SORT, /**< Error sorting a team after loading the definition. */
    EXIT_PROM_REL, /**< Error when executing the promotion/relegation stuff. */
    EXIT_STRATEGY_ERROR, /**< Error related to CPU strategies. */
    EXIT_BET_ERROR, /**< Error related to betting. */
    EXIT_END
};

/**
 * An enum representing a countries promotion rules for reserve teams.
 */
enum ReservePromRules
{
   RESERVE_PROM_RULES_NONE = 0,
   RESERVE_PROM_RULES_DEFAULT
};

/**
 * A struct representing a country.
 */
typedef struct country
{
    gchar *name, /**< Name of the country. */
	*symbol, /**< Symbol of the country, eg a flag pixmap. */
	*sid; /**< Id of the country, eg 'england'. */

    /** A rating point from 0-10 telling us
	how good the first league of the country is.
	Spain, for instance, has rating 10, whereas Ireland has only 5. */
    gint rating;

    enum ReservePromRules reserve_promotion_rules;

    /** Leagues and cups arrays. */
    GPtrArray *leagues;
    GPtrArray *cups;
    
    /** Pointer array holding all cups. */
    GPtrArray *allcups;

    Bygfoot *bygfoot;
} Country;

/** Struct used for having all the windows
    in one place. */
typedef struct
{
    GtkWidget *main,
	*startup,
	*live,
	*warning,
	*progress,
	*digits,
	*stadium,
	*job_offer,
	*yesno,
	*options,
        *constants,
	*font_sel,
	*file_chooser,
	*contract,
	*menu_player,
	*menu_youth,
	*user_management,
	*wdebug,
	*help,
	*transfer_dialog,
	*sponsors,
	*mmatches,
	*bets,
	*splash,
	*training_camp,
	*news,
        *alr,
        *strategy;
    
    /** The position of the horizontal divider is stored here. */
    gint paned_pos;
} Windows;

typedef struct {
    gchar *json_filename;
} CommandLineArgs;

void bygfoot_init(Bygfoot *bygfoot, enum BygfootFrontend frontend);
Country *bygfoot_load_country(Bygfoot *bygfoot, const gchar *country_name);
User *bygfoot_add_user(Bygfoot *bygfoot, const gchar *username, Team *tm);
void bygfoot_start_game(Bygfoot *bygfoot);
void bygfoot_show_progress(const Bygfoot *bygfoot, gfloat value, const gchar *text, gint pictype);
gdouble bygfoot_get_progress_bar_fraction(const Bygfoot *bygfoot);

Competition *
bygfoot_get_competition_id(const Bygfoot *bygfoot, int id);
#endif
