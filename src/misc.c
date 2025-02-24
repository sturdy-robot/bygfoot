/*
   misc.c

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

#include <time.h>

#include "main.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "strategy_struct.h"
#include "variables.h"

/**
   Print the contents of a GError (if it was set).
   If abort_program is TRUE, we free the memory
   and exit the game.
   @param error The GError we check.
   @param abort_program Whether or not we continue or exit the program.
*/
void
misc_print_error(GError **error, gboolean abort_program)
{
#ifdef DEBUG
    printf("misc_print_error\n");
#endif

    if(*error == NULL)
	return;
    
    debug_print_message("error message: %s\n", (*error)->message);
    g_error_free(*error);
    *error = NULL;

    if(abort_program)
	main_exit_program(EXIT_PRINT_ERROR, NULL);
}

/** Swap two integers.
    @param first The first integer.
    @param second The second integer. */
void
misc_swap_int(gint *first, gint *second)
{
#ifdef DEBUG
    printf("misc_swap_int\n");
#endif

    gint swap = *first;

    *first = *second;
    *second = swap;
}

/** Swap two pointers.
    @param first The first pointer.
    @param second The second pointer. */
void
misc_swap_gpointer(gpointer *first, gpointer *second)
{
#ifdef DEBUG
    printf("\n");
#endif

#ifdef DEBUG
    printf("misc_swap_gpointer\n");
#endif

    gpointer swap = *first;

    *first = *second;
    *second = swap;
}

/** Transform a string containing white spaces into an array of strings without
    white spaces. 
    @param string The string containing white spaces.
    @return A GPtrArray containing all the strings without white spaces that were part of the original string.
    This array must be freed with free_gchar_array(). */
GPtrArray*
misc_separate_strings(gchar *string)
{
    gint i, cnt = 0, start = 0;
    gchar buf[BIG];
    GPtrArray *string_array = g_ptr_array_new();

    for(i=0;i<strlen(string);i++)
	if(g_ascii_isspace(string[i]))
	    start++;
	else
	    break;

    if(start == strlen(string))
    {
	debug_print_message("misc_separate_strings: input string contains only white spaces\n");
	return string_array;
    }

    for(i=start;i<strlen(string) + 1;i++)
    {
	if(i < strlen(string) && !g_ascii_isspace(string[i]))
	    buf[cnt++] = string[i];
	else
	{
	    buf[cnt] = '\0';
	    cnt = 0;
	    if(strlen(buf) > 0)
		g_ptr_array_add(string_array, (gpointer)g_strdup(buf));
	}
    }

    return string_array;
}

/** Write a pointer array randomly into another one and free the original array.
    @param array The array to randomise.
    @return A new pointer array containing the items in random order. */
GPtrArray*
misc_randomise_g_pointer_array(GPtrArray *array)
{
#ifdef DEBUG
    printf("misc_randomise_g_pointer_array\n");
#endif

    GPtrArray *new = g_ptr_array_new();
    gint order[array->len];
    gint i;

    math_generate_permutation(order, 0, array->len - 1);

    for(i=0;i<array->len;i++)
	g_ptr_array_add(new, g_ptr_array_index(array, order[i]));

    g_ptr_array_free(array, TRUE);

    return new;
}

/** Return a freshly allocated copy of the array. */
GPtrArray*
misc_copy_ptr_array(const GPtrArray *array)
{
#ifdef DEBUG
    printf("misc_copy_ptr_array\n");
#endif

    gint i;
    GPtrArray *copy = NULL;

    if(array != NULL)
	copy = g_ptr_array_new();
    else
	return NULL;

    for(i=0;i<array->len;i++)
	g_ptr_array_add(copy, g_ptr_array_index(array, i));

    return copy;
}

/** Add all pointers of \p src to the end of \p dest */
void
misc_extend_ptr_array(GPtrArray *dest, GPtrArray *src)
{
#ifdef GLIB_VERSION_2_62
    g_ptr_array_extend(dest, src, NULL, NULL);
#else
    gint i;
    for (i = 0; i < src->len; i++) {
        g_ptr_array_add(dest, g_ptr_array_index(src, i));
    }
#endif
}

/**
 * Insert an item into the \p array at \p index_.
 */
void
misc_g_ptr_array_insert(GPtrArray *array, gint _index, gpointer data)
{
#ifdef GLIB_VERSION_2_40
    g_ptr_array_insert(array, _index, data);
#else
    gint i;
    g_ptr_array_set_size(array, array->len + 1);
    for (i = _index; i < array->len; i++) {
        gpointer *insert_point = &g_ptr_array_index(array, i);
        gpointer old_data = *insert_point;
        *insert_point = data;
        data = old_data;
    }
#endif
}

/** Search for \p needle in \p haystack.  If \p needle is found \p index is
 * set to the index of \p neelde in \p haystack and TRUE is returned.
 * Otherwise, \p index_ is undefined and this function returns FALSE.*/
gboolean
misc_g_ptr_array_find(GPtrArray *haystack, gconstpointer needle, guint *index_)
{
#ifdef GLIB_VERSION_2_54
    return g_ptr_array_find(haystack, needle, index_);
#else
    gint i;
    for (i = 0; i < haystack->len; i++) {
        if (g_ptr_array_index(haystack, i) == needle) {
            *index_ = i;
            return TRUE;
        }
    }
    return FALSE;
#endif
}

/** Print a thousands-grouped output of 'number' into 'buf',
    like 2 234 345 instead of 2234345.
    @param number The number to print. 
    @buf The buffer to hold the number. */
void
misc_print_grouped_int(gint number, gchar *buf)
{
#ifdef DEBUG
    printf("misc_print_grouped_int\n");
#endif

    gint i;
    gchar buf2[GROUPED_INT_SIZE];
    gint length = 0;
    gfloat copy = (gfloat)(abs(number));
    gint number2 = abs(number);

    strcpy(buf, "");

    while(copy >= 1)
    {
	copy /= 10;
	length++;
    }

    if(length > 9)
    {
	sprintf(buf2, "%d", number);
	strcat(buf, buf2);
	return;
    }

    for(i = length; i > 0; i--)
    {
	sprintf(buf2, "%d", math_get_place(number2, i));
	strcat(buf, buf2);
	if(i % 3 == 1)
	    strcat(buf, " ");
    }

    if(number < 0)
    {
	sprintf(buf2, "- ");
	strcat(buf2, buf);
	sprintf(buf, "%s", buf2);
    }
    else if(number == 0)
	strcat(buf, "0");
}

/** Check whether 'item' is in array 'array' between
    'min' and 'max'.
    @param array The integer list.
    @param max The upper delimitor (exclusive).
    @param item The item we look for. */
gboolean
query_integer_is_in_array(gint item, gint *array, gint max)
{
#ifdef DEBUG
    printf("query_integer_is_in_array\n");
#endif

    gint i;

    for(i=0;i<max;i++)
	if(item == array[i])
	    return TRUE;
    
    return FALSE;
}

/** Check whether the number is in the array. */
gboolean
query_misc_integer_is_in_g_array(gint item, GArray *array)
{
#ifdef DEBUG
    printf("query_misc_integer_is_in_g_array\n");
#endif

    gint i;

    for(i=0;i<array->len;i++)
	if(item == g_array_index(array, gint, i))
	    return TRUE;

    return FALSE;
}

/** Compare two integers. */
gint
misc_int_compare(gint first, gint second)
{
#ifdef DEBUG
    printf("misc_int_compare\n");
#endif

    if(first > second)
	return -1;
    else if(first < second)
	return 1;

    return 0;
}

/** Compare two floats. */
gint
misc_float_compare(gfloat first, gfloat second)
{
#ifdef DEBUG
    printf("misc_float_compare\n");
#endif

    if(first > second)
	return -1;
    else if(first < second)
	return 1;

    return 0;
}

const gchar*
misc_strip_definitions_root(gchar *directory)
{
    gchar **vector;
    GList *list = root_definitions_directories;
    gchar *buf;

    while (list!=NULL)
    {
	if (g_str_has_prefix(directory, (gchar*)list->data))
        {
            vector = g_strsplit(directory, (gchar*)list->data,-1);
            buf =  g_strdup((gchar*)vector[g_strv_length(vector)-1]);
            g_strfreev(vector);
            return buf;
        }
        list = list->next; 
    }
    return "";
}

/** Check whether the string starts with a string in the string array. */
gboolean
query_misc_string_starts_with(const gchar *string, GList *list)
{
    while (list!=NULL)
    {
	return g_str_has_prefix(string, (gchar*)list->data);
        list = list->next; 
    }
    return FALSE;
}

/** Check whether the string is in the string array. */
gboolean
query_misc_string_in_array(const gchar *string, const GPtrArray *array)
{
    gint i;

    for(i=0;i<array->len;i++)
	if(strcmp(string, (gchar*)g_ptr_array_index(array, i)) == 0)
	    return TRUE;

    return FALSE;
}

/** Get a float representation of someone's age
    based on his birth year and month. */
gfloat
misc_get_age_from_birth(gint birth_year, gint birth_month)
{
#ifdef DEBUG
    printf("misc_get_age_from_birth\n");
#endif

    GDate *current_date = g_date_new();
    GDate *birth_date = g_date_new();

    g_date_set_time(current_date, time(NULL));
    g_date_set_dmy(birth_date, 15, birth_month, birth_year);

    return (gfloat)g_date_days_between(birth_date, current_date) / 365.25;
}

/* skip spaces */
const gchar*
misc_skip_spaces(const gchar* s)
{
   while (g_ascii_isspace(*s)) s++;
   return s;
}

/* read number, skip all leading and trailing spaces */
const gchar*
misc_parse_value(const gchar *s, gint *value)
{
   s = misc_skip_spaces(s);
   *value = 0;
   while (g_ascii_isdigit(*s)) {
      *value = *value * 10 + (*s - '0');
      s++;
   }
   return s;
}

/* parse numeric expression (supports + and -) */
const gchar*
misc_parse_expression(const gchar *s, gint *result)
{
    gint value = 0,
	loop = 1;

    s = misc_parse_value(s, &value);
    *result = value;

    while (loop) {
	s = misc_skip_spaces(s);
	switch(*s) {
	    case '+': 
		s = misc_parse_value(s+1, &value);
		*result += value;
		break;
	    case '-': 
		s = misc_parse_value(s+1, &value);
		*result -= value;
		break;
	    default:
		loop = 0;
	}
    }
    return s;
}

/* parse comparison (supports '<', '>' and '=') */
const gchar*
misc_parse_comparison(const gchar *s, gint *result)
{
   gint value = 0;
   s = misc_parse_expression(s, result);
   s = misc_skip_spaces(s);
   switch(*s) {
      case '<':
            if (*(s+1) == '=')
	    {
	       s = misc_parse_expression(s+2, &value);
	       *result = *result <= value;
	    }
	    else 
	    {
               s = misc_parse_expression(s+1, &value);
	       *result = *result < value;
	    }
	    break;
      case 'L':
            if (*(s+1) == 'E')
	    {
	       s = misc_parse_expression(s+2, &value);
	       *result = *result <= value;
	    }
	    else 
	    {
               s = misc_parse_expression(s+1, &value);
	       *result = *result < value;
	    }
	    break;
       case '=':
	       s = misc_parse_expression(s+1, &value);
	       *result = *result == value;
	   break;
       case '>': 
	   if (*(s+1) == '=')
	   {
	       s = misc_parse_expression(s+2, &value);
	       *result = *result >= value;
	    }
	    else 
	    {
               s = misc_parse_expression(s+1, &value);
	       *result = *result > value;
	    }
	    break;
       case 'G': 
	   if (*(s+1) == 'E')
	   {
	       s = misc_parse_expression(s+2, &value);
	       *result = *result >= value;
	    }
	    else 
	    {
               s = misc_parse_expression(s+1, &value);
	       *result = *result > value;
	    }
	    break;
       case '!':
	   if(*(s + 1) == '=')
	   {
               s = misc_parse_expression(s+2, &value);
	       *result = *result != value;
	   }
	   break;
   }
   return s;
}

const gchar*
misc_parse_and(const gchar *s, gint *result)
{
   gint value = 0;
   s = misc_parse_comparison(s, result);
   s = misc_skip_spaces(s);
   while (*s == 'a' && *(s+1) == 'n' && *(s+2) == 'd') {
      s = misc_parse_comparison(s + 3, &value);
      *result = *result && value;
      s = misc_skip_spaces(s);
   }

   return s;
}

const gchar*
misc_parse(const gchar *s, gint *result)
{
   gint value = 0;
   s = misc_parse_and(s, result);
   s = misc_skip_spaces(s);
   while (*s == 'o' && *(s+1) == 'r') {
       s = misc_parse_and(s + 2, &value);
       *result = *result || value;
       s = misc_skip_spaces(s);
   }
   return s;
}

/** Free the string if it's non-NULL and assign the contents to it. */
void
misc_string_assign(gchar **string, const gchar *contents)
{
#ifdef DEBUG
    printf("misc_string_assign\n");
#endif

    if(contents == NULL)
	return;

    if(*string != NULL)
	g_free(*string);

    *string = g_strdup(contents);
}

/** Choose one of strings separated with '|' */
void
misc_string_choose_random(gchar *string)
{
   gint count = 1;
   gchar **array;
   
   array = g_strsplit(string, "|", -1);
   count = g_strv_length(array);
   count = math_rndi(0, count - 1);
   strcpy(string, array[count]);
}

/** Replace a token in a string by another string. 
    The replacement should NOT CONTAIN THE TOKEN otherwise
    we end up in an infinite loop. */
void
misc_string_replace_token(gchar *string, const gchar *token, const gchar *replacement)
{
    gchar buf[SMALL], buf2[SMALL];
    gchar *occurrence = NULL;

    occurrence = g_strrstr(string, token);

    if(occurrence == NULL || strlen(string) < strlen(token))
	return;
    
    while(occurrence != NULL)
    {
	strncpy(buf, string, strlen(string) - strlen(occurrence));
	buf[strlen(string) - strlen(occurrence)] = '\0';
	strcpy(buf2, occurrence + strlen(token));
	sprintf(string, "%s%s%s", buf, replacement, buf2);
	occurrence = g_strrstr(string, token);
    }
}

/** Replace simple arithmetic expressions like "1 + 2"
    and comparisons like "3 < 4" with the appropriate result. */
void
misc_string_replace_expressions(gchar *string)
{
    gint i, j, last_idx = 0;
    gint value = -1;
    gchar buf[SMALL], buf2[SMALL];
    gchar *occurrence = NULL,
	*occurrence2 = NULL;

    if(debug > 200)
	g_print("misc_string_replace_expressions: #%s#\n",
		string);

    while(g_strrstr(string, "[") != NULL)
    {
	strcpy(buf, string);
	strcpy(string, "");

	occurrence = g_strrstr(buf, "[");
	i = strlen(buf) - strlen(occurrence);

	strncpy(buf2, buf, i);
	buf2[i] = '\0';
	strcat(string, buf2);	

	occurrence2 = g_strstr_len(occurrence, strlen(occurrence), "]");

	if(occurrence2 == NULL)
	{
	    debug_print_message("misc_string_replace_expressions: no matching ] found.");
	    return;
	}

	j = strlen(buf) - strlen(occurrence2);
	
	strncpy(buf2, buf + i + 1, j - i - 1);
	buf2[j - i - 1] = '\0';
	if (g_strrstr(buf2, "|"))
	    misc_string_choose_random(buf2);
	else 
	{
	    if(g_strrstr(buf2, "<") ||
	       g_strrstr(buf2, ">") ||
	       g_strrstr(buf2, "=") ||
	       g_strrstr(buf2, " G ") ||
	       g_strrstr(buf2, " L ") ||
	       g_strrstr(buf2, " GE ") ||
	       g_strrstr(buf2, " LE "))
		misc_parse(buf2, &value);
	    else
		misc_parse_expression(buf2, &value);
	    sprintf(buf2, "%d", value);
	}
	strcat(string, buf2);
	value = -1;

	last_idx = j + 1;

	if(last_idx < strlen(buf))
	{
	    strncpy(buf2, buf + last_idx, strlen(buf) - last_idx);
	    buf2[strlen(buf) - last_idx] = '\0';
	    strcat(string, buf2);
	}
    }
}

/** Try to replace all special tokens in the string and write the result to dest.
    @param string The string containing tokens.
    @param token_rep The arrays with the tokens and replacements.
    @return TRUE if we could replace all tokens and the commentary condition
    was fulfilled, FALSE otherwise. */
void
misc_string_replace_tokens(gchar *string, GPtrArray **token_rep)
{
    gint i;

    for(i=0;i<token_rep[0]->len;i++)
	if(g_strrstr(string, 
		     (gchar*)g_ptr_array_index(token_rep[0], i)))
	    misc_string_replace_token(string, 
				      (gchar*)g_ptr_array_index(token_rep[0], i),
				      (gchar*)g_ptr_array_index(token_rep[1], i));
}

/** Replace the portion 'paren' plus parentheses in the string. */
void
misc_string_replace_parenthesised(gchar *string, const gchar *paren, 
				  const gchar *replacement)
{
    gchar buf[SMALL];

    sprintf(buf, "(%s)", paren);

    misc_string_replace_token(string, buf, replacement);
}

/** Find the innermost parenthesised portion of the string
    and write it into the dest string. */
void
misc_string_get_parenthesised(const gchar *string, gchar *dest)
{
    const gchar *openpar = g_strrstr(string, "(");
    const gchar *closepar = g_strstr_len(string, strlen(string), ")");
    gint len = strlen(openpar) - strlen(closepar) - 1;

    strncpy(dest, openpar + 1, len);
    dest[len] = '\0';
}

/** Find out whether the conditions in the string are fulfilled. */
gboolean
misc_parse_condition(const gchar *condition, GPtrArray **token_rep)
{
    gboolean return_value = FALSE;
    gchar buf[SMALL], buf2[SMALL];

    strcpy(buf, condition);

    while(g_strrstr(buf, "("))
    {
	misc_string_get_parenthesised(buf, buf2);

	if(misc_parse_condition(buf2, token_rep))
	    misc_string_replace_parenthesised(buf, buf2, "1 = 1");
	else
	    misc_string_replace_parenthesised(buf, buf2, "1 = 2");
    }

    do
    {
	strcpy(buf2, buf);
	misc_string_replace_tokens(buf, token_rep);
	misc_string_replace_expressions(buf);
    }
    while(strcmp(buf2, buf) != 0);

    if(g_strrstr(buf, "_") != NULL)
	return FALSE;

    misc_parse(buf, &return_value);

    return return_value;
}

/** Add a replacement rule to the token array.
    The string should be in allocated as it will
    get freed later. */
void
misc_token_add(GPtrArray **token_rep, gint token_idx, 
	       gchar *replacement)
{
    g_ptr_array_add(token_rep[0], 
		    (gpointer)g_strdup(g_array_index(replacement_tokens.list, Option, token_idx).string_value));
    g_ptr_array_add(token_rep[1], (gpointer)replacement);
}

/** Add a 0 or 1 as a token to the token array. */
void
misc_token_add_bool(GPtrArray **token_rep, gint token_idx, 
                    gboolean value)
{
    g_ptr_array_add(token_rep[0], 
		    (gpointer)g_strdup(g_array_index(replacement_tokens.list, Option, token_idx).string_value));
    g_ptr_array_add(token_rep[1], misc_int_to_char(value));
}

/** Remove the replacement rule given by the index. */
void
misc_token_remove(GPtrArray **token_rep, gint idx)
{ 
    gint i;

    for(i=token_rep[0]->len - 1; i >= 0; i--)
	if(strcmp((gchar*)g_ptr_array_index(token_rep[0], i),
		  g_array_index(replacement_tokens.list, Option, idx).string_value) == 0)
	{
	    g_free(g_ptr_array_index(token_rep[0], i));
	    g_free(g_ptr_array_index(token_rep[1], i));
	    
	    g_ptr_array_remove_index_fast(token_rep[0], i);
	    g_ptr_array_remove_index_fast(token_rep[1], i);
	}    
}

/** Try to replace all tokens in the given text and write the result
    into the dest variable. */
gboolean
misc_string_replace_all_tokens(GPtrArray **token_rep,
                               const gchar *text_tokens, gchar *dest)
{
    gchar buf[SMALL];

    strcpy(dest, text_tokens);
    
    do
    {
	strcpy(buf, dest);
	misc_string_replace_tokens(dest, token_rep);
	misc_string_replace_expressions(dest);
    }
    while(strcmp(buf, dest) != 0);

    return (g_strrstr(dest, "_") == NULL);
}

/* Alphabetic compare function. */
gint
misc_alphabetic_compare(gconstpointer a, gconstpointer b)
{
    const gchar *string[2] = {(const gchar*)a,
                              (const gchar*)b};
    gchar alphabet[26] = {'a','b','c','d','e','f','g',
                          'h','i','j','k','l','m','n',
                          'o','p','q','r','s','t','u',
                          'v','w','x','y','z'};
    gint len[2] = {strlen(string[0]), strlen(string[1])};
    gint maxlen = MIN(len[0], len[1]);
    gint letter[2];
    gint i, j, k;

    for(i = 0; i < maxlen; i++)
    {
        for(k = 0; k < 2; k++)
        {
            letter[k] = 0;
            for(j = 0; j < 26; j++)
                if(string[k][i] == alphabet[j])
                {
                    letter[k] = j;
                    break;
                }
        }
        
        if(letter[0] < letter[1])
            return -1;
        else if(letter[0] > letter[1])
            return 1;
    }

    if(len[0] != len[1])
        return 1 - 2 * (len[0] < len[1]);

    return 0;
}

static gboolean
misc_condition_evaluate_var(const StratCondPart *var, GArray *operands, GArray *token_rep)
{
    const StratTokenValue *token_value;
    gint token = GPOINTER_TO_INT(var->value);
    /* I think it would be good to warn here that the variable was not found,
     * however the the previous implementation considered the whole condition
     * to be FALSE when a variable was not found, so for now we need to
     * replicate the behavior.
     */
    if (token == REP_TOKEN_LAST) {
        return FALSE;
    }
    token_value = &g_array_index(token_rep, StratTokenValue, token);
    if (!token_value->initialized)
        return FALSE;
    g_array_append_val(operands, token_value->value);
    return TRUE;
}

static gboolean
misc_parse_condition_is_binary_op(const StratCondPart *part)
{
    switch (part->token) {
    default:
        return FALSE;
    case STRAT_COND_EQ:
    case STRAT_COND_NE:
    case STRAT_COND_GT:
    case STRAT_COND_GE:
    case STRAT_COND_LT:
    case STRAT_COND_LE:
    case STRAT_COND_AND:
    case STRAT_COND_OR:
        return TRUE;
    }
}

static gboolean
misc_parse_condition_is_compare_op(const StratCondPart *part)
{
    switch (part->token) {
    default:
        return FALSE;
    case STRAT_COND_EQ:
    case STRAT_COND_NE:
    case STRAT_COND_GT:
    case STRAT_COND_GE:
    case STRAT_COND_LT:
    case STRAT_COND_LE:
        return TRUE;
    }
}

static gboolean
misc_condition_evaluate(const StratCondPart *operation, GArray *operands)
{
    gint op0, op1, result;
    if (operands->len < 2)
        return FALSE;

    op0 = g_array_index(operands, gint, operands->len - 2);
    op1 = g_array_index(operands, gint, operands->len - 1);

    switch (operation->token) {
    default:
        g_critical("Unknown token: %d\n", operation->token);
        return FALSE;
    case STRAT_COND_EQ:
        result = op0 == op1;
        break;
    case STRAT_COND_NE:
        result = op0 != op1;
        break;
    case STRAT_COND_GT:
        result = op0 > op1;
        break;
    case STRAT_COND_GE:
        result = op0 >= op1;
        break;
    case STRAT_COND_LT:
        result = op0 < op1;
        break;
    case STRAT_COND_LE:
        result = op0 <= op1;
        break;
    case STRAT_COND_AND:
        result = op0 && op1;
        break;
    case STRAT_COND_OR:
        result = op0 || op1;
        break;
    }

    g_array_remove_range(operands, operands->len - 2, 2);
    g_array_append_val(operands, result);
    return TRUE;
}

static const StratCondPart *
misc_parse_value_expression(GArray *stack, const StratCondPart *input)
{
    if (input->token == STRAT_COND_INT || input->token == STRAT_COND_VAR) {
        g_array_append_val(stack, *input);
        return input + 1;
    }
    return input;
}

static const StratCondPart *
misc_parse_compare_expression(GArray *stack, const StratCondPart *input)
{
    input = misc_parse_value_expression(stack, input);
    if (misc_parse_condition_is_compare_op(input)) {
        const StratCondPart *compare_op = input;
        input = misc_parse_value_expression(stack, input + 1);
        g_array_append_val(stack, *compare_op);
    }
    return input;
}

static const StratCondPart *
misc_parse_and_expression(GArray *stack, const StratCondPart *input)
{
    input = misc_parse_compare_expression(stack, input);
    if (input->token == STRAT_COND_AND) {
        const StratCondPart *and = input;
        input = misc_parse_and_expression(stack, input + 1);
        g_array_append_val(stack, *and);
    }
    return input;
}

static const StratCondPart *
misc_parse_or_expression(GArray *stack, const StratCondPart *input)
{
    if (input->token == STRAT_COND_OPEN_PAREN) {
       input = misc_parse_or_expression(stack, input + 1);
       if (input->token != STRAT_COND_CLOSE_PAREN);
           return NULL;
       return input + 1;
    }

    input = misc_parse_and_expression(stack, input);
    if (input->token == STRAT_COND_OR) {
        const StratCondPart *or = input;
        input = misc_parse_or_expression(stack, input + 1);
        g_array_append_val(stack, *or);
    }
    return input;
}

static const StratCondPart *
misc_parse_paren_expression(GArray *stack, const StratCondPart *input)
{
    if (input->token == STRAT_COND_OPEN_PAREN) {
       input = misc_parse_or_expression(stack, input + 1);
       if (input->token != STRAT_COND_CLOSE_PAREN) {
            return NULL;
       }
       return input + 1;
    }
    return misc_parse_or_expression(stack, input);
}

/**
 * This functions parses a condition into a stack of operations in postfix
 * notation, where the operands preceeded the operation.
 * Lexer Tokens:
 *
 * TOKEN = COND | INT | VARIABLE | LOGIC_OP | OPEN_PAREN | CLOSE_PAREN
 * COND = '=' | '!=' | GT | GE | LT | LE
 * GT = 'G' | '>'
 * GE = 'GE' | '>='
 * LT = 'L' | '<'
 * LE = 'LE | '<='
 * INT = [-]*[0-9]+
 * VARIABLE = _[A-Z]+_
 * LOGIC_OP = 'and' | 'or'
 * OPEN_PAREN = '('
 * CLOSE_PAREN = ')'
 *
 * BNF: 
 *
 * <Expression> ::= <Paren Expression>
 *
 * <Paren Expression> ::= ( <Or Expression> )
 *                      | <Or Expression>
 *
 * <Or Expression> ::= <And Expression> or <Or Expression>
 *                   | <And Expression>
 *                   | ( And Expression )
 * 
 * <And Expression> ::= <Compare Expression> and <And Exp>
 *                    | <Compare Expression>
 *
 * <Compare Expression> ::= <Value Expression> compare_op <Value Expresion>
 *                        | <Value Expression>
 * <Value Expression> ::= int
 *                      | variable
 */
GArray *
misc_parse_condition_fast(const gchar *condition)
{
    GError *error = NULL;

    GRegex *int_regex = g_regex_new("-*[0-9]+", 0, 0, &error);
    GRegex *var_regex = g_regex_new("_[A-Z]+_", 0, 0, &error);
    const gchar *iter = condition;
    GArray *tokens = g_array_new(TRUE, FALSE, sizeof(StratCondPart));
    GMatchInfo *match_info = NULL;
    GArray *stack;

    /* Lexer */

    while (*iter) {
        StratCondPart part;
        part.value = iter;
        switch (*iter) {
        case ' ':
            iter++;
            continue;
        case '(':
            part.token = STRAT_COND_OPEN_PAREN;
            part.len = 1;
            break;
        case ')':
            part.token = STRAT_COND_CLOSE_PAREN;
            part.len = 1;
            break;
        case '=':
            part.token = STRAT_COND_EQ;
            part.len = 1;
            break;
        case '!':
            if (iter[1] != '=')
                goto lexer_done;
            part.token = STRAT_COND_NE;
            part.len = 2;
            break;
        case 'G':
            if (iter[1] == 'E') {
                part.token = STRAT_COND_GE;
                part.len = 2;
                break;
            }
            part.token = STRAT_COND_GT;
            part.len = 1;
            break;
        case '>':
            if (iter[1] == '=') {
                part.token = STRAT_COND_GE;
                part.len = 2;
                break;
            }
            part.token = STRAT_COND_GT;
            part.len = 1;
            break;
        case 'L':
            if (iter[1] == 'E') {
                part.token = STRAT_COND_LE;
                part.len = 2;
                break;
            }
            part.token = STRAT_COND_LT;
            part.len = 1;
            break;
        case '<':
            if (iter[1] == '=') {
                part.token = STRAT_COND_LE;
                part.len = 2;
                break;
            }
            part.token = STRAT_COND_LT;
            part.len = 1;
            break;
        case 'a':
            if (!g_strrstr_len(iter, 3, "and"))
                goto lexer_done;
            part.token = STRAT_COND_AND;
            part.len = 3;
            break;
        case 'o':
            if (iter[1] != 'r')
                goto lexer_done;
            part.token = STRAT_COND_OR;
            part.len = 2;
            break;
        case '_': {
            gint start_pos = -1, end_pos = -1;
            if (!g_regex_match(var_regex, iter, 0, &match_info))
                goto lexer_done;
            if (!g_match_info_fetch_pos(match_info, 0, &start_pos, &end_pos))
                goto lexer_done;
            if (start_pos != 0)
                goto lexer_done;
            part.token = STRAT_COND_VAR;
            part.len = end_pos;
            part.value = GINT_TO_POINTER(misc_parse_replacement_token(part.value, part.len));
            break;
        }
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            gint start_pos = -1, end_pos = -1;
            if (!g_regex_match(int_regex, iter, 0, &match_info))
                goto lexer_done;
            if (!g_match_info_fetch_pos(match_info, 0, &start_pos, &end_pos))
                goto lexer_done;
            if (start_pos != 0)
                goto lexer_done;
            part.token = STRAT_COND_INT;
            part.len = end_pos;
            part.value = GINT_TO_POINTER(g_ascii_strtoll(part.value, NULL, 0));
            break;
        }
        default:
            goto lexer_done;
        }

        iter += part.len;
        g_array_append_val(tokens, part);
    }

lexer_done:
    g_regex_unref(int_regex);
    g_regex_unref(var_regex);

    if (match_info)
        g_match_info_free(match_info);

    if (*iter) {
        /* Handle error */
        g_array_unref(tokens);
        g_critical("Failed to lex strategy condition %s at char %d (%c)\n",
                   condition, (gint)(iter - condition), *iter);
        return NULL;
    }

    /* Parser */
    stack = g_array_new(FALSE, FALSE, sizeof(StratCondPart));
    if (!misc_parse_paren_expression(stack, (StratCondPart*)tokens->data)) {
    	g_critical("Failed to parse strategy condiiton %s\n", condition);
        g_array_unref(stack);
        stack = NULL;
    }

    g_array_unref(tokens);
    return stack;
}



gboolean
misc_evaluate_condition(const GArray *condition, GArray *token_rep)
{
    GArray *operands = g_array_new(FALSE, FALSE, sizeof(gint));
    gboolean result = FALSE;
    gint i;
    /* Execute the stack. */
    for (i = 0; i < condition->len; i++) {
        const StratCondPart *part = &g_array_index(condition, StratCondPart, i);
        switch (part->token) {
        case STRAT_COND_VAR:
            if (!misc_condition_evaluate_var(part, operands, token_rep)) {
                /* In order to match the behavior of the previous
                 * implementation, we need to return FALSE if we
                 * failed to replace a variable. */
                goto done;
            }
	    break;
        case STRAT_COND_INT: {
	    int value = GPOINTER_TO_INT(part->value);
            g_array_append_val(operands, value);
            break;
	}
        default:
            misc_condition_evaluate(part, operands);
            break;
        }
    }

    result = g_array_index(operands, gint, 0);
done:
    g_array_free(operands, TRUE);
    return result;
}

struct replacement_token_value {
    const char *token;
    enum ReplacementToken value;
};

struct replacement_token_key {
    const char *token;
    gint len;
};

static int replacement_token_bsearch(const void *key, const void *member)
{
    const struct replacement_token_value *value = member;
    const struct replacement_token_key *rkey = key;
    return strncmp(rkey->token, value->token, rkey->len);
}

enum ReplacementToken
misc_parse_replacement_token(const char *token, gint len)
{
    const static struct replacement_token_value table[] = {
        {"_AT_", REP_TOKEN_ATTENDANCE},
        {"_AVSKILLDIFF_", REP_TOKEN_AVSKILLDIFF},
        {"_CUPAUX_", REP_TOKEN_BOOL_CUP_AUX},
        {"_CUPET_", REP_TOKEN_BOOL_CUP_EXTRA},
        {"_CUPFIRSTLEG_", REP_TOKEN_BOOL_CUP_FIRST_LEG},
        {"_CUPHOMEAWAY_", REP_TOKEN_BOOL_CUP_HOME_AWAY},
        {"_CUPINTERNATIONAL_", REP_TOKEN_BOOL_CUP_INTERNATIONAL},
        {"_CUPKO_", REP_TOKEN_BOOL_CUP_KNOCKOUT},
        {"_CUPMATCHLOSERN_", REP_TOKEN_CUP_MATCH_LOSERN},
        {"_CUPMATCHLOSER_", REP_TOKEN_CUP_MATCH_LOSER},
        {"_CUPMATCHWINNERN_", REP_TOKEN_CUP_MATCH_WINNERN},
        {"_CUPMATCHWINNER_", REP_TOKEN_CUP_MATCH_WINNER},
        {"_CUPNATIONAL_", REP_TOKEN_BOOL_CUP_NATIONAL},
        {"_CUPNEUTRAL_", REP_TOKEN_BOOL_CUP_NEUTRAL},
        {"_CUPPEN_", REP_TOKEN_BOOL_CUP_PENALTIES},
        {"_CUPPROMREL_", REP_TOKEN_BOOL_CUP_PROMREL},
        {"_CUPROUNDNAME_", REP_TOKEN_CUP_ROUND_NAME},
        {"_CUPSECONDLEG_", REP_TOKEN_BOOL_CUP_SECOND_LEG},
        {"_CUPSTAGE_", REP_TOKEN_CUP_STAGE},
        {"_CUP_", REP_TOKEN_BOOL_CUP},
        {"_CUP_", REP_TOKEN_CUP},
        {"_EX_", REP_TOKEN_EXTRA},
        {"_FORMATION_", REP_TOKEN_FORM},
        {"_FOULS0_", REP_TOKEN_STAT_FOULS0},
        {"_FOULS1_", REP_TOKEN_STAT_FOULS1},
        {"_GDAGG_", REP_TOKEN_GOAL_DIFF_AGGREGATE},
        {"_GD_", REP_TOKEN_GOAL_DIFF},
        {"_GOALS0_", REP_TOKEN_GOALS0},
        {"_GOALS1_", REP_TOKEN_GOALS1},
        {"_GOALSTOWIN_", REP_TOKEN_GOALS_TO_WIN},
        {"_HIGHGOALS0_", REP_TOKEN_HIGHSCORER_GOALS0},
        {"_HIGHGOALS1_", REP_TOKEN_HIGHSCORER_GOALS1},
        {"_HIGHSCORER0_", REP_TOKEN_HIGHSCORER0},
        {"_HIGHSCORER1_", REP_TOKEN_HIGHSCORER1},
        {"_HOMEADV_", REP_TOKEN_HOMEADV},
        {"_INJS0_", REP_TOKEN_STAT_INJS0},
        {"_INJS1_", REP_TOKEN_STAT_INJS1},
        {"_LEAGUECUPNAME_", REP_TOKEN_LEAGUE_CUP_NAME},
        {"_LEAGUELOST0_", REP_TOKEN_STREAK_LEAGUE_LOST0},
        {"_LEAGUELOST1_", REP_TOKEN_STREAK_LEAGUE_LOST1},
        {"_LEAGUEUNBEATEN0_", REP_TOKEN_STREAK_LEAGUE_UNBEATEN0},
        {"_LEAGUEUNBEATEN1_", REP_TOKEN_STREAK_LEAGUE_UNBEATEN1},
        {"_LEAGUEWON0_", REP_TOKEN_STREAK_LEAGUE_WON0},
        {"_LEAGUEWON1_", REP_TOKEN_STREAK_LEAGUE_WON1},
        {"_LOST0_", REP_TOKEN_STREAK_LOST0},
        {"_LOST1_", REP_TOKEN_STREAK_LOST1},
        {"_MI_", REP_TOKEN_MINUTE},
        {"_MR_", REP_TOKEN_MINUTE_REMAINING},
        {"_MT_", REP_TOKEN_MINUTE_TOTAL},
        {"_MULTIPLESCORERS0_", REP_TOKEN_BOOL_MULTIPLE_SCORERS0},
        {"_MULTIPLESCORERS1_", REP_TOKEN_BOOL_MULTIPLE_SCORERS1},
        {"_NPOSS_", REP_TOKEN_NO_POSSESSION},
        {"_NUMATT_", REP_TOKEN_NUM_ATT},
        {"_NUMDEF_", REP_TOKEN_NUM_DEF},
        {"_NUMMID_", REP_TOKEN_NUM_MID},
        {"_OLDRANK0_", REP_TOKEN_OLDRANK0},
        {"_OLDRANK1_", REP_TOKEN_OLDRANK1},
        {"_OPPSKILL_", REP_TOKEN_OPPONENT_SKILL},
        {"_P0_", REP_TOKEN_PLAYER0},
        {"_P1_", REP_TOKEN_PLAYER1},
        {"_PEN0_", REP_TOKEN_STAT_PEN0},
        {"_PEN1_", REP_TOKEN_STAT_PEN1},
        {"_PLGOALS0_", REP_TOKEN_PLAYER_GOALS0},
        {"_PLGOALS1_", REP_TOKEN_PLAYER_GOALS1},
        {"_PLGOALSALL0_", REP_TOKEN_PLAYER_GOALS_ALL0},
        {"_PLGOALSALL1_", REP_TOKEN_PLAYER_GOALS_ALL1},
        {"_PLYELLOWS_", REP_TOKEN_PLAYER_YELLOWS},
        {"_POSS0_", REP_TOKEN_STAT_POSS0},
        {"_POSS1_", REP_TOKEN_STAT_POSS1},
        {"_POSS_", REP_TOKEN_POSSESSION},
        {"_RANK0_", REP_TOKEN_RANK0},
        {"_RANK1_", REP_TOKEN_RANK1},
        {"_REDS0_", REP_TOKEN_STAT_REDS0},
        {"_REDS1_", REP_TOKEN_STAT_REDS1},
        {"_REL_", REP_TOKEN_RESULT_REL},
        {"_REW_", REP_TOKEN_RESULT_REW},
        {"_RE_", REP_TOKEN_RESULT},
        {"_SCORERS0_", REP_TOKEN_SCORERS0},
        {"_SCORERS1_", REP_TOKEN_SCORERS1},
        {"_SHOTPER0_", REP_TOKEN_STAT_SHOT_PER0},
        {"_SHOTPER1_", REP_TOKEN_STAT_SHOT_PER1},
        {"_SHOTS0_", REP_TOKEN_STAT_SHOTS0},
        {"_SHOTS1_", REP_TOKEN_STAT_SHOTS1},
        {"_SUBSLEFT_", REP_TOKEN_SUBS_LEFT},
        {"_T0_", REP_TOKEN_TEAM_HOME},
        {"_T1_", REP_TOKEN_TEAM_AWAY},
        {"_TAVSKILL0_", REP_TOKEN_TEAM_AVSKILL0},
        {"_TAVSKILL1_", REP_TOKEN_TEAM_AVSKILL1},
        {"_TAVSKILLDIFF_", REP_TOKEN_TEAM_AVSKILLDIFF},
        {"_TIME_", REP_TOKEN_TIME},
        {"_TLAYER0_", REP_TOKEN_TEAM_LAYER0},
        {"_TLAYER1_", REP_TOKEN_TEAM_LAYER1},
        {"_TLAYERDIFF_", REP_TOKEN_TEAM_LAYERDIFF},
        {"_TLN_", REP_TOKEN_TEAM_LOSINGN},
        {"_TL_", REP_TOKEN_TEAM_LOSING},
        {"_TT_", REP_TOKEN_TEAM},
        {"_TWN_", REP_TOKEN_TEAM_WINNINGN},
        {"_TW_", REP_TOKEN_TEAM_WINNING},
        {"_UNBEATEN0_", REP_TOKEN_STREAK_UNBEATEN0},
        {"_UNBEATEN1_", REP_TOKEN_STREAK_UNBEATEN1},
        {"_WON0_", REP_TOKEN_STREAK_WON0},
        {"_WON1_", REP_TOKEN_STREAK_WON1},
        {"_YELLOWLIMIT_", REP_TOKEN_YELLOW_LIMIT},
        {"_YELLOWS0_", REP_TOKEN_STAT_YELLOWS0},
        {"_YELLOWS1_", REP_TOKEN_STAT_YELLOWS1}
    };

    struct replacement_token_key string;
    string.token = token;
    string.len = len;
    struct replacement_token_value *found = bsearch(&string,
                                                    table,
                                                    sizeof(table) / sizeof(struct replacement_token_value),
                                                    sizeof(struct replacement_token_value),
                                                    replacement_token_bsearch);
    if (!found) {
        return REP_TOKEN_LAST;
    }
    return found->value;
}
