/*
   xml_loadsave_leagues_cups.h

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

#ifndef XML_LOADSAVE_LEAGUES_CUPS_H
#define XML_LOADSAVE_LEAGUES_CUPS_H

#include "bygfoot.h"
#include "variables.h"

void
xml_loadsave_leagues_cups_write(const gchar *prefix);

void
xml_loadsave_leagues_cups_start_element (GMarkupParseContext *context,
					 const gchar         *element_name,
					 const gchar        **attribute_names,
					 const gchar        **attribute_values,
					 gpointer             user_data,
					 GError             **error);

void
xml_loadsave_leagues_cups_end_element    (GMarkupParseContext *context,
					  const gchar         *element_name,
					  gpointer             user_data,
					  GError             **error);

void
xml_loadsave_leagues_cups_text         (GMarkupParseContext *context,
					const gchar         *text,
					gsize                text_len,  
					gpointer             user_data,
					GError             **error);

void
xml_loadsave_leagues_cups_read(Country *country, const gchar *dirname, const gchar *prefix);

void
xml_loadsave_leagues_cups_adjust_team_ptrs_cups(GPtrArray *cups);

void
xml_loadsave_leagues_cups_adjust_team_ptrs(void);

#endif
