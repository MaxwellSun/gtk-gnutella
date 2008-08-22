/*
 * $Id$
 *
 * Copyright (c) 2008, Raphael Manfredi
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
 * Gnutella DHT interface.
 *
 * @author Raphael Manfredi
 * @date 2008
 */

#include "common.h"

RCSID("$Id$")

#include "gdht.h"
#include "fileinfo.h"
#include "extensions.h"
#include "ggep_type.h"
#include "hosts.h"
#include "hostiles.h"
#include "downloads.h"

#include "if/core/guid.h"
#include "if/dht/lookup.h"
#include "if/gnet_property_priv.h"

#include "lib/atoms.h"
#include "lib/endian.h"
#include "lib/walloc.h"
#include "lib/override.h"		/* Must be the last header included */

/**
 * Hash table holding all the pending SHA1 lookups by KUID.
 */
static GHashTable *sha1_lookups;	/* KUID -> struct sha1_lookup * */

/**
 * Convert a SHA1 to the proper Kademlia key for lookups.
 *
 * @return KUID atom for SHA1 lookup
 */
static kuid_t *
kuid_from_sha1(const sha1_t *sha1)
{
	return kuid_get_atom((const kuid_t *) sha1);	/* Identity */
}

typedef enum {
	SHA1_LOOKUP_MAGIC = 0x5fd660bfU
} slk_magic_t;

/**
 * Context for SHA1 ALOC lookups.
 *
 * The fi_guid field is a unique fileinfo identifier from which we can
 * safely retrieve the fileinfo, if still present.
 */
struct sha1_lookup {
	slk_magic_t magic;
	kuid_t *id;				/**< ID being looked for (atom) */
	guid_t *fi_guid;		/**< GUID of the fileinfo being searched (atom) */
};

static inline void
sha1_lookup_check(const struct sha1_lookup *slk)
{
	g_assert(slk);
	g_assert(SHA1_LOOKUP_MAGIC == slk->magic);
}

/*
 * Get human-readable DHT value type and version.
 * @return pointer to static data
 */
static const char *
value_infostr(const lookup_val_rc_t *rc)
{
	static char info[60];

	gm_snprintf(info, sizeof info, "DHT %s v%u.%u (%lu byte%s)",
		dht_value_type_to_string(rc->type), rc->major, rc->minor,
		(gulong) rc->length, 1 == rc->length ? "" : "s");

	return info;
}

/**
 * Free SHA1 lookup context.
 */
static void
gdht_free_sha1_lookup(struct sha1_lookup *slk, gboolean do_remove)
{
	sha1_lookup_check(slk);

	if (do_remove)
		g_hash_table_remove(sha1_lookups, slk->id);

	kuid_atom_free(slk->id);
	atom_guid_free((char *) slk->fi_guid);
	wfree(slk, sizeof *slk);
}

/**
 * Callback when SHA1 lookup is unsuccessful.
 */
static void
gdht_sha1_not_found(const kuid_t *kuid, lookup_error_t error, gpointer arg)
{
	struct sha1_lookup *slk = arg;

	sha1_lookup_check(slk);
	g_assert(slk->id == kuid);		/* They are atoms */

	if (GNET_PROPERTY(dht_lookup_debug) > 1)
		g_message("DHT ALOC lookup for %s failed: %s",
			kuid_to_string(kuid), lookup_strerror(error));

	gdht_free_sha1_lookup(slk, TRUE);
}

/**
 * Handle DHT ALOC value received to generate a new alt-loc source for
 * the file.
 */
static void
gdht_handle_aloc(const lookup_val_rc_t *rc, const fileinfo_t *fi)
{
	extvec_t exv[MAX_EXTVEC];
	int exvcnt;
	int i;
	gboolean firewalled = FALSE;
	guid_t guid;
	guint16 port = 0;
	gboolean tls = FALSE;
	filesize_t filesize = 0;
	guint32 flags = 0;

	g_assert(DHT_VT_ALOC == rc->type);

	ext_prepare(exv, MAX_EXTVEC);
	memset(guid.v, 0, GUID_RAW_SIZE);

	exvcnt = ext_parse(rc->data, rc->length, exv, MAX_EXTVEC);

	for (i = 0; i < exvcnt; i++) {
		extvec_t *e = &exv[i];
		guint16 paylen;

		switch (e->ext_token) {
		case EXT_T_GGEP_client_id:
			if (GUID_RAW_SIZE == ext_paylen(e))
				memcpy(guid.v, ext_payload(e), GUID_RAW_SIZE);
			break;
		case EXT_T_GGEP_firewalled:
			if (1 == ext_paylen(e)) {
				guint8 fw = peek_u8(ext_payload(e));
				firewalled = fw != 0;
			}
			break;
		case EXT_T_GGEP_length:
			{
				guint64 fs;
				ggept_status_t ret;

				ret = ggept_filesize_extract(e, &fs);
				if (GGEP_OK == ret)
					filesize = fs;
			}
			break;
		case EXT_T_GGEP_port:
			if (2 == ext_paylen(e))
				port = peek_be16(ext_payload(e));
			break;
		case EXT_T_GGEP_tls:
			tls = TRUE;
			break;
		case EXT_T_GGEP_ttroot:
			/* XXX ignored for now */
			break;
		default:
			if (GNET_PROPERTY(ggep_debug) > 1 && e->ext_type == EXT_GGEP) {
				paylen = ext_paylen(e);
				g_warning("%s: unhandled GGEP \"%s\" (%d byte%s)",
					value_infostr(rc),
					ext_ggep_id_str(e), paylen, paylen == 1 ? "" : "s");
			}
			break;
		}
	}

	/*
	 * Check servent's port, if specified.  It should match that of the
	 * creator.  If not, warn, but trust what is in the ALOC.
	 */

	if (port) {
		if (port != rc->port && GNET_PROPERTY(download_debug))
			g_warning("%s: port mismatch: creator's was %u, ALOC is %u for %s",
				value_infostr(rc), rc->port, port, fi->pathname);
	} else {
		port = rc->port;
	}

	/*
	 * Rule out invalid addresses if not firewalled.
	 */

	if (!firewalled && !host_is_valid(rc->addr, port)) {
		if (GNET_PROPERTY(download_debug))
			g_warning("discarding %s from %s for %s: invalid IP:port",
				value_infostr(rc), host_addr_port_to_string(rc->addr, port),
				fi->pathname);
		return;
	}

	/*
	 * Discard hostile sources.
	 */

	if (hostiles_check(rc->addr)) {
		if (GNET_PROPERTY(download_debug))
			g_warning("discarding %s from %s for %s: hostile IP",
				value_infostr(rc), host_addr_port_to_string(rc->addr, port),
				fi->pathname);
		return;
	}

	/*
	 * Check that filesize matches.
	 */

	if (filesize && fi->size != filesize) {
		if (GNET_PROPERTY(download_debug))
			g_warning("discarding %s from %s for %s: "
				"we have size=%lu, ALOC says %lu",
				value_infostr(rc), host_addr_port_to_string(rc->addr, port),
				fi->pathname, (gulong) fi->size, (gulong) filesize);
	}

	/*
	 * Create a new download.
	 */

	if (GNET_PROPERTY(download_debug) > 1)
		g_message("adding %s%ssource %s from DHT ALOC for %s",
			firewalled ? "firewalled " : "", tls ? "TLS " : "",
			host_addr_port_to_string(rc->addr, port), fi->pathname);

	if (firewalled)
		flags |= SOCK_F_PUSH;
	else if (tls)
		flags |= SOCK_F_TLS;

	download_auto_new(filepath_basename(fi->pathname),
		fi->size,
		rc->addr, port,
		(char *) guid.v,
		NULL,		/* hostname */
		fi->sha1,
		NULL,		/* tth */
		tm_time(),
		deconstify_gpointer(fi),
		NULL,		/* proxies */
		flags);
}

/**
 * Callback when SHA1 lookup is successful.
 */
static void
gdht_sha1_found(const kuid_t *kuid, const lookup_val_rs_t *rs, gpointer arg)
{
	struct sha1_lookup *slk = arg;
	fileinfo_t *fi;
	size_t i;

	g_assert(rs);
	sha1_lookup_check(slk);
	g_assert(slk->id == kuid);		/* They are atoms */

	if (GNET_PROPERTY(dht_lookup_debug) > 1)
		g_message("DHT ALOC lookup for %s returned %lu value%s",
			kuid_to_string(kuid), (gulong) rs->count,
			1 == rs->count ? "" : "s");

	fi = file_info_by_guid(slk->fi_guid);

	if (NULL == fi) {
		if (GNET_PROPERTY(dht_lookup_debug))
			g_warning("fileinfo for %s was removed whilst looking for ALOC",
				kuid_to_string(kuid));
		goto cleanup;
	}

	/*
	 * Parse ALOC results.
	 *
	 * The lookup code is supposed to have filtered out non-ALOC results
	 * since we explicitly demanded this type of results.  Hence warn if
	 * we get something else.
	 */

	for (i = 0; i < rs->count; i++) {
		lookup_val_rc_t *rc = &rs->records[i];

		if (rc->type != DHT_VT_ALOC) {
			g_warning("skipping non-ALOC record (%s) at %s",
				dht_value_type_to_string(rc->type), _WHERE_);
			continue;
		}

		gdht_handle_aloc(rc, fi);
	}

cleanup:
	lookup_free_value_results(rs);
	gdht_free_sha1_lookup(slk, TRUE);
}

/**
 * Launch a SHA1 lookup in the DHT to collect alternate locations.
 */
void
gdht_find_sha1(const fileinfo_t *fi)
{
	struct sha1_lookup *slk;

	file_info_check(fi);
	g_assert(fi->sha1);

	slk = walloc(sizeof *slk);
	slk->magic = SHA1_LOOKUP_MAGIC;
	slk->id = kuid_from_sha1(fi->sha1);
	slk->fi_guid = (guid_t *) atom_guid_get(fi->guid);

	/*
	 * If we have so many queued searches that we did not manage to get
	 * a previous one completed before it is re-attempted, ignore the new
	 * request.
	 */

	if (g_hash_table_lookup(sha1_lookups, slk->id)) {
		if (GNET_PROPERTY(dht_lookup_debug))
			g_warning("DHT already has pending search for %s: SHA1 of %s (%s)",
				kuid_to_hex_string(slk->id),
				fi->pathname, kuid_to_string(slk->id));

		gdht_free_sha1_lookup(slk, FALSE);
		return;
	}

	if (GNET_PROPERTY(dht_lookup_debug))
		g_message("DHT will be searching ALOC for %s: SHA1 of %s (%s)",
			kuid_to_hex_string(slk->id), fi->pathname, kuid_to_string(slk->id));

	g_hash_table_insert(sha1_lookups, slk->id, slk);
	ulq_find_value(slk->id, DHT_VT_ALOC, 
		gdht_sha1_found, gdht_sha1_not_found, slk);
}

/**
 * Initialize the Gnutella DHT layer.
 */
void
gdht_init(void)
{
	sha1_lookups = g_hash_table_new(sha1_hash, sha1_eq);
}

/**
 * Hash table iterator to free a struct sha1_lookup
 */
static void
free_sha1_lookups_kv(gpointer unused_key, gpointer val, gpointer unused_x)
{
	struct sha1_lookup *slk = val;

	(void) unused_key;
	(void) unused_x;

	gdht_free_sha1_lookup(slk, FALSE);
}

/**
 * Shutdown the Gnutella DHT layer.
 */
void
gdht_close(void)
{
	g_hash_table_foreach(sha1_lookups, free_sha1_lookups_kv, NULL);
	g_hash_table_destroy(sha1_lookups);
}

/* vi: set ts=4 sw=4 cindent: */
