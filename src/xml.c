/*
   xml.c

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

#include "cup.h"
#include "file.h"
#include "free.h"
#include "gui.h"
#include "league.h"
#include "misc.h"
#include "option.h"
#include "support.h"
#include "table.h"
#include "transfer_struct.h"
#include "user.h"
#include "variables.h"
#include "xml.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_fixtures.h"
#include "xml_loadsave_league.h"
#include "xml_loadsave_league_stat.h"
#include "xml_loadsave_live_game.h"
#include "xml_loadsave_table.h"
#include "xml_loadsave_teams.h"
#include "xml_loadsave_transfers.h"
#include "xml_loadsave_users.h"

void
xml_load_users(const gchar *dirname, const gchar *basename)
{
#ifdef DEBUG
    printf("xml_load_users\n");
#endif

    gint i;
    gchar buf[SMALL];

    free_users(TRUE);

    xml_loadsave_users_read(dirname, basename);

    for(i=0;i<users->len;i++)
    {
	sprintf(buf, "%s%s%s___user_%02d_options", 
		dirname, G_DIR_SEPARATOR_S, basename, i);
	file_load_opt_file(buf, &usr(i).options, FALSE);

	sprintf(buf, "%s%s%s___user_%02d_live_game.xml", 
		dirname, G_DIR_SEPARATOR_S, basename, i);
	xml_loadsave_live_game_read(buf, &usr(i).live_game);
    }
}

void
xml_load_league(Country *country, GPtrArray *league_list, const gchar *dirname, const gchar *basename)
{
#ifdef DEBUG
    printf("xml_load_league\n");
#endif

    gchar buf[SMALL], team_file[SMALL];
    League new = league_new(FALSE, country);
    gchar *prefix = g_strndup(basename, strlen(basename) - 4);
    League *league = g_malloc0(sizeof(League));
    *league = new;

    g_ptr_array_add(league_list, league);

    sprintf(buf, "%s%s%s", dirname, G_DIR_SEPARATOR_S, basename);
    sprintf(team_file, "%s%s%s_teams.xml", dirname, G_DIR_SEPARATOR_S, prefix);
    xml_loadsave_league_read(buf, team_file, league);

    sprintf(buf, _("Loading league: %s"), league->c.name);

    bygfoot_show_progress(country->bygfoot, bygfoot_get_progress_bar_fraction(country->bygfoot), buf,
                      PIC_TYPE_LOAD);

    if(debug > 80)
	g_print("%s\n", buf);

    sprintf(buf, "%s%s%s_fixtures.xml", dirname, G_DIR_SEPARATOR_S, prefix);
    xml_loadsave_fixtures_read(buf, &league->c, league->fixtures);

    sprintf(buf, "%s%s%s_stat.xml", dirname, G_DIR_SEPARATOR_S, prefix);
    xml_loadsave_league_stat_read(buf, &league->stats);

    g_free(prefix);
}

void
xml_load_cup(Bygfoot *bygfoot, Cup *cup, const gchar *dirname, const gchar *basename)
{
#ifdef DEBUG
    printf("xml_load_cup\n");
#endif

    gchar buf[SMALL];
    gchar *prefix = g_strndup(basename, strlen(basename) - 4);

    sprintf(buf, "%s%s%s", dirname, G_DIR_SEPARATOR_S, basename);
    xml_loadsave_cup_read(buf, cup);

    sprintf(buf, _("Loading cup: %s"),
	    cup->c.name);
    bygfoot_show_progress(bygfoot, bygfoot_get_progress_bar_fraction(bygfoot), buf,
                      PIC_TYPE_LOAD);

    if(debug > 80)
	g_print("%s\n", buf);

    sprintf(buf, "%s%s%s_fixtures.xml", dirname, G_DIR_SEPARATOR_S, prefix);
    xml_loadsave_fixtures_read(buf, &cup->c, cup->fixtures);

    g_free(prefix);
}

void
xml_load_transfers(const gchar *dirname, const gchar *basename)
{
#ifdef DEBUG
    printf("xml_load_transfers\n");
#endif

    gchar buf[SMALL];

    sprintf(buf, "%s%s%s___transfer_list.xml", dirname, G_DIR_SEPARATOR_S, basename);

    free_g_array(&transfer_list);
    transfer_list = g_array_new(FALSE, FALSE, sizeof(Transfer));

    xml_loadsave_transfers_read(buf);
}

void
xml_write_string(FILE *fil, const gchar *string, gint tag, const gchar* indent)
{
#ifdef DEBUG
    printf("xml_write_string\n");
#endif

    if(string == NULL)
	return;

    fprintf(fil, "%s<_%d>%s</_%d>\n", indent, tag, string, tag);
}

void
xml_write_float(FILE *fil, float value, gint tag, const gchar* indent)
{
    gint ivalue = 0;

    memcpy(&ivalue, &value, sizeof(gint));
    /* TODO: Drop 0x prefix once old save format is no longer supported. */
    fprintf(fil, "%s<_%d>0x%x</_%d>\n", indent, tag, ivalue, tag);
}

gfloat
xml_read_float(const char *str)
{
    gfloat fvalue = 0.0f;
    gint ivalue = 0;
    if (str[0] != '0') {
        /* Old save format. */
        return (((gfloat)(g_ascii_strtoll(str, NULL, 10)) / 10000));
    }

    ivalue = g_ascii_strtoll(str, NULL, 16);
    memcpy(&fvalue, &ivalue, sizeof(gfloat));
    return fvalue;
}
