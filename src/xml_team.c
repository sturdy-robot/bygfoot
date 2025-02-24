/*
   xml_team.c

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

#include "file.h"
#include "free.h"
#include "main.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "variables.h"
#include "xml_team.h"

/**
 * The tags used in the XML files defining teams.
 */
#define TAG_TEAM "team"
#define TAG_TEAM_NAME "team_name"
#define TAG_STADIUM_NAME "stadium_name"
#define TAG_SYMBOL "symbol"
#define TAG_AVERAGE_TALENT "average_talent"
#define TAG_FORMATION "formation"
#define TAG_NAMES_FILE "names_file"
#define TAG_FIRST_TEAM "first_team"
#define TAG_RESERVE_LEVEL "reserve_level"
#define TAG_PLAYER "player"
#define TAG_PLAYER_NAME "player_name"
#define TAG_PLAYER_BIRTH_YEAR "birth_year"
#define TAG_PLAYER_BIRTH_MONTH "birth_month"
#define TAG_PLAYER_SKILL "skill"
#define TAG_PLAYER_TALENT "talent"
#define TAG_PLAYER_POSITION "position"

enum XmlTeamStates
{
    STATE_TEAM = 0,
    STATE_TEAM_NAME,
    STATE_STADIUM_NAME,
    STATE_SYMBOL,
    STATE_AVERAGE_TALENT,
    STATE_FORMATION,
    STATE_NAMES_FILE,
    STATE_FIRST_TEAM,
    STATE_RESERVE_LEVEL,
    STATE_PLAYER,
    STATE_PLAYER_NAME,
    STATE_PLAYER_BIRTH_YEAR,
    STATE_PLAYER_BIRTH_MONTH,
    STATE_PLAYER_SKILL,
    STATE_PLAYER_TALENT,
    STATE_PLAYER_POSITION,
    STATE_END
};

typedef struct {
    gint state, birth_year;
    Player new_player;
    Team *team;
    const gchar *d_file;
} TeamUserData;

void
xml_team_read_start_element (GMarkupParseContext *context,
			    const gchar         *element_name,
			    const gchar        **attribute_names,
			    const gchar        **attribute_values,
			    gpointer             user_data,
			    GError             **error)
{
#ifdef DEBUG
    printf("xml_team_read_start_element\n");
#endif
    TeamUserData *team_user_data = (TeamUserData*)user_data;

    if(strcmp(element_name, TAG_TEAM) == 0)
	team_user_data->state = STATE_TEAM;
    else if(strcmp(element_name, TAG_TEAM_NAME) == 0)
	team_user_data->state = STATE_TEAM_NAME;
    else if(strcmp(element_name, TAG_STADIUM_NAME) == 0)
	team_user_data->state = STATE_STADIUM_NAME;
    else if(strcmp(element_name, TAG_SYMBOL) == 0)
	team_user_data->state = STATE_SYMBOL;
    else if(strcmp(element_name, TAG_AVERAGE_TALENT) == 0)
	team_user_data->state = STATE_AVERAGE_TALENT;
    else if(strcmp(element_name, TAG_FORMATION) == 0)
	team_user_data->state = STATE_FORMATION;
    else if(strcmp(element_name, TAG_NAMES_FILE) == 0)
	team_user_data->state = STATE_NAMES_FILE;
    else if(strcmp(element_name, TAG_FIRST_TEAM) == 0)
	team_user_data->state = STATE_FIRST_TEAM;
    else if(strcmp(element_name, TAG_RESERVE_LEVEL) == 0)
	team_user_data->state = STATE_RESERVE_LEVEL;
    else if(strcmp(element_name, TAG_PLAYER) == 0)
    {
	team_user_data->state = STATE_PLAYER;
	team_user_data->new_player = player_new(team_user_data->team, ((gfloat)team_user_data->team->average_talent / 10000) * 
				const_float_fast(float_player_max_skill), TRUE);
    }
    else if(strcmp(element_name, TAG_PLAYER_NAME) == 0)
	team_user_data->state = STATE_PLAYER_NAME;
    else if(strcmp(element_name, TAG_PLAYER_BIRTH_YEAR) == 0)
	team_user_data->state = STATE_PLAYER_BIRTH_YEAR;
    else if(strcmp(element_name, TAG_PLAYER_BIRTH_MONTH) == 0)
	team_user_data->state = STATE_PLAYER_BIRTH_MONTH;
    else if(strcmp(element_name, TAG_PLAYER_SKILL) == 0)
	team_user_data->state = STATE_PLAYER_SKILL;
    else if(strcmp(element_name, TAG_PLAYER_TALENT) == 0)
	team_user_data->state = STATE_PLAYER_TALENT;
    else if(strcmp(element_name, TAG_PLAYER_POSITION) == 0)
	team_user_data->state = STATE_PLAYER_POSITION;
    else
	debug_print_message("xml_team_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, team_user_data->state);
}

/**
 * The function called by the parser when a closing tag is read.
 * The state variable is changed in this function.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_team_read_end_element    (GMarkupParseContext *context,
			      const gchar         *element_name,
			      gpointer             user_data,
			      GError             **error)
{
#ifdef DEBUG
    printf("xml_team_read_end_element\n");
#endif
    TeamUserData *team_user_data = (TeamUserData*)user_data;

    if(strcmp(element_name, TAG_TEAM_NAME) == 0 ||
       strcmp(element_name, TAG_STADIUM_NAME) == 0 ||
       strcmp(element_name, TAG_SYMBOL) == 0 ||
       strcmp(element_name, TAG_AVERAGE_TALENT) == 0 ||
       strcmp(element_name, TAG_FORMATION) == 0 ||
       strcmp(element_name, TAG_NAMES_FILE) == 0 ||
       strcmp(element_name, TAG_FIRST_TEAM) == 0 ||
       strcmp(element_name, TAG_RESERVE_LEVEL) == 0 ||
       strcmp(element_name, TAG_PLAYER) == 0)
    {
	team_user_data->state = STATE_TEAM;
	if(strcmp(element_name, TAG_PLAYER) == 0)
	{
	    if(team_user_data->team->players->len == const_int("int_team_cpu_players"))
	    {
		free_player(&team_user_data->new_player);
		main_exit_program(EXIT_LOAD_TEAM_DEF, 
				  "xml_team_read_end_element: too many players in team definition '%s' (only %d allowed).",
				  team_user_data->d_file, const_int("int_team_cpu_players"));
	    }
	    else
		g_array_append_val(team_user_data->team->players, team_user_data->new_player);
	}
    }
    else if(strcmp(element_name, TAG_PLAYER_NAME) == 0 ||
	    strcmp(element_name, TAG_PLAYER_BIRTH_YEAR) == 0 ||
	    strcmp(element_name, TAG_PLAYER_BIRTH_MONTH) == 0 ||
	    strcmp(element_name, TAG_PLAYER_SKILL) == 0 ||
	    strcmp(element_name, TAG_PLAYER_TALENT) == 0 ||
	    strcmp(element_name, TAG_PLAYER_POSITION) == 0)
	team_user_data->state = STATE_PLAYER;
    else if(strcmp(element_name, TAG_TEAM) != 0)
	debug_print_message("xml_team_read_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, team_user_data->state);
}

/**
 * The function called by the parser when the text between tags is read.
 * This function is responsible for filling in the variables (e.g. team names)
 * when a file gets loaded.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_team_read_text         (GMarkupParseContext *context,
			   const gchar         *text,
			   gsize                text_len,  
			   gpointer             user_data,
			   GError             **error)
{
#ifdef DEBUG
    printf("xml_team_read_text\n");
#endif

    gchar buf[text_len + 1];
    gint int_value;
    gfloat float_value;
    TeamUserData *team_user_data = (TeamUserData*)user_data;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);
    float_value = (gfloat)g_ascii_strtod(buf, NULL);

    if(team_user_data->state == STATE_TEAM_NAME)
	misc_string_assign(&team_user_data->team->name, buf);
    else if(team_user_data->state == STATE_STADIUM_NAME)
    {
	g_free(team_user_data->team->stadium.name);
	misc_string_assign(&team_user_data->team->stadium.name, buf);
    }
    else if(team_user_data->state == STATE_SYMBOL)
	misc_string_assign(&team_user_data->team->symbol, buf);
    else if(team_user_data->state == STATE_AVERAGE_TALENT && opt_int("int_opt_load_defs") == 1)
	team_user_data->team->average_talent = 
	    (float_value / 10000) * const_float_fast(float_player_max_skill);
    else if(team_user_data->state == STATE_FORMATION)
	team_user_data->team->structure = int_value;
    else if(team_user_data->state == STATE_NAMES_FILE)
	misc_string_assign(&team_user_data->team->names_file, buf);
    else if(team_user_data->state == STATE_FIRST_TEAM)
	misc_string_assign(&team_user_data->team->first_team.sid, buf);
    else if(team_user_data->state == STATE_RESERVE_LEVEL)
	team_user_data->team->reserve_level = int_value;
    else if(team_user_data->state == STATE_PLAYER_NAME)
	misc_string_assign(&team_user_data->new_player.name, buf);
    else if(team_user_data->state == STATE_PLAYER_BIRTH_YEAR && opt_int("int_opt_load_defs") == 1)
	team_user_data->birth_year = int_value;
    else if(team_user_data->state == STATE_PLAYER_BIRTH_MONTH && opt_int("int_opt_load_defs") == 1)
	team_user_data->new_player.age = misc_get_age_from_birth(team_user_data->birth_year, int_value);
    else if(team_user_data->state == STATE_PLAYER_SKILL && opt_int("int_opt_load_defs") == 1)
	team_user_data->new_player.skill = ((gfloat)int_value / 10000) * 
	    const_float_fast(float_player_max_skill);
    else if(team_user_data->state == STATE_PLAYER_TALENT && opt_int("int_opt_load_defs") == 1)
	team_user_data->new_player.talent = ((gfloat)int_value / 10000) * 
	    const_float_fast(float_player_max_skill);
    else if(team_user_data->state == STATE_PLAYER_POSITION)
	team_user_data->new_player.pos = int_value;
}

/** Parse a team definition file and write the team accordingly. */
void
xml_team_read(Team *tm, const gchar *def_file)
{
#ifdef DEBUG
    printf("xml_team_read\n");
#endif

    GMarkupParser parser = {xml_team_read_start_element,
			    xml_team_read_end_element,
			    xml_team_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;
    TeamUserData user_data;

    memset(&user_data, 0, sizeof(user_data));

    user_data.team = tm;
    user_data.d_file = def_file;

    context = 
	g_markup_parse_context_new(&parser, 0, &user_data, NULL);

    if(!g_file_get_contents(def_file, &file_contents, &length, &error))
    {
	debug_print_message("xml_team_read: error reading file %s\n", def_file);
	misc_print_error(&error, FALSE);
	return;
    }

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);	
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_critical("xml_team_read: error parsing file %s\n", def_file);
	misc_print_error(&error, TRUE);
    }

    team_complete_def(user_data.team);
}
