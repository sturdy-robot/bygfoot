/*
   competition.c

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2023 Tom Stellard (tom@stellard.net)

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

#include "bygfoot.h"
#include "competition.h"

gboolean
clid_is_league(gint clid)
{
    return clid < ID_CUP_START;
}

gboolean
clid_is_cup(gint clid)
{
    return clid >= ID_CUP_START;
}

gboolean
competition_is_cup(const Competition *c)
{
    return clid_is_cup(c->id);
}
