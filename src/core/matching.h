/*
 * $Id$
 *
 * Copyright (c) 2001-2003, Raphael Manfredi
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */

#ifndef _core_matching_h_
#define _core_matching_h_

/*
 * Basic explanation of how search table works:
 *
 *    A search_table is a global object.  Only one of these is expected to
 *  exist.  It consists of a number of "bins", each bin containing all
 *  entries which have a certain sequence of two characters in a row, plus
 *  some metadata.
 *
 *    Each bin is a simple array, without repetitions, of items.  Each item
 *  consists of a string to which a certain mapping of characters onto
 *  characters has been applied, plus a void * representing the actual data
 *  mapped to.  (I used void * to make this code reasonably generic, so that
 *  in any project I or someone else wants to use code like this for, they
 *  can just use it.)  The same mapping is also applied to each search before
 *  running it.  This maps uppercase and lowercase letters to match one
 *  another, maps all whitespace and punctuation to a simple space, etc.
 *  This mechanism is very flexible and could easily be adapted to match
 *  accented characters, etc.
 *
 *    The actual search builds a regular expression to do the matching.  This
 *  might have a tiny bit higher overhead than a custom implementation of
 *  string matching, but it also allows a great deal of flexibility and ease
 *  of experimentation with different search techniques, both to increase
 *  efficiency and to give the best possible results.  Hopefully the code is
 *  reasonably self-explanatory, but if you find it confusing, email the
 *  gtk-gnutella-devel mailing list and I'll try to respond...
 *
 *    -- KBH, 2001-10-03
 */

struct st_entry {
	gchar *string;
	void *data;
	guint32 mask;
};

struct st_bin {
	gint nslots, nvals;
	struct st_entry **vals;
};

typedef	guint8 char_map_t[256];		/* Maps one char to another */

typedef struct _search_table {
	gint nentries, nchars, nbins;
	struct st_bin **bins;
	struct st_bin all_entries;
	char_map_t index_map, fold_map;
} search_table_t;


struct query_hashvec;

guint match_map_string(char_map_t map, gchar *string);

void st_initialize(search_table_t *, char_map_t);
void st_create(search_table_t *table);
void st_destroy(search_table_t *);
void st_insert_item(search_table_t *, const gchar *, void *);
void st_compact(search_table_t *);

struct shared_file;

/* FIXME: The type of this callback is too specific. */
typedef gboolean (*st_search_callback)(struct shared_file *);

gint st_search(
	search_table_t *table,
	gchar *search,
	st_search_callback callback,
	gint max_res,
	struct query_hashvec *qhv);

#endif	/* _core_matching_h_ */

/* vi: set ts=4: */
