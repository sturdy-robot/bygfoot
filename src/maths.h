#ifndef MATHS_H
#define MATHS_H

#include <math.h>

#include "bygfoot.h"

/**
   Macros for random number generation (#rnd for float, #rndi and #gauss_disti for integer).
*/
#define math_rnd(lower,upper) ((gfloat)random()/(gfloat)0x7fffffff*((upper)-(lower))+(lower))
#define math_rndi(lower,upper) ((gint)rint( math_rnd((gfloat)(lower) - 0.499, (gfloat)(upper) + 0.499) ))
#define math_gauss_disti(lower, upper) ((gint)rint( math_gauss_dist((gfloat)lower - 0.499, (gfloat)upper + 0.499)))

gfloat
math_gaussrand(void);

gfloat
math_gauss_dist(gfloat lower, gfloat upper);

gint
math_get_place(gint value, gint place);

gint
math_round_integer(gint number, gint places);

void
math_generate_permutation(gint *array, gint start, gint end);

gint
math_get_bye_len(gint number);

gint
math_sum_int_array(const gint *array, gint max);

#endif
