/*
   name.c

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

#include "main.h"
#include "free.h"
#include "name.h"
#include "option.h"
#include "variables.h"
#include "xml_name.h"

static void
load_name_list(const gchar *names_file)
{
    NameList new;
    /** Create new name list. */
    new.sid = NULL;
    new.first_names = new.last_names = NULL;

    xml_name_read(names_file, &new);

    if(new.sid == NULL)
    {
	debug_print_message("name_get_new: names file with sid '%s' not found, taking general names file.\n",
		  names_file);
	load_name_list(opt_str("string_opt_player_names_file"));
    }

    g_array_append_val(name_lists, new);
}

/** Get a random player name from the given
    names list. If the names list is not found, create
    it from file. If the file can't be found, either,
    make some fuss and take one from the general names. */
gchar*
name_get(const gchar *names_file)
{
#ifdef DEBUG
    printf("name_get\n");
#endif

#ifdef DEBUG
    printf("name_get\n");
#endif

    gint i;

    if(math_rnd(0, 1) < const_float("float_name_random_list_prob")) {
        Country *c;
        if (!country_list) {
            c = &country;
        } else {
            gint country_idx = math_rndi(0, country_list->len);
            if (country_idx == country_list->len)
                c = &country;
            else
                c = g_ptr_array_index(country_list, country_idx);
        }
        names_file = ((League*)g_ptr_array_index(c->leagues, 0))->names_file;
    }

    for(i=0;i<name_lists->len;i++)
	if(strcmp(names_file, nli(i).sid) == 0)
	    return name_get_from_list(&nli(i));

    load_name_list(names_file);

    return name_get_from_list(&nli(name_lists->len - 1));
}

/** Return a newly allocated string with a randomly
    picked combined name from the list. */
gchar*
name_get_from_list(const NameList *namelist)
{
#ifdef DEBUG
    printf("name_get_from_list\n");
#endif

#ifdef DEBUG
    printf("name_get_from_list\n");
#endif

    gchar buf[SMALL];

    sprintf(buf, "%s %s", name_get_random_first_name(namelist),
	    name_get_random_last_name(namelist));

    return g_strdup(buf);
}

/** Find the namelist with the given sid. */
NameList*
name_get_list_from_sid(const gchar *sid)
{
    gint i;
    NameList new;

    for(i=0;i<name_lists->len;i++)
	if(strcmp(sid, nli(i).sid) == 0)
	    return &nli(i);

    new.sid = NULL;
    new.first_names = new.last_names = NULL;

    xml_name_read(sid, &new);

    if(new.sid == NULL)
	main_exit_program(EXIT_POINTER_NOT_FOUND, 
			  "name_get_list_from_sid: namelist with sid %s not found", sid);
    else
    {
	g_array_append_val(name_lists, new);
	return &nli(name_lists->len - 1);
    }

    return NULL;
}

gchar *
name_get_from_random_list(void)
{
    gint i;

    gint country_idx;
    Country *country_names;
    const gchar *names_file;
    gchar *name;
    NameList new;

    if (!country_list) {
        country_names = &country;
    } else {
        country_idx = math_rndi(0, country_list->len);
        if (country_idx == country_list->len)
            country_names = &country;
        else
            country_names = g_ptr_array_index(country_list, country_idx);
    }
    names_file = ((League*)g_ptr_array_index(country_names->leagues, 0))->names_file;

    /** Create new name list. */
    new.sid = NULL;
    new.first_names = new.last_names = NULL;

    xml_name_read(names_file, &new);

    name = name_get_from_list(&new);

    free_name_list(&new, FALSE);

    return name;
}
