/*
   xml_loadsave_players.c

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

#include "misc.h"
#include "player.h"
#include "team.h"
#include "xml.h"
#include "xml_loadsave_players.h"

enum
{
    TAG_PLAYERS = TAG_START_PLAYERS,
    TAG_PLAYER,
    TAG_PLAYER_NAME,
    TAG_PLAYER_POS,
    TAG_PLAYER_CPOS,
    TAG_PLAYER_HEALTH,
    TAG_PLAYER_RECOVERY,
    TAG_PLAYER_ID,
    TAG_PLAYER_VALUE,
    TAG_PLAYER_WAGE,
    TAG_PLAYER_OFFERS,
    TAG_PLAYER_SKILL,
    TAG_PLAYER_CSKILL,
    TAG_PLAYER_TALENT,
    TAG_PLAYER_ETAL,
    TAG_PLAYER_FITNESS,
    TAG_PLAYER_LSU,
    TAG_PLAYER_AGE,
    TAG_PLAYER_PEAK_AGE,
    TAG_PLAYER_PEAK_REGION,
    TAG_PLAYER_CONTRACT,
    TAG_PLAYER_PARTICIPATION,
    TAG_PLAYER_GAMES_GOAL,
    TAG_PLAYER_GAMES_GOAL_CLID,
    TAG_PLAYER_GAMES_GOAL_GAMES,
    TAG_PLAYER_GAMES_GOAL_GOALS,
    TAG_PLAYER_GAMES_GOAL_SHOTS,
    TAG_PLAYER_CARD,
    TAG_PLAYER_CARD_CLID,
    TAG_PLAYER_CARD_YELLOW,
    TAG_PLAYER_CARD_RED,
    TAG_PLAYER_CAREER,
    TAG_PLAYER_STREAK,
    TAG_PLAYER_STREAK_COUNT,
    TAG_PLAYER_STREAK_PROB,
    TAG_PLAYER_STATS,
    TAG_PLAYER_STATS_CLID,
    TAG_PLAYER_STATS_GAMES,
    TAG_PLAYER_STATS_GOALS,
    TAG_PLAYER_STATS_SHOTS,
    TAG_PLAYER_STATS_YELLOW,
    TAG_PLAYER_STATS_RED,
    TAG_END
};

gint state, etalidx, careeridx;
Player *new_player;
/* For legacy saved games. */
PlayerCompetitionStats new_games_goals;
/* For legacy saved games. */
PlayerCompetitionStats new_card;
PlayerCompetitionStats new_stats;

void
xml_loadsave_players_start_element(gint tag, Team *tm, GArray *players)
{
#ifdef DEBUG
    printf("xml_loadsave_players_start_element\n");
#endif

    gint i;
    gboolean valid_tag = FALSE;

    for(i=TAG_PLAYERS;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    if(tag == TAG_PLAYER)
    {
        g_array_set_size(players, players->len + 1);
        new_player = &g_array_index(players, Player, players->len - 1);
	player_init(new_player);
	new_player->team = tm;
	etalidx = careeridx = 0;
    }

    if(!valid_tag)
	debug_print_message("xml_loadsave_players_start_element: unknown tag. I'm in state %d\n",
		  state);
}

void
xml_loadsave_players_end_element(gint tag)
{
#ifdef DEBUG
    printf("xml_loadsave_players_end_element\n");
#endif

    if(tag == TAG_PLAYER)
        return;
    else if(tag == TAG_PLAYER_NAME ||
	    tag == TAG_PLAYER_POS ||
	    tag == TAG_PLAYER_CPOS ||
	    tag == TAG_PLAYER_HEALTH ||
	    tag == TAG_PLAYER_RECOVERY ||
	    tag == TAG_PLAYER_ID ||
	    tag == TAG_PLAYER_VALUE ||
	    tag == TAG_PLAYER_WAGE ||
	    tag == TAG_PLAYER_OFFERS ||
	    tag == TAG_PLAYER_SKILL ||
	    tag == TAG_PLAYER_CSKILL ||
	    tag == TAG_PLAYER_TALENT ||
	    tag == TAG_PLAYER_ETAL ||
	    tag == TAG_PLAYER_FITNESS ||
	    tag == TAG_PLAYER_LSU ||
	    tag == TAG_PLAYER_AGE ||
	    tag == TAG_PLAYER_PEAK_AGE ||
	    tag == TAG_PLAYER_PEAK_REGION ||
	    tag == TAG_PLAYER_CONTRACT ||
	    tag == TAG_PLAYER_PARTICIPATION ||
	    tag == TAG_PLAYER_GAMES_GOAL ||
	    tag == TAG_PLAYER_STATS ||
	    tag == TAG_PLAYER_CAREER ||
	    tag == TAG_PLAYER_CARD ||
	    tag == TAG_PLAYER_STREAK ||
	    tag == TAG_PLAYER_STREAK_COUNT ||
	    tag == TAG_PLAYER_STREAK_PROB)
    {
	state = TAG_PLAYER;
	if(tag == TAG_PLAYER_ETAL)
	    etalidx++;
	else if(tag == TAG_PLAYER_CAREER)
	    careeridx++;
	else if(tag == TAG_PLAYER_CARD) {
            gint i;
            gboolean found = FALSE;
            for (i = 0; i < new_player->stats->len; i++) {
                PlayerCompetitionStats *stats = &g_array_index(new_player->stats, PlayerCompetitionStats, i);
                if (stats->competition != new_card.competition)
                    continue;
                stats->yellow = new_card.yellow;
                stats->red = new_card.red;
                found = TRUE;
                break;
            }
            if (!found)
      	        g_array_append_val(new_player->stats, new_card);
        } else if(tag == TAG_PLAYER_GAMES_GOAL) {
            gint i;
            gboolean found = FALSE;
            for (i = 0; i < new_player->stats->len; i++) {
                PlayerCompetitionStats *stats = &g_array_index(new_player->stats, PlayerCompetitionStats, i);
                if (stats->competition != new_games_goals.competition)
                    continue;
                stats->games = new_games_goals.games;
                stats->goals = new_games_goals.goals;
                stats->shots = new_games_goals.shots;
                found = TRUE;
                break;
            }
            if (!found)
 	        g_array_append_val(new_player->stats, new_games_goals);
        } else if(tag == TAG_PLAYER_STATS)
            g_array_append_val(new_player->stats, new_stats);
    }
    else if(tag == TAG_PLAYER_GAMES_GOAL_CLID ||
	    tag == TAG_PLAYER_GAMES_GOAL_GAMES ||
	    tag == TAG_PLAYER_GAMES_GOAL_GOALS ||
	    tag == TAG_PLAYER_GAMES_GOAL_SHOTS)
	state = TAG_PLAYER_GAMES_GOAL;
    else if(tag == TAG_PLAYER_CARD_CLID ||
	    tag == TAG_PLAYER_CARD_YELLOW ||
	    tag == TAG_PLAYER_CARD_RED)
	state = TAG_PLAYER_CARD;
    else if (tag == TAG_PLAYER_STATS_CLID ||
             tag == TAG_PLAYER_STATS_GAMES ||
             tag == TAG_PLAYER_STATS_GOALS ||
             tag == TAG_PLAYER_STATS_SHOTS ||
             tag == TAG_PLAYER_STATS_YELLOW ||
             tag == TAG_PLAYER_STATS_RED)
        state = TAG_PLAYER_STATS;
    else if(tag != TAG_PLAYERS)
	debug_print_message("xml_loadsave_players_end_element: unknown tag. I'm in state %d\n",
		  state);
}

void
xml_loadsave_players_text(gchar *text)
{
#ifdef DEBUG
    printf("xml_loadsave_players_text\n");
#endif
    if(state == TAG_PLAYER_NAME)
	misc_string_assign(&new_player->name, text);
    else if(state == TAG_PLAYER_POS)
	new_player->pos = xml_read_int(text);
    else if(state == TAG_PLAYER_CPOS)
	new_player->cpos = xml_read_int(text);
    else if(state == TAG_PLAYER_HEALTH)
	new_player->health = xml_read_int(text);
    else if(state == TAG_PLAYER_RECOVERY)
	new_player->recovery = xml_read_int(text);
    else if(state == TAG_PLAYER_ID)
	new_player->id = xml_read_int(text);
    else if(state == TAG_PLAYER_VALUE)
	new_player->value = xml_read_int(text);
    else if(state == TAG_PLAYER_WAGE)
	new_player->wage = xml_read_int(text);
    else if(state == TAG_PLAYER_OFFERS)
	new_player->offers = xml_read_int(text);
    else if(state == TAG_PLAYER_PARTICIPATION)
	new_player->participation = xml_read_int(text);
    else if(state == TAG_PLAYER_SKILL)
	new_player->skill = xml_read_float(text);
    else if(state == TAG_PLAYER_CSKILL)
	new_player->cskill = xml_read_float(text);
    else if(state == TAG_PLAYER_TALENT)
	new_player->talent = xml_read_float(text);
    else if(state == TAG_PLAYER_FITNESS)
	new_player->fitness = xml_read_float(text);
    else if(state == TAG_PLAYER_LSU)
	new_player->lsu = xml_read_float(text);
    else if(state == TAG_PLAYER_AGE)
	new_player->age = xml_read_float(text);
    else if(state == TAG_PLAYER_PEAK_AGE)
	new_player->peak_age = xml_read_float(text);
    else if(state == TAG_PLAYER_PEAK_REGION)
	new_player->peak_region = xml_read_float(text);
    else if(state == TAG_PLAYER_CONTRACT)
	new_player->contract = xml_read_float(text);
    else if(state == TAG_PLAYER_ETAL)
	new_player->etal[etalidx] = xml_read_float(text);
    else if(state == TAG_PLAYER_GAMES_GOAL_CLID)
	new_games_goals.competition = GINT_TO_POINTER(xml_read_int(text));
    else if(state == TAG_PLAYER_GAMES_GOAL_GAMES)
	new_games_goals.games = xml_read_int(text);
    else if(state == TAG_PLAYER_GAMES_GOAL_GOALS)
	new_games_goals.goals = xml_read_int(text);
    else if(state == TAG_PLAYER_GAMES_GOAL_SHOTS)
	new_games_goals.shots = xml_read_int(text);
    else if(state == TAG_PLAYER_CARD_CLID)
	new_card.competition = GINT_TO_POINTER(xml_read_int(text));
    else if(state == TAG_PLAYER_CARD_YELLOW)
	new_card.yellow = xml_read_int(text);
    else if(state == TAG_PLAYER_CARD_RED)
	new_card.red = xml_read_int(text);
    else if(state == TAG_PLAYER_STATS_CLID)
	new_stats.competition = GINT_TO_POINTER(xml_read_int(text));
    else if(state == TAG_PLAYER_STATS_GAMES)
	new_stats.games = xml_read_int(text);
    else if(state == TAG_PLAYER_STATS_GOALS)
	new_stats.goals = xml_read_int(text);
    else if(state == TAG_PLAYER_STATS_SHOTS)
	new_stats.shots = xml_read_int(text);
    else if(state == TAG_PLAYER_STATS_YELLOW)
	new_stats.yellow = xml_read_int(text);
    else if(state == TAG_PLAYER_STATS_RED)
	new_stats.red = xml_read_int(text);
    else if(state == TAG_PLAYER_CAREER)
	new_player->career[careeridx] = xml_read_int(text);
    else if(state == TAG_PLAYER_STREAK)
	new_player->streak = xml_read_int(text);
    else if(state == TAG_PLAYER_STREAK_COUNT)
	new_player->streak_count = xml_read_float(text);
    else if(state == TAG_PLAYER_STREAK_PROB)
	new_player->streak_prob = xml_read_float(text);

    #undef int_value
    #undef float_value
}

void
xml_loadsave_players_write(FILE *fil, const GArray *players)
{
#ifdef DEBUG
    printf("xml_loadsave_players_write\n");
#endif

    gint i;

    fprintf(fil, "%s<_%d>\n", I1, TAG_PLAYERS);
   
    for(i=0;i<players->len;i++)
	xml_loadsave_players_write_player(fil, &g_array_index(players, Player, i));

    fprintf(fil, "%s</_%d>\n", I1, TAG_PLAYERS);
}

void
xml_loadsave_players_write_player(FILE *fil, const Player *pl)
{
#ifdef DEBUG
    printf("xml_loadsave_players_write_player\n");
#endif

    gint i;

    fprintf(fil, "%s<_%d>\n", I1, TAG_PLAYER);

    xml_write_string(fil, pl->name, TAG_PLAYER_NAME, I2);
    xml_write_int(fil, pl->pos, TAG_PLAYER_POS, I2);
    xml_write_int(fil, pl->cpos, TAG_PLAYER_CPOS, I2);
    xml_write_int(fil, pl->health, TAG_PLAYER_HEALTH, I2);
    xml_write_int(fil, pl->recovery, TAG_PLAYER_RECOVERY, I2);
    xml_write_int(fil, pl->id, TAG_PLAYER_ID, I2);
    xml_write_int(fil, pl->value, TAG_PLAYER_VALUE, I2);
    xml_write_int(fil, pl->wage, TAG_PLAYER_WAGE, I2);
    xml_write_int(fil, pl->offers, TAG_PLAYER_OFFERS, I2);
    xml_write_int(fil, pl->participation, TAG_PLAYER_PARTICIPATION, I2);

    xml_write_float(fil, pl->skill, TAG_PLAYER_SKILL, I2);
    xml_write_float(fil, pl->cskill, TAG_PLAYER_CSKILL, I2);
    xml_write_float(fil, pl->talent, TAG_PLAYER_TALENT, I2);
    xml_write_float(fil, pl->fitness, TAG_PLAYER_FITNESS, I2);
    xml_write_float(fil, pl->lsu, TAG_PLAYER_LSU, I2);
    xml_write_float(fil, pl->age, TAG_PLAYER_AGE, I2);
    xml_write_float(fil, pl->peak_age, TAG_PLAYER_PEAK_AGE, I2);
    xml_write_float(fil, pl->peak_region, TAG_PLAYER_PEAK_REGION, I2);
    xml_write_float(fil, pl->contract, TAG_PLAYER_CONTRACT, I2);

    for(i=0;i<4;i++)
	xml_write_float(fil, pl->etal[i], TAG_PLAYER_ETAL, I3);
    
    for(i=0;i<PLAYER_VALUE_END;i++)
	xml_write_int(fil, pl->career[i], TAG_PLAYER_CAREER, I2);

    xml_write_int(fil, pl->streak, TAG_PLAYER_STREAK, I2);
    xml_write_float(fil, pl->streak_count, TAG_PLAYER_STREAK_COUNT, I2);
    xml_write_float(fil, pl->streak_prob, TAG_PLAYER_STREAK_PROB, I2);

    for(i=0;i<pl->stats->len;i++)
    {
	fprintf(fil, "%s<_%d>\n", I2, TAG_PLAYER_STATS);

	xml_write_int(fil, g_array_index(pl->stats, PlayerCompetitionStats, i).competition->id,
		      TAG_PLAYER_STATS_CLID, I3);
	xml_write_int(fil, g_array_index(pl->stats, PlayerCompetitionStats, i).games,
		      TAG_PLAYER_STATS_GAMES, I3);
	xml_write_int(fil, g_array_index(pl->stats, PlayerCompetitionStats, i).goals,
		      TAG_PLAYER_STATS_GOALS, I3);
	xml_write_int(fil, g_array_index(pl->stats, PlayerCompetitionStats, i).shots,
		      TAG_PLAYER_STATS_SHOTS, I3);
	xml_write_int(fil, g_array_index(pl->stats, PlayerCompetitionStats, i).yellow,
		      TAG_PLAYER_STATS_YELLOW, I3);
	xml_write_int(fil, g_array_index(pl->stats, PlayerCompetitionStats, i).red,
		      TAG_PLAYER_STATS_RED, I3);

	fprintf(fil, "%s</_%d>\n", I2, TAG_PLAYER_STATS);
    }

    fprintf(fil, "%s</_%d>\n", I1, TAG_PLAYER);
}
