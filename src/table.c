/*
   table.c

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
#include "competition.h"
#include "league.h"
#include "misc.h"
#include "table.h"
#include "team.h"
#include "variables.h"

/** Return a newly allocated empty table. */
Table
table_new(void)
{
#ifdef DEBUG
    printf("table_new\n");
#endif

    Table new;

    new.name = NULL;
    new.competition = NULL;
    new.round = -1;
    new.elements = g_array_new(FALSE, FALSE, sizeof(TableElement));

    return new;
}

/** Return a nullified table element.
    @param team The team pointer of the element.
    @param old_rank The old_rank value of the element.
    @see #TableElement */
TableElement
table_element_new(Team *team, gint old_rank)
{
#ifdef DEBUG
    printf("table_element_new\n");
#endif

    gint i;
    TableElement new;

    new.team = team;
    new.old_rank = old_rank;

    for(i=0;i<TABLE_END;i++)
	new.values[i] = 0;

    return new;
}

/** Update the appropriate table entries after
    a fixture has been calculated. 
    @param fix The fixture that's just been calculated. */
void
table_update(const Fixture *fix)
{
#ifdef DEBUG
    printf("table_update\n");
#endif

    gint i, j, end;
    gint idx = (fix->result[0][0] < fix->result[1][0]);
    TableElement *elements[2];

    end = (fix->competition->id < ID_CUP_START) ? 2 : 1;

    for(j = 0; j < end; j++)
    {        
        elements[0] = elements[1] = NULL;
        table_update_get_elements(elements, fix, (j == 1));

        for(i=0;i<2;i++)
        {
            if(elements[i] != NULL)
            {
                elements[i]->values[TABLE_PLAYED]++;
                elements[i]->values[TABLE_GF] += fix->result[i][0];
                elements[i]->values[TABLE_GA] += fix->result[!i][0];
                elements[i]->values[TABLE_GD] = 
                    elements[i]->values[TABLE_GF] - elements[i]->values[TABLE_GA];                
            }
        }
    
        if(fix->result[0][0] == fix->result[1][0])
            for(i=0;i<2;i++)
            {
                if(elements[i] != NULL)
                {
                    elements[i]->values[TABLE_DRAW]++;
                    elements[i]->values[TABLE_PTS] += 1;
                }
            }
        else
        {
            if(elements[idx] != NULL)
            {
                elements[idx]->values[TABLE_WON]++;
                elements[idx]->values[TABLE_PTS] += 3;
            }

            if(elements[!idx] != NULL)
            {
                elements[!idx]->values[TABLE_LOST]++;
            }
        }
    }
}

/** Get the pointers to the table entries
    representing the two teams from the fixture. 
    @param elements The table entries.
    @fix The fixture.
    @non_cumulative Whether to return the last non-cumulative table. */
void
table_update_get_elements(TableElement **elements, const Fixture *fix, gboolean non_cumulative)
{
#ifdef DEBUG
    printf("table_update_get_elements\n");
#endif

    gint i, j;
    Table *table;

    if(fix->competition->id < ID_CUP_START)
    {
        for(j = 0; j < 2; j++)
        {
            if(non_cumulative &&
               fix->teams[j]->league->tables->len == 1)
            {
                elements[j] = NULL;
                continue;
            }

            table = (non_cumulative) ? 
                league_table(fix->teams[j]->league) :
                league_table_cumul(fix->teams[j]->league);
            
            for(i=0;i<table->elements->len;i++)
            {
                if(g_array_index(table->elements, TableElement, i).team == fix->teams[j])
                    elements[j] = &g_array_index(table->elements, TableElement, i);
            }
        }
    }
    else
	for(i=0;i<cup_get_last_tables((Cup*)fix->competition)->len;i++)
	{
	    table = &g_array_index(cup_get_last_tables((Cup*)fix->competition), Table, i);

	    if(elements[0] == NULL || elements[1] == NULL)
		for(j=0;j<table->elements->len;j++)
		{
		    if(g_array_index(table->elements, TableElement, j).team == fix->teams[0])
			elements[0] = &g_array_index(table->elements, TableElement, j);
		    else if(g_array_index(table->elements, TableElement, j).team == fix->teams[1])
			elements[1] = &g_array_index(table->elements, TableElement, j);
		}
	}
}

/** Compare function used to sort the tables.
    @param a The first table element.
    @param b The second element.
    @param clid_round The clid and the cup round of the table
    encoded as a gpointer. */
gint
table_element_compare_func(gconstpointer a,
			   gconstpointer b,
			   gpointer comp_ptr)
{
#ifdef DEBUG
    printf("table_element_compare_func\n");
#endif

    gint i;
    gint cup_round, value;
    TableElement *element1 = (TableElement*)a,
	*element2 = (TableElement*)b;
    GArray *fixtures;
    const Fixture *fix[2] = {NULL, NULL};
    Competition *comp = (Competition*)comp_ptr;

    if(element1->team == element2->team)
	return 0;

    fixtures = league_cup_get_fixtures(comp->id);
    if(competition_is_league(comp))
	cup_round = -1;
    else
	cup_round = cup_has_tables((Cup*)comp);
    
    /*todo use misc_int_compare*/
    if(element1->values[TABLE_PTS] != element2->values[TABLE_PTS])
	value = misc_int_compare(element1->values[TABLE_PTS],
				 element2->values[TABLE_PTS]);
    else if(element1->values[TABLE_GF] - element1->values[TABLE_GA] !=
	    element2->values[TABLE_GF] - element2->values[TABLE_GA])
	value = misc_int_compare(element1->values[TABLE_GF] - element1->values[TABLE_GA],
				 element2->values[TABLE_GF] - element2->values[TABLE_GA]);
    else if(element1->values[TABLE_GA] != element2->values[TABLE_GA])
	value = misc_int_compare(element1->values[TABLE_GA],
				 element2->values[TABLE_GA]);
    else
    {
	for(i=0;i<fixtures->len;i++)
	{
	    if(g_array_index(fixtures, Fixture, i).round == cup_round &&
	       g_array_index(fixtures, Fixture, i).week_number <= week &&
	       g_array_index(fixtures, Fixture, i).week_round_number <= week_round)
	    {
		if(g_array_index(fixtures, Fixture, i).teams[0] == element1->team &&
		   g_array_index(fixtures, Fixture, i).teams[1] == element2->team)
		    fix[0] = &g_array_index(fixtures, Fixture, i);
		else if(g_array_index(fixtures, Fixture, i).teams[1] == element1->team &&
			g_array_index(fixtures, Fixture, i).teams[0] == element2->team)
		    fix[1] = &g_array_index(fixtures, Fixture, i);
	    }
	}
	
	if(fix[0] == NULL || fix[1] == NULL)
	    value = 0;
	else
	{
	    if(fix[0]->result[0][0] + fix[1]->result[1][0] >
	       fix[0]->result[1][0] + fix[1]->result[0][0])
		value = -1;
	    else if(fix[0]->result[0][0] + fix[1]->result[1][0] <
		    fix[0]->result[1][0] + fix[1]->result[0][0])
		value = 1;
	    else if(fix[1]->result[1][0] > fix[0]->result[1][0])
		value = -1;
	    else
		value = 1;
	}
    }
    
    return value;
}

/** Find out if there are active leagues (which automatically
    have tables then) or cups with tables in the country. */
gboolean
query_tables_in_country(void)
{
#ifdef DEBUG
    printf("query_tables_in_country\n");
#endif

    gint i;

    for(i=0;i<country.leagues->len;i++) {
        League *league = g_ptr_array_index(country.leagues, i);
	if(query_league_active(league))
	    return TRUE;
    }

    for(i=0;i<country.cups->len;i++) {
        Cup *cup = g_ptr_array_index(country.cups, i);
	if(cup_has_tables(cup) != -1)
	    return TRUE;
    }

    return FALSE;
}

/** Copy a table. */
Table
table_copy(const Table *table)
{
#ifdef DEBUG
    printf("table_copy\n");
#endif

    gint i, j;
    Table new_table;
    TableElement new_table_element;
    TableElement *elem;

    new_table.name = g_strdup(table->name);
    new_table.competition = table->competition;
    new_table.round = table->round;
    new_table.elements = g_array_new(FALSE, FALSE, sizeof(TableElement));

    for(i = 0; i < table->elements->len; i++)
    {
        elem = &g_array_index(table->elements, TableElement, i);
        new_table_element.team = elem->team;
        new_table_element.old_rank = elem->old_rank;

        for(j=0;j<TABLE_END;j++)
            new_table_element.values[j] = elem->values[j];
        
        g_array_append_val(new_table.elements, new_table_element);
    }

    return new_table;
}

/** Refresh the team pointers in the table from the team ids. */
void
table_refresh_team_pointers(Table *table)
{
    gint i;

    for(i = 0; i < table->elements->len; i++)
        g_array_index(table->elements, TableElement, i).team =
            team_of_id(GPOINTER_TO_INT(g_array_index(table->elements, TableElement, i).team));
}
