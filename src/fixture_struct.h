#ifndef FIXTURE_STRUCT_H
#define FIXTURE_STRUCT_H

#include "bygfoot.h"
#include "team_struct.h"

/** Structure representing a fixture, or, in other words,
    a match. */
typedef struct
{
    /** The cup or league the fixture belongs to. */
    gint clid;
    /** The round (in a cup) the fixture belongs to. */
    gint round;    
    /** When it takes place. */
    gint week_number, week_round_number;
    /** The teams involved. */
    Team *teams[2];
    /** The number of goals for each team in
	regulation, extra time and penalty shoot-out. */
    gint result[2][3];
    /** Whether there's home advantage, this is second leg,
	or the game has to be decided. */
    gboolean home_advantage, second_leg, decisive;
    /** How many people attended and whether there were
	special events. */
    gint attendance;
} Fixture;

#endif