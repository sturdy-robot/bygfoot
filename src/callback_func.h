#ifndef CALLBACK_FUNC_H
#define CALLBACK_FUNC_H

#include "bygfoot.h"

void
callback_show_next_live_game(void);

void
callback_player_clicked(gint idx, GdkEventButton *event);

void
callback_show_last_match(void);

void
callback_show_fixtures(gint type);

#endif
