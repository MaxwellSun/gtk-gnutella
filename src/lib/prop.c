/*
 * $Id$
 *
 * Copyright (c) 2001-2003, Richard Eckart
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

#include "common.h"

RCSID("$Id$");

#include "prop.h"
#include "file.h"
#include "misc.h"
#include "glib-missing.h"
#include "override.h"		/* Must be the last header included */

#define debug track_props
static guint32 track_props = 0;	/* XXX need to init lib's props--RAM */
static guint32 common_dbg = 0;	/* XXX -- need to init lib's props --RAM */

/*
 * Checks if a property is part of a property set.
 */
#define prop_in_range(ps, p) ((p >= ps->offset) && (p < ps->size+ps->offset))

const gchar * const prop_type_str[] = {
	"boolean",
	"guint32",
	"guint64",
	"string",
	"ip",
	"storage",
	"multichoice"
};

static const char hex_alphabet_lower[] = "0123456789abcdef";

static gchar prop_tmp[4096];

/***
 *** Helpers
 ***/

#define prop_assert(ps, prop, x)									\
do {																\
	if (!(x)) {														\
		g_error("assertion failed for property \"%s\": %s",			\
			PROP(ps, prop).name, #x);								\
	}																\
} while (0)

/*
 * prop_parse_ip_vector:
 *
 * Parse comma delimited numeric vector.
 */
static void prop_parse_ip_vector(const gchar *name,
	const gchar *str, gsize size, guint32 *t)
{
	/* Parse comma delimited settings */

	gchar **h;
	gsize i;

	g_assert(str != NULL);
	g_assert(t != NULL);

	h = g_strsplit(str, ",", size + 1);

	for (i = 0; i < size; i++) {
		if (!h[i])
			break;

		t[i] = gchar_to_ip(h[i]);
	}

	if (i < size)
		g_warning("prop_parse_ip_vector: (prop=\"%s\") "
			"target initialization incomplete!", name);

	g_strfreev(h);
}
/*
 * prop_parse_guint64_vector:
 *
 * Parse comma delimited numeric vector.
 */
static void prop_parse_guint64_vector(const gchar *name,
	const gchar *str, gsize size, guint64 *t)
{
	/* Parse comma delimited settings */

	gchar **h;
	gsize i;

	g_assert(str != NULL);
	g_assert(t != NULL);

	h = g_strsplit(str, ",", size + 1);

	for (i = 0; i < size; i++) {
		int error;
		gulong v;

		if (!h[i])
			break;

		/* TODO: long is most likely a 32-bit integer! */
		v = gm_atoul(h[i], NULL, &error);
		if (error) {
			g_warning("prop_parse_guint64_vector: (prop=\"%s\") "
				"h[i]=\"%s\": \"%s\"", name, h[i], g_strerror(error));
			continue;
		}

		t[i] = v;
	}

	if (i < size)
		g_warning("prop_parse_guint64_vector: (prop=\"%s\") "
			"target initialization incomplete!", name);

	g_strfreev(h);
}

/*
 * prop_parse_guint32_vector:
 *
 * Parse comma delimited numeric vector.
 */
static void prop_parse_guint32_vector(const gchar *name,
	const gchar *str, gsize size, guint32 *t)
{
	/* Parse comma delimited settings */

	gchar **h;
	gsize i;

	g_assert(str != NULL);
	g_assert(t != NULL);

	h = g_strsplit(str, ",", size + 1);

	for (i = 0; i < size; i++) {
		int error;
		gulong v;

		if (!h[i])
			break;

		v = gm_atoul(h[i], NULL, &error);
		if (!error && (guint32) v != v)
			error = ERANGE;

		if (error) {
			g_warning("prop_parse_guint32_vector: "
				"(prop=\"%s\") h[i]=\"%s\": \"%s\"",
				name, h[i], g_strerror(error));
			continue;
		}

		t[i] = v;
	}

	if (i < size)
		g_warning("prop_parse_guint32_vector: (prop=\"%s\") "
			"target initialization incomplete!", name);

	g_strfreev(h);
}

/*
 * prop_parse_boolean_vector:
 *
 * Parse comma delimited boolean vector (TRUE/FALSE list).
 */
static void prop_parse_boolean_vector(const gchar *name,
	const gchar *str, gsize size, gboolean *t)
{
	/* Parse comma delimited settings */

	const gchar *a, *b, *p = str;
	gsize i;

	g_assert(str != NULL);
	g_assert(t != NULL);

	for (i = 0; *p != '\0' && i < size; i++) {
		size_t l;
		gboolean valid;

		while (is_ascii_space(*p))
			p++;
		if (*p == '\0')
			break;
		a = p;
		b = strchr(p, ',');
		if (!b)
			b = strchr(p, '\0');
		p = b--;
		g_assert(*p == ',' || *p == '\0');
		p++;

		while (b > a && is_ascii_space(*b))
			b--;

		l = (b - a) + 1;
		valid = FALSE;
		switch (l) {
		case sizeof "TRUE" - 1:
			if (0 == g_ascii_strncasecmp(a, "TRUE", l)) {
				t[i] = TRUE;
				valid = TRUE;
			}
			break;
		case sizeof "FALSE" - 1:
			if (0 == g_ascii_strncasecmp(a, "FALSE", l)) {
				t[i] = FALSE;
				valid = TRUE;
			}
			break;
		case sizeof "1" - 1:
			if (*a == '1') {
				t[i] = TRUE;
				valid = TRUE;
			} else if (*a == '0') {
				t[i] = FALSE;
				valid = TRUE;
			}
			break;
		}
		if (!valid) {
			g_warning("Not a boolean value (prop=\"%s\"): \"%s\"", name, a);
		}
	}

	if (i != size)
		g_warning("prop_parse_boolean_vector: (prop=\"%s\") "
			"target initialization incomplete!", name);
}

/*
 * prop_parse_storage:
 *
 * Parse a hex string into a guint8 array.
 */
static void prop_parse_storage(const gchar *name,
	const gchar *str, gsize size, guint8 *t)
{
	gsize i;

	g_assert(size > 0);
	if (size * 2 != strlen(str))
		g_error("prop_parse_storage: (prop=\"%s\") "
			"storage does not match requested size", name);

	for (i = 0; i < size; i++) {
		guchar h, l;

		h = str[i * 2];
		l = str[i * 2 + 1];
		if (!is_ascii_xdigit(h) || !is_ascii_xdigit(l)) {
			t[i] = '\0';
			g_warning("prop_parse_storage: (prop=\"%s\") "
				"storage is damaged: \"%s\"", name, str);
			return;
		}
		t[i] = (hex2dec(h) << 4) + hex2dec(l);
	}
}



/***
 *** Properties
 ***/

/*
 * prop_get_def:
 *
 * Copy the property definition from the property set and return it.
 * Use the prop_free_def call to free the memory again. A simple g_free
 * won't do, since there are lot's of pointers to allocated memory
 * in the definition structure.
 * The prop_changed_listeners field will always be NULL in the copy.
 */
prop_def_t *prop_get_def(prop_set_t *ps, property_t p)
{
	prop_def_t *buf;

	g_assert(ps != NULL);

	if (!prop_in_range(ps, p))
		g_error("prop_get_def: unknown property %d", p);

	buf = g_memdup(&PROP(ps, p), sizeof(prop_def_t));
	buf->name = g_strdup(PROP(ps, p).name);
	buf->desc = g_strdup(PROP(ps, p).desc);
	buf->ev_changed = NULL;

	switch(buf->type) {
	case PROP_TYPE_BOOLEAN:
		buf->data.boolean.def = g_memdup(
			PROP(ps,p).data.boolean.def,
			sizeof(gboolean) * PROP(ps,p).vector_size);
		buf->data.boolean.value = g_memdup(
			PROP(ps,p).data.boolean.value,
			sizeof(gboolean) * PROP(ps,p).vector_size);
		break;
	case PROP_TYPE_MULTICHOICE: {
		guint n = 0;

		while (PROP(ps,p).data.guint32.choices[n].title != NULL)
			n++;

		n ++; /* Keep space for terminating {NULL, 0} field */

		buf->data.guint32.choices = g_memdup(
			PROP(ps,p).data.guint32.choices,
			sizeof(prop_def_choice_t) * n);

		buf->data.guint32.choices[n-1].title = NULL;
		buf->data.guint32.choices[n-1].value = 0;

		n = 0;
		while (PROP(ps,p).data.guint32.choices[n].title != NULL) {
			buf->data.guint32.choices[n].title =
				g_strdup(PROP(ps,p).data.guint32.choices[n].title);
			n++;
		}
		/* no break -> continue to PROP_TYPE_GUINT32 */
	}
	case PROP_TYPE_IP:
	case PROP_TYPE_GUINT32:
		buf->data.guint32.def = g_memdup(
			PROP(ps,p).data.guint32.def,
			sizeof(guint32) * PROP(ps,p).vector_size);
		buf->data.guint32.value = g_memdup(
			PROP(ps,p).data.guint32.value,
			sizeof(guint32) * PROP(ps,p).vector_size);
		break;
	case PROP_TYPE_GUINT64:
		buf->data.guint64.def = g_memdup(
			PROP(ps,p).data.guint64.def,
			sizeof(guint64) * PROP(ps,p).vector_size);
		buf->data.guint64.value = g_memdup(
			PROP(ps,p).data.guint64.value,
			sizeof(guint64) * PROP(ps,p).vector_size);
	case PROP_TYPE_STRING:
		buf->data.string.def	= g_new(gchar*, 1);
		*buf->data.string.def   = g_strdup(*PROP(ps,p).data.string.def);
		buf->data.string.value  = g_new(gchar*, 1);
		*buf->data.string.value = g_strdup(*PROP(ps,p).data.string.value);
		break;
	case PROP_TYPE_STORAGE:
		buf->data.storage.value = g_memdup
			(PROP(ps,p).data.storage.value, PROP(ps,p).vector_size);
	}

	return buf;
}

void prop_free_def(prop_def_t *d)
{
	g_assert(d != NULL);

	switch (d->type) {
	case PROP_TYPE_BOOLEAN:
		G_FREE_NULL(d->data.boolean.value);
		G_FREE_NULL(d->data.boolean.def);
		break;
	case PROP_TYPE_MULTICHOICE: {
		guint n = 0;

		while (d->data.guint32.choices[n].title != NULL) {
			G_FREE_NULL(d->data.guint32.choices[n].title);
			n++;
		}

		G_FREE_NULL(d->data.guint32.choices);
		/* no break -> continue to PROP_TYPE_GUINT32 */
	}
	case PROP_TYPE_IP:
	case PROP_TYPE_GUINT32:
		G_FREE_NULL(d->data.guint32.value);
		G_FREE_NULL(d->data.guint32.def);
		break;
	case PROP_TYPE_GUINT64:
		G_FREE_NULL(d->data.guint64.value);
		G_FREE_NULL(d->data.guint64.def);
		break;
	case PROP_TYPE_STRING:
		if (*d->data.string.value)
			G_FREE_NULL(*d->data.string.value);
		if (*d->data.string.def)
			G_FREE_NULL(*d->data.string.def);
		G_FREE_NULL(d->data.string.value);
		G_FREE_NULL(d->data.string.def);
		break;
	case PROP_TYPE_STORAGE:
		G_FREE_NULL(d->data.storage.value);
	}
	G_FREE_NULL(d->name);
	G_FREE_NULL(d->desc);
	G_FREE_NULL(d);
}

/*
 * prop_add_prop_changed_listener:
 *
 * Add a change listener to a given property. If init is TRUE then
 * the listener is immediately called.
 */
void prop_add_prop_changed_listener(
	prop_set_t *ps, property_t prop, prop_changed_listener_t l, gboolean init)
{
	prop_add_prop_changed_listener_full(ps, prop, l, init, FREQ_SECS, 0);
}

/*
 * prop_add_prop_changed_listener_full:
 *
 * Add a change listener to a given property. If init is TRUE then
 * the listener is immediately called.
 */
void prop_add_prop_changed_listener_full(
	prop_set_t *ps, property_t prop, prop_changed_listener_t l,
	gboolean init, enum frequency_type freq, guint32 interval)
{
	g_assert(ps != NULL);
	g_assert(prop_in_range(ps, prop));

	event_add_subscriber(
		PROP(ps,prop).ev_changed, (GCallback) l, freq, interval);

	if (init)
		(*l)(prop);
}

void prop_remove_prop_changed_listener(
	prop_set_t *ps, property_t prop, prop_changed_listener_t l)
{
	g_assert(ps != NULL);
	g_assert(prop_in_range(ps, prop));

	event_remove_subscriber(PROP(ps,prop).ev_changed, (GCallback) l);
}

static void prop_emit_prop_changed(prop_set_t *ps, property_t prop)
{
	g_assert(ps != NULL);

	event_trigger(
		PROP(ps,prop).ev_changed,
		T_VETO(prop_changed_listener_t, prop));

	if (PROP(ps,prop).save)
		ps->dirty = TRUE;
}

void prop_set_boolean(
	prop_set_t *ps, property_t prop, const gboolean *src,
	guint32 offset, guint32 length)
{
	gboolean old;
	gboolean new;
	gboolean differ = FALSE;
	guint32 n;

	g_assert(ps != NULL);
	g_assert(src != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_set_boolean: unknown property %d", prop);
	if (PROP(ps,prop).type != PROP_TYPE_BOOLEAN)
		g_error("Type mismatch setting value for [%s] of type"
			" %s when %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_BOOLEAN]);

	if (length == 0)
		length = PROP(ps,prop).vector_size;

	prop_assert(ps, prop, offset + length <= PROP(ps,prop).vector_size);

	for (n = 0; (n < length) && !differ; n++) {
		old = PROP(ps,prop).data.boolean.value[n + offset] ? 1 : 0;
		new = src[n] ? 1 : 0;

		if (old != new)
			differ = TRUE;
	}

	if (!differ)
		return;

	memcpy(&PROP(ps,prop).data.boolean.value[offset], src,
		length * sizeof(gboolean));

	if (debug >= 5) {
		guint32 i;

		printf("updated property [%s] = ( ", PROP(ps,prop).name);

		for (i = 0; i < PROP(ps,prop).vector_size; i++)
			printf("%s%s ",
				PROP(ps,prop).data.boolean.value[i] ? "TRUE" : "FALSE",
				(i < (PROP(ps,prop).vector_size-1)) ? "," : "");

		printf(")\n");
	}

	prop_emit_prop_changed(ps, prop);
}

gboolean *prop_get_boolean(
	prop_set_t *ps, property_t prop, gboolean *t,
	guint32 offset, guint32 length)
{
	gboolean *target;

	g_assert(ps != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_get_boolean: unknown property %d", prop);
	if (PROP(ps,prop).type != PROP_TYPE_BOOLEAN)
		g_error("Type mismatch setting value for [%s] of type"
			" %s when %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_BOOLEAN]);

	if (length == 0)
		length = PROP(ps,prop).vector_size;

	prop_assert(ps, prop, offset + length <= PROP(ps,prop).vector_size);

	target = t != NULL ? t : g_malloc(length * sizeof(gboolean));
	memcpy(target, &PROP(ps,prop).data.boolean.value[offset],
		sizeof(gboolean) * length);

	return target;
}

void prop_set_guint64(
	prop_set_t *ps, property_t prop, const guint64 *src,
	guint64 offset, guint64 length)
{
	gboolean differ = FALSE;

	g_assert(ps != NULL);
	g_assert(src != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_set_guint64: unknown property %d", prop);
	if ((PROP(ps,prop).type != PROP_TYPE_GUINT64) )
		g_error("Type mismatch setting value for [%s] of type"
			" %s when %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_GUINT64]);

	if (length == 0)
		length = PROP(ps,prop).vector_size;

	prop_assert(ps, prop, offset + length <= PROP(ps,prop).vector_size);

	differ = (memcmp(&PROP(ps,prop).data.guint64.value[offset], src,
		length * sizeof(guint64)) != 0);

	if (!differ)
		return;

	/*
	 * Only do bounds-checking on non-vector properties.
	 */
	if (PROP(ps,prop).vector_size == 1) {
		/*
		 * Either check multiple choices or min/max.
		 */
			prop_assert(ps, prop, PROP(ps,prop).data.guint64.choices == NULL);

			if (
				(PROP(ps,prop).data.guint64.min <= *src) &&
				(PROP(ps,prop).data.guint64.max >= *src)
			) {
				*PROP(ps,prop).data.guint64.value = *src;
			} else {
				guint64 newval = *src;

				if (newval > PROP(ps,prop).data.guint64.max)
					newval = PROP(ps,prop).data.guint64.max;
				if (newval < PROP(ps,prop).data.guint64.min)
					newval = PROP(ps,prop).data.guint64.min;

				g_warning("prop_set_guint64: [%s] new value out of bounds "
					"(%" PRIu64 "/%" PRIu64 "): %" PRIu64
						" (adjusting to %" PRIu64 ")",
					PROP(ps,prop).name,
					PROP(ps,prop).data.guint64.min,
					PROP(ps,prop).data.guint64.max,
					*src, newval );

				*PROP(ps,prop).data.guint64.value = newval;
			}
	} else {
		memcpy(&PROP(ps,prop).data.guint64.value[offset], src,
			sizeof(guint64) * length);
	}

	if (debug >= 5) {
		guint64 n;

		printf("updated property [%s] = ( ", PROP(ps,prop).name);

		for (n = 0; n < PROP(ps,prop).vector_size; n++) {
			if (PROP(ps,prop).type == PROP_TYPE_IP) {
				printf("%s%s ", ip_to_gchar(
					PROP(ps,prop).data.guint64.value[n]),
					(n < (PROP(ps,prop).vector_size-1)) ? "," : "");
			} else {
				printf("%" PRIu64 "%s ", PROP(ps,prop).data.guint64.value[n],
					(n < (PROP(ps,prop).vector_size-1)) ? "," : "");
			}
		}

		printf(")\n");
	}

	prop_emit_prop_changed(ps, prop);
}

guint64 *prop_get_guint64(
	prop_set_t *ps, property_t prop, guint64 *t,
	guint64 offset, guint64 length)
{
	guint64 *target;

	g_assert(ps != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_get_guint64: unknown property %d", prop);
	if ((PROP(ps,prop).type != PROP_TYPE_GUINT64))
		g_error("Type mismatch setting value for [%s] of type"
			" %s when %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_GUINT64]);

   if (length == 0)
		length = PROP(ps,prop).vector_size;

	prop_assert(ps, prop, offset + length <= PROP(ps,prop).vector_size);

	target = t != NULL ? t : g_malloc(length * sizeof(guint64));
	memcpy(target, &PROP(ps,prop).data.guint64.value[offset],
		sizeof(guint64) * length);

	return target;
}

void prop_set_guint32(
	prop_set_t *ps, property_t prop, const guint32 *src,
	guint32 offset, guint32 length)
{
	gboolean differ = FALSE;

	g_assert(ps != NULL);
	g_assert(src != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_set_guint32: unknown property %d", prop);
	if ((PROP(ps,prop).type != PROP_TYPE_GUINT32) &&
	   (PROP(ps,prop).type != PROP_TYPE_IP) &&
	   (PROP(ps,prop).type != PROP_TYPE_MULTICHOICE) )
		g_error("Type mismatch setting value for [%s] of type"
			" %s when %s, %s or %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_GUINT32],
			prop_type_str[PROP_TYPE_IP],
			prop_type_str[PROP_TYPE_MULTICHOICE]);

	if (length == 0)
		length = PROP(ps,prop).vector_size;

	prop_assert(ps, prop, offset + length <= PROP(ps,prop).vector_size);

	differ = (memcmp(&PROP(ps,prop).data.guint32.value[offset], src,
		length * sizeof(guint32)) != 0);

	if (!differ)
		return;

	/*
	 * Only do bounds-checking on non-vector properties.
	 */
	if (PROP(ps,prop).vector_size == 1) {
		/*
		 * Either check multiple choices or min/max.
		 */
		if (PROP(ps,prop).type == PROP_TYPE_MULTICHOICE) {
			guint n = 0;
			gboolean invalid = TRUE;
			guint32 newval = *src;

			prop_assert(ps, prop, PROP(ps,prop).data.guint32.choices != NULL);

			while (PROP(ps,prop).data.guint32.choices[n].title != NULL) {
				if (PROP(ps,prop).data.guint32.choices[n].value == newval) {
					invalid = FALSE;
					break;
				}
				n++;
			}

			if (invalid) {
				g_warning("prop_set_guint32: [%s] new value is invalid "
					"choice %u (leaving at %u)",
					PROP(ps,prop).name, newval, *src);
			} else {
				*PROP(ps,prop).data.guint32.value = newval;
			}
		} else {

			prop_assert(ps, prop, PROP(ps,prop).data.guint32.choices == NULL);

			if (
				(PROP(ps,prop).data.guint32.min <= *src) &&
				(PROP(ps,prop).data.guint32.max >= *src)
			) {
				*PROP(ps,prop).data.guint32.value = *src;
			} else {
				guint32 newval = *src;

				if (newval > PROP(ps,prop).data.guint32.max)
					newval = PROP(ps,prop).data.guint32.max;
				if (newval < PROP(ps,prop).data.guint32.min)
					newval = PROP(ps,prop).data.guint32.min;

				g_warning("prop_set_guint32: [%s] new value out of bounds "
					"(%u/%u): %u (adjusting to %u)",
					PROP(ps,prop).name,
					PROP(ps,prop).data.guint32.min,
					PROP(ps,prop).data.guint32.max,
					*src, newval );

				*PROP(ps,prop).data.guint32.value = newval;
			}
		}
	} else {
		memcpy(&PROP(ps,prop).data.guint32.value[offset], src,
			sizeof(guint32) * length);
	}

	if (debug >= 5) {
		guint32 n;

		printf("updated property [%s] = ( ", PROP(ps,prop).name);

		for (n = 0; n < PROP(ps,prop).vector_size; n++) {
			if (PROP(ps,prop).type == PROP_TYPE_IP) {
				printf("%s%s ", ip_to_gchar(
					PROP(ps,prop).data.guint32.value[n]),
					(n < (PROP(ps,prop).vector_size-1)) ? "," : "");
			} else {
				printf("%u%s ", PROP(ps,prop).data.guint32.value[n],
					(n < (PROP(ps,prop).vector_size-1)) ? "," : "");
			}
		}

		printf(")\n");
	}

	prop_emit_prop_changed(ps, prop);
}

guint32 *prop_get_guint32(
	prop_set_t *ps, property_t prop, guint32 *t,
	guint32 offset, guint32 length)
{
	guint32 *target;

	g_assert(ps != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_get_guint32: unknown property %d", prop);
	if ((PROP(ps,prop).type != PROP_TYPE_GUINT32) &&
	   (PROP(ps,prop).type != PROP_TYPE_IP) &&
	   (PROP(ps,prop).type != PROP_TYPE_MULTICHOICE) )
		g_error("Type mismatch setting value for [%s] of type"
			" %s when %s, %s or %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_GUINT32],
			prop_type_str[PROP_TYPE_IP],
			prop_type_str[PROP_TYPE_MULTICHOICE]);

   	if (length == 0)
		length = PROP(ps,prop).vector_size;

	prop_assert(ps, prop, offset + length <= PROP(ps,prop).vector_size);

	target = t != NULL ? t : g_malloc(length * sizeof(guint32));
	memcpy(target, &PROP(ps,prop).data.guint32.value[offset],
		sizeof(guint32) * length);

	return target;
}

void prop_set_storage(
	prop_set_t *ps, property_t prop, const gchar *src, gsize length)
{
	gboolean differ = FALSE;

	g_assert(ps != NULL);
	g_assert(src != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_set_storage: unknown property %d", prop);
	if (PROP(ps,prop).type != PROP_TYPE_STORAGE)
		g_error("Type mismatch setting value for [%s] of type"
			" %s when %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_STORAGE]);

	prop_assert(ps, prop, length == PROP(ps,prop).vector_size);

	differ = (memcmp(PROP(ps,prop).data.storage.value, src, length) != 0);

	if (!differ)
		return;

	memcpy(PROP(ps,prop).data.storage.value, src, length);

	if (debug >= 5) {
		printf("updated property [%s] (binary)\n", PROP(ps,prop).name);
		dump_hex(stderr, PROP(ps,prop).name,
			(const gchar *) PROP(ps,prop).data.storage.value,
			PROP(ps,prop).vector_size);
	}

	prop_emit_prop_changed(ps, prop);
}

gchar *
prop_get_storage(prop_set_t *ps, property_t prop, gchar *t, gsize length)
{
	gpointer target;

	g_assert(ps != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_get_storage: unknown property %d", prop);
	if (PROP(ps,prop).type != PROP_TYPE_STORAGE)
		g_error("Type mismatch getting value for [%s] of type"
			" %s when %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_STORAGE]);

	prop_assert(ps, prop, length == PROP(ps,prop).vector_size);

	target = t != NULL ? t : g_malloc(length);
	memcpy(target, PROP(ps,prop).data.storage.value, length);

	return target;
}

void prop_set_string(prop_set_t *ps, property_t prop, const gchar *val)
{
	gchar *old;
	gboolean differ = FALSE;

	g_assert(ps != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_get_gchar: unknown property %d", prop);
	if (PROP(ps,prop).type != PROP_TYPE_STRING)
		g_error("Type mismatch getting value for [%s] of type"
			" %s when %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_STRING]);

	prop_assert(ps, prop, PROP(ps,prop).vector_size == 1);

	old = *PROP(ps,prop).data.string.value;

	if (val == NULL) {
		/*
		 * Clear property.
		 */
		if (old != NULL) {
			G_FREE_NULL(old);
			differ = TRUE;
		}
	} else {
		/*
		 * Update property.
		 */
		if (old != NULL) {
			differ = strcmp(old, val) != 0;
			G_FREE_NULL(old);
		} else {
			differ = TRUE;
		}
		*PROP(ps,prop).data.string.value = g_strdup(val);
	}

	if (differ && debug >= 5)
		printf("updated property [%s] = \"%s\"\n",
			PROP(ps,prop).name,
			*PROP(ps,prop).data.string.value);

	if (differ)
		prop_emit_prop_changed(ps, prop);
}

/*
 * prop_get_string:
 *
 * Fetches the value of a string property. If a string buffer is provided
 * (t != NULL), then this is used. The size indicates the size of the given
 * string buffer and may not be 0 in this case. The pointer which is
 * returned will point to the given buffer.
 * If no string buffer is given (t == NULL), new memory is allocated and
 * returned. This memory must be free'ed later. The size parameter has
 * no effect in this case.
 */
gchar *prop_get_string(prop_set_t *ps, property_t prop, gchar *t, guint32 size)
{
	gchar *target;
	gchar *s;

	g_assert(ps != NULL);

	if (t != NULL)
		g_assert(size > 0);

	if (!prop_in_range(ps, prop))
		g_error("prop_get_gchar: unknown property %d", prop);
	if (PROP(ps,prop).type != PROP_TYPE_STRING)
		g_error("Type mismatch getting value for [%s] of type"
			" %s when %s was expected",
			PROP(ps,prop).name,
			prop_type_str[PROP(ps,prop).type],
			prop_type_str[PROP_TYPE_STRING]);

	s = *PROP(ps,prop).data.string.value;

	target = t;
	if (target == NULL) {
		/*
		 * Create new string.
		 */
		target = g_strdup(s);
	} else {
		/*
		 * Use given string buffer.
		 */
		if (s == NULL) {
			target[0] = '\0';
			target = NULL;
		} else {
			g_strlcpy(target, s, size);
		}
	}

	return target;
}

/*
 * prop_name
 *
 * Fetch the property name in the config files.
 */
gchar *prop_name(prop_set_t *ps, property_t prop)
{
	return PROP(ps,prop).name;
}

/*
 * prop_to_string:
 *
 * Helper function for update_label() and update_entry()
 */
gchar *prop_to_string(prop_set_t *ps, property_t prop)
{
	static gchar s[4096];

	g_assert(ps != NULL);

	if (!prop_in_range(ps, prop))
		g_error("prop_get_gchar: unknown property %d", prop);

	switch (PROP(ps,prop).type) {
		case PROP_TYPE_GUINT32: {
			guint32 val;

			prop_get_guint32(ps, prop, &val, 0, 1);

			gm_snprintf(s, sizeof(s), "%u", val);
			break;
		}
		case PROP_TYPE_GUINT64: {
			guint64 val;

			prop_get_guint64(ps, prop, &val, 0, 1);

			gm_snprintf(s, sizeof(s), "%" PRIu64, val);
			break;
		}
		case PROP_TYPE_STRING: {
			gchar *buf = prop_get_string(ps, prop, NULL, 0);
			g_strlcpy(s, buf, sizeof(s));
			G_FREE_NULL(buf);
			break;
		}
		case PROP_TYPE_IP: {
			guint32 val;

			prop_get_guint32(ps, prop, &val, 0, 1);

			g_strlcpy(s, ip_to_gchar(val), sizeof(s));
			break;
		}
		case PROP_TYPE_BOOLEAN: {
			gboolean val;

			prop_get_boolean(ps, prop, &val, 0, 1);

			g_strlcpy(s, val ? "TRUE" : "FALSE", sizeof(s));
			break;
		}
		case PROP_TYPE_MULTICHOICE: {
			guint n = 0;

			while (
				(PROP(ps, prop).data.guint32.choices[n].title != NULL) &&
				(PROP(ps, prop).data.guint32.choices[n].value !=
					*(PROP(ps, prop).data.guint32.value))
				)
				n++;

			if (PROP(ps, prop).data.guint32.choices[n].title != NULL)
				gm_snprintf(s, sizeof(s), "%d: %s",
					*(PROP(ps, prop).data.guint32.value),
					PROP(ps,prop).data.guint32.choices[n].title);
			else
				gm_snprintf(s, sizeof(s),
					"%d: No descriptive string found for this value",
					*(PROP(ps, prop).data.guint32.value));
			break;
		}
		case PROP_TYPE_STORAGE: {
			g_strlcpy(s, guid_hex_str(prop_get_storage(ps, prop, NULL,
							PROP(ps,prop).vector_size)), sizeof(s));
			break;
		}
		default:
			s[0] = '\0';
			g_error("update_entry_gnet: incompatible type %s",
				prop_type_str[PROP(ps,prop).type]);
	}

	return s;
}


/*
 * config_boolean:
 *
 * Returns "TRUE" or "FALSE" depending on the given boolean value.
 */
static const gchar *config_boolean(gboolean b)
{
	static const gchar *b_true = "TRUE";
	static const gchar *b_false = "FALSE";
	return b ? b_true : b_false;
}

/*
 * config_comment:
 *
 * Creates a string containing aset of lines from with words taken from s,
 * each line no longer than 80 chars (except when a single words is very long)
 * and prepended with "# ".
 */
static gchar *config_comment(const gchar *s)
{
	gchar **sv;
	gchar *tok;
	gint n;
	gint len = 0;		 /* length without "# " at the beginning */
	GString *out;
	static gchar result[2048];

	g_assert(s != NULL);

	out = g_string_new("# ");
	sv = g_strsplit(s, " ", 0);

	for (tok = sv[0], n = 0; tok != NULL; ++n, tok = sv[n]) {
		if ((len == 0) || ((len + strlen(sv[n]) < 72))) {
			/* append to this line */
			if (n > 0)
				g_string_append_c(out, ' ');
			g_string_append(out, sv[n]);
			len += strlen(sv[n]) + (n == 0 ? 0 : 1);
		} else {
			/* end line and append to new line */
			g_string_append(out, "\n# ");
			g_string_append(out, sv[n]);
			len = strlen(sv[n]);
		}
	}

	g_strlcpy(result, out->str, sizeof(result));
	g_strfreev(sv);

	g_string_free(out, TRUE);

	return result;
}

/*
 * prop_save_to_file_if_dirty
 *
 * Like prop_save_to_file(), but only perform when dirty, i.e. when at least
 * one persisted property changed since the last time we saved.
 */
void prop_save_to_file_if_dirty(
	prop_set_t *ps, const gchar *dir, const gchar *filename)
{
	if (!ps->dirty)
		return;

	prop_save_to_file(ps, dir, filename);
}

/*
 * prop_save_to_file
 *
 * Read the all properties from the given property set and stores them
 * along with thier description to the given file in the given directory.
 * If this file was modified since the property set was read from it at
 * startup, the modifies file will be renamed to [filename].old before
 * saving.
 */
void prop_save_to_file(
	prop_set_t *ps, const gchar *dir, const gchar *filename)
{
	FILE *config;
	time_t mtime = 0;
	struct stat buf;
	gchar *newfile;
	gchar *pathname;
	guint n;

	g_assert(filename != NULL);
	g_assert(ps != NULL);

	if (debug >= 2)
		printf("saving %s to %s%s%s\n", ps->name,
			dir, G_DIR_SEPARATOR_S, filename);

	if (!is_directory(dir))
		return;

	pathname = make_pathname(dir, filename);
	g_return_if_fail(NULL != pathname);

	if (-1 == stat(pathname, &buf))
		g_warning("could not stat \"%s\": %s", pathname, g_strerror(errno));
	else
		mtime = buf.st_mtime;

	/*
	 * Rename old config file if they changed it whilst we were running.
	 */

	if (ps->mtime && mtime > ps->mtime) {
		gchar *old = g_strconcat(pathname, ".old", NULL);
		g_warning("config file \"%s\" changed whilst I was running", pathname);
		if (-1 == rename(pathname, old))
			g_warning("unable to rename as \"%s\": %s", old, g_strerror(errno));
		else
			g_warning("renamed old copy as \"%s\"", old);
		G_FREE_NULL(old);
	}

	/*
	 * Create new file, which will be renamed at the end, so we don't
	 * clobber a good configuration file should we fail abruptly.
	 */

	newfile = g_strconcat(pathname, ".new", NULL);
	config = file_fopen(newfile, "w");

	if (config == NULL)
		goto end;

	fprintf(config,
			"#\n# Gtk-Gnutella %s%s (%s) by Olrick & Co.\n# %s\n#\n",
			GTA_VERSION_NUMBER,
#ifdef GTA_REVISION
			" " GTA_REVISION,
#else
			"",
#endif
			GTA_RELEASE, GTA_WEBSITE);

	fprintf(config,
		"#\n# Description of contents\n"
		"%s\n\n",
		config_comment(ps->desc));

	for (n = 0; n < ps->size; n++) {
		prop_def_t *p = &ps->props[n];
		gchar **vbuf;
		guint i;
		gchar sbuf[1024];
		gchar *val = NULL;
		gboolean quotes = FALSE;
		gboolean defaultvalue = TRUE;

		if (p->save == FALSE)
			continue;

		vbuf = g_new(gchar*, p->vector_size+1);
		vbuf[0] = NULL;

		fprintf(config, "%s\n", config_comment(p->desc));

		switch (p->type) {
		case PROP_TYPE_BOOLEAN:
			for (i = 0; i < p->vector_size; i++) {
				g_strlcpy(sbuf, config_boolean(p->data.boolean.value[i]),
					sizeof(sbuf));
				if (p->data.boolean.value[i] != p->data.boolean.def[i])
					defaultvalue = FALSE;
				vbuf[i] = g_strdup(sbuf);
			}
			vbuf[p->vector_size] = NULL;

			val = g_strjoinv(",", vbuf);
			break;
		case PROP_TYPE_MULTICHOICE:
		case PROP_TYPE_GUINT32:
			for (i = 0; i < p->vector_size; i++) {
				gm_snprintf(sbuf, sizeof(sbuf), "%u",
						p->data.guint32.value[i]);
				if (p->data.guint32.value[i] != p->data.guint32.def[i])
					defaultvalue = FALSE;
				vbuf[i] = g_strdup(sbuf);
			}
			vbuf[p->vector_size] = NULL;

			val = g_strjoinv(",", vbuf);
			break;
		case PROP_TYPE_GUINT64:
			for (i = 0; i < p->vector_size; i++) {
				gm_snprintf(sbuf, sizeof(sbuf), "%" PRIu64,
						p->data.guint64.value[i]);
				if (p->data.guint64.value[i] != p->data.guint64.def[i])
					defaultvalue = FALSE;
				vbuf[i] = g_strdup(sbuf);
			}
			vbuf[p->vector_size] = NULL;

			val = g_strjoinv(",", vbuf);
			break;
		case PROP_TYPE_STRING:
			val = g_strdup(*p->data.string.value);
			if (
				(val == NULL && *p->data.string.def != NULL) ||
				(val != NULL && *p->data.string.def == NULL) ||
				0 != strcmp(val, *p->data.string.def)
			)
				defaultvalue = FALSE;
			quotes = TRUE;
			break;
		case PROP_TYPE_IP:
			for (i = 0; i < p->vector_size; i++) {
				g_strlcpy(sbuf, ip_to_gchar(p->data.guint32.value[i]),
					sizeof(sbuf));
				if (p->data.guint32.value[i] != p->data.guint32.def[i])
					defaultvalue = FALSE;
				vbuf[i] = g_strdup(sbuf);
			}
			vbuf[p->vector_size] = NULL;

			val = g_strjoinv(",", vbuf);
			quotes = TRUE;
			break;
		case PROP_TYPE_STORAGE:
			val = g_malloc((p->vector_size * 2) + 1);

			for (i = 0; i < p->vector_size; i++) {
				guint8 c = p->data.storage.value[i];

				val[i * 2] = hex_alphabet_lower[c >> 4];
				val[i * 2 + 1] = hex_alphabet_lower[c & 0x0f];
			}

			val[i * 2] = '\0';
			quotes = TRUE;

			/* No default values for storage type properties. */
			defaultvalue = FALSE;
			break;
		};

		g_assert(val != NULL);

		fprintf(config, "%s%s = %s%s%s\n\n", defaultvalue ? "#" : "",
			p->name, quotes ? "\"" : "", val, quotes ? "\"" : "");

		G_FREE_NULL(val);
		g_strfreev(vbuf);
	}

	fprintf(config, "### End of configuration file ###\n");

	/*
	 * Rename saved configuration file on success.
	 */

	if (0 == fclose(config)) {
		ps->dirty = FALSE;
		if (-1 == rename(newfile, pathname))
			g_warning("could not rename %s as %s: %s",
				newfile, pathname, g_strerror(errno));
		ps->mtime = time(NULL);
	} else
		g_warning("could not flush %s: %s", newfile, g_strerror(errno));

end:
	G_FREE_NULL(newfile);
	G_FREE_NULL(pathname);
}

/*
 * load_helper:
 *
 * Called by prop_load_from_file to actually set the properties.
 */
static void
load_helper(prop_set_t *ps, property_t prop, const gchar *val)
{
	prop_def_t *p = &PROP(ps,prop);
	prop_set_stub_t *stub;
	static union {
		gboolean	boolean[100];
		guint32		uint32[100];
		guint64		uint64[100];
	} vecbuf;

	if (!p->save) {
		g_warning("Refusing to load run-time only property \"%s\"", p->name);
		return;
	}

	stub = ps->get_stub();

	switch (p->type) {
	case PROP_TYPE_BOOLEAN:
		prop_assert(ps, prop,
			p->vector_size * sizeof(gboolean) < sizeof(vecbuf.boolean));

		/* Initialize vector with defaults */
		stub->boolean.get(prop, vecbuf.boolean, 0, 0);
		prop_parse_boolean_vector(p->name, val, p->vector_size, vecbuf.boolean);
		stub->boolean.set(prop, vecbuf.boolean, 0, 0);
		break;
	case PROP_TYPE_MULTICHOICE:
	case PROP_TYPE_GUINT32:
		prop_assert(ps, prop,
			p->vector_size * sizeof(guint32) < sizeof(vecbuf.uint32));

		/* Initialize vector with defaults */
		stub->guint32.get(prop, vecbuf.uint32, 0, 0);
		prop_parse_guint32_vector(p->name, val, p->vector_size, vecbuf.uint32);
		stub->guint32.set(prop, vecbuf.uint32, 0, 0);
		break;
	case PROP_TYPE_GUINT64:
		prop_assert(ps, prop,
			p->vector_size * sizeof(guint64) < sizeof(vecbuf.uint64));

		/* Initialize vector with defaults */
		stub->guint64.get(prop, vecbuf.uint64, 0, 0);
		prop_parse_guint64_vector(p->name, val, p->vector_size, vecbuf.uint64);
		stub->guint64.set(prop, vecbuf.uint64, 0, 0);
		break;
	case PROP_TYPE_STRING:
		stub->string.set(prop, val);
		break;
	case PROP_TYPE_IP:
		prop_assert(ps, prop,
			p->vector_size * sizeof(guint32) < sizeof(vecbuf.uint32));

		/* Initialize vector with defaults */
		stub->guint32.get(prop, vecbuf.uint32, 0, 0);
		prop_parse_ip_vector(p->name, val, p->vector_size, vecbuf.uint32);
		stub->guint32.set(prop, vecbuf.uint32, 0, 0);
		break;
	case PROP_TYPE_STORAGE:
		{
			gchar s[1024];
			gchar *d, *buf;

			if (p->vector_size > sizeof s) {
				d = g_malloc(p->vector_size);
				buf = d;
			} else {
				d = NULL;
				buf = s;
			}
			prop_parse_storage(p->name, val, p->vector_size, buf);
			stub->storage.set(prop, buf, p->vector_size);

			G_FREE_NULL(d);
		}
		break;
	}

	G_FREE_NULL(stub);
}

void prop_load_from_file(
	prop_set_t *ps, const gchar *dir, const gchar *filename)
{
	static const char fmt[] = "Bad line %u in config file, ignored";
	FILE *config;
	gchar *path;
	guint32 n = 0;
	struct stat buf;

	g_assert(dir != NULL);
	g_assert(filename != NULL);
	g_assert(ps != NULL);

	if (!is_directory(dir))
		return;

	path = make_pathname(dir, filename);
	g_return_if_fail(NULL != path);

	config = file_fopen(path, "r");
	if (!config) {
		G_FREE_NULL(path);
		return;
	}

	if (-1 == fstat(fileno(config), &buf))
		g_warning("could open but not fstat \"%s\" (fd #%d): %s",
			path, fileno(config), g_strerror(errno));
	else
		ps->mtime = buf.st_mtime;

	G_FREE_NULL(path);

	/*
	 * Lines should match the following expression:
	 *
	 * ^<keyword>=<value>
	 *
	 * whereas:
	 *
	 * <keyword> matches
	 *
	 * ([[:blank:]]*)[[:alpha:]](([[:alnum:]]|_)*)([[:blank:]]*)
	 *
	 * and <value> matches
	 *
	 * ([[:blank:]]*)(("[^"]*")|([^[:space:]]*))
	 *
	 */
	while (fgets(prop_tmp, sizeof(prop_tmp), config)) {
		property_t i;
		gchar *s, *k, *v;
		gint c;

		n++; /* Increase line counter */
		k = v = NULL;
		s = prop_tmp;
		/* Skip leading blanks */
		while ((c = (guchar) *s) != '\0' && is_ascii_blank(c))
			s++;

		if (!is_ascii_alpha(c))	/* <keyword> must start with a letter */
			continue;

		/* Here starts the <keyword> */
		k = s;
		while ((c = (guchar) *s) == '_' || is_ascii_alnum(c))
			s++;
		*s = '\0'; /* Terminate <keyword>, original value is stored in c */

		if (c != '=' && !is_ascii_blank(c)) {
			/* <keyword> must be followed by a '=' and optional blanks */
			g_warning(fmt, n);
			continue;
		}
		while (c != '\0' && is_ascii_blank(c))
			c = *(++s);

		if (c != '=') {
			/* <keyword> must be followed by a '=' and optionally blanks */
			g_warning(fmt, n);
			continue;
		}

		g_assert(c == '=' && (*s == '\0' || *s == '='));
		s++; /* Skip '=' (maybe already overwritten with a '\0') */

		/* Skip optional blanks */
		while ((c = (guchar) *s) != '\0' && is_ascii_blank(c))
			s++;

		if (c == '"') {
			/* Here starts the <value> part (quoted) */
			v = ++s; /* Skip double-quote '"' */

			/* Scan for terminating double-quote '"' */
			while ((c = (guchar) *s) != '\0' && c != '\n' && c != '"')
				s++;

			/* Check for proper quote termination */
			if (c == '\0' || c == '\n') {
				/* Missing terminating double-quote '"' */
				g_warning(fmt, n);
				continue;
			}
			g_assert(*s == '"');
		} else {
			/* Here starts the <value> part (unquoted) */
			v = s;
			/* The first space terminates the value */
			while ((c = (guchar) *s) != '\0' && !is_ascii_space(c))
				s++;
		}

		g_assert(*s == '\0' || *s == '"' || is_ascii_space(c));
		*s = '\0'; /* Terminate value in case of trailing characters */

		if (common_dbg > 5)
			g_message("k=\"%s\", v=\"%s\"", k, v);

		for (i = 0; i < ps->size; i++)
			if (!g_ascii_strcasecmp(k, (ps->props[i]).name)) {
				load_helper(ps, i + ps->offset, v);
				break;
			}

		if (i >= ps->size)
			g_warning("config file, line %u: unknown keyword '%s', ignored",
					(guint) n, k);
	}

	fclose(config);
}

inline property_t prop_get_by_name(prop_set_t *ps, const gchar *name)
{
	g_assert(ps != NULL);

	return GPOINTER_TO_UINT(g_hash_table_lookup(ps->byName, name));
}

/* vi: set ts=4: */
