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

/**
 * @ingroup core
 * @file
 *
 * Header parsing routines.
 *
 * @author Raphael Manfredi
 * @date 2001-2003
 */

#include "common.h"

RCSID("$Id$")

#include "features.h"

#include "if/gnet_property_priv.h"
#include "if/core/main.h"

#include "lib/ascii.h"
#include "lib/header.h"
#include "lib/glib-missing.h"
#include "lib/walloc.h"

#include "lib/override.h"		/* Must be the last header included */

struct header_x_feature {
	char *name;
	int major;
	int minor;
	const gboolean *guard;
};

struct features {
	GList *list;
};

static struct features * 
features_get(xfeature_t xf)
{
	static struct features features[NUM_FEATURES];
	guint i;

	i = (guint) xf;
	g_return_val_if_fail(i < G_N_ELEMENTS(features), NULL);
	return &features[i];
}

/***
 *** X-Features header parsing utilities
 ***/

/**
 * Removes all memory used by the header_features_add.
 */
static void
header_features_cleanup(xfeature_t xf)
{
	struct features *features;
	GList *cur;

 	features = features_get(xf);
	g_return_if_fail(features);

   	cur = g_list_first(features->list);
	for (/* NOTHING */; NULL != cur; cur = g_list_next(cur)) {
		struct header_x_feature *header = cur->data;

		G_FREE_NULL(header->name);
		wfree(header, sizeof *header);
	}
	g_list_free(features->list);
	features->list = NULL;
}

void
features_close(void)
{
	guint i;

	for (i = 0; i < NUM_FEATURES; i++) {
		header_features_cleanup(i);
	}
}

/**
 * Add conditional support for feature ``name'': if at run-time the value
 * pointed-at by ``guard'' is FALSE, the feature is not emitted.
 */
void
header_features_add_guarded(xfeature_t xf,
	const char *name, int major, int minor, const gboolean *guard)
{
	struct header_x_feature *item;
	struct features *features;

	features = features_get(xf);
	g_return_if_fail(features);

	item = walloc(sizeof *item);
	item->name = g_strdup(name);
	item->major = major;
	item->minor = minor;
	item->guard = guard;

	features->list = g_list_append(features->list, item);
}

/**
 * Add support for feature ``name'' with the specified version to the
 * X-Features header.
 */
void
header_features_add(xfeature_t xf, const char *name, int major, int minor)
{
	header_features_add_guarded(xf, name, major, minor, NULL);
}

/**
 * Adds the X-Features header to a HTTP request.
 *
 * @param xfeatures	structure holding the collected features
 * @param buf		should point to the beginning of the header.
 * @param len		no brief description.
 * @param rw		the number of bytes that were already written.
 *
 * *rw is changed too *rw + bytes written
 */
void
header_features_generate(xfeature_t xf, char *dst, size_t len, size_t *rw)
{
	static const char hdr[] = "X-Features";
	struct features *features;
	GList *cur;
	header_fmt_t *fmt;

	g_assert(len <= INT_MAX);
	g_assert(*rw <= INT_MAX);
	g_assert(len >= *rw);

	if (len - *rw < (sizeof(hdr) + sizeof(": \r\n") - 1))
		return;

	features = features_get(xf);
	g_return_if_fail(features);

	if (g_list_first(features->list) == NULL)
		return;

	fmt = header_fmt_make(hdr, ", ", 0, len - *rw);

	for (cur = g_list_first(features->list); cur; cur = g_list_next(cur)) {
		struct header_x_feature *item = cur->data;
		char buf[50];

		if (item->guard && !*item->guard)
			continue;

		gm_snprintf(buf, sizeof buf, "%s/%d.%d",
			item->name, item->major, item->minor);

		header_fmt_append_value(fmt, buf);
	}

	header_fmt_end(fmt);

	if (header_fmt_length(fmt) < len - *rw) {
		*rw += clamp_strncpy(&dst[*rw], len - *rw,
				header_fmt_string(fmt), header_fmt_length(fmt));
	}

	header_fmt_free(&fmt);
}

/**
 * Retrieves the major and minor version from a feature in the X-Features
 * header, if no support was found both major and minor are 0 and FALSE
 * is returned.
 */
gboolean
header_get_feature(const char *name, const header_t *header,
	guint *major, guint *minor)
{
	const char *x_features = "X-Features";
	char *buf = NULL;
	char *start;

	if (major)
		*major = 0;
	if (minor)
		*minor = 0;

	buf = header_get(header, x_features);

	/*
	 * We could also try to scan for the header: name, so this would
     * make this function even more generic. But I would suggest another
     * function for this though.
     */

	if (buf == NULL) {
		/*
		 * Actually the 'specs' say we should assume it is supported if the
		 * X-Features header is not there. But I wouldn't count on it, and
		 * it was only for "legacy" attributes in the HTTP file exchange.
		 * Better safe than sorry.
		 */

		return FALSE;
	}

	/*
	 * We must locate the name exactly, and not a subpart of another
	 * feature.  If we look for "bar", then we must not match on "foobar".
	 */

	start = buf;
	for (;;) {
		int pc;			/* Previous char */

		buf = ascii_strcasestr(buf, name);

		if (buf == NULL)
			return FALSE;
		if (buf == start)
			break;

		pc = *(buf - 1);
		if (is_ascii_space(pc) || pc == ',' || pc == ';')
			break;			/* Found it! */

		/*
		 * Since we're looking for whole words separated by a space or the
		 * regular header punctuation, the next match can't occur before
		 * the end of the current string we matched...
		 */

		buf += strlen(name);
	}

	buf += strlen(name);		/* Should now be on the "/" sep */

	if (*buf != '/') {
		if (debugging(0))
			g_warning("malformed %s header \"%s\", ignoring",
				x_features, NULL_STRING(header_get(header, x_features)));
		return FALSE;
	}

	buf++;

	if (*buf == '\0')
		return FALSE;

	return 0 == parse_major_minor(buf, NULL, major, minor);
}

/* vi: set ts=4 sw=4 cindent: */
