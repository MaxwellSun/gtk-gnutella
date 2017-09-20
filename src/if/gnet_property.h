/*
 * Copyright (c) 2001-2003, Richard Eckart
 *
 * THIS FILE IS AUTOGENERATED! DO NOT EDIT!
 * This file is generated from gnet_props.ag using autogen.
 * Autogen is available at http://autogen.sourceforge.net/.
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

#ifndef _gnet_property_h_
#define _gnet_property_h_

#include "lib/prop.h"
#include "lib/log.h"		/* For s_carp() */

#define GNET_PROPERTY_MIN ((NO_PROP+1))
#define GNET_PROPERTY_MAX ((NO_PROP+1)+GNET_PROPERTY_END-1)
#define GNET_PROPERTY_NUM (GNET_PROPERTY_END-(NO_PROP+1))

typedef enum {
    PROP_READING_HOSTFILE=(NO_PROP+1),
    PROP_READING_ULTRAFILE,
    PROP_ANCIENT_VERSION,
    PROP_NEW_VERSION_STR,
    PROP_UP_CONNECTIONS,
    PROP_NORMAL_CONNECTIONS,
    PROP_MAX_CONNECTIONS,
    PROP_NODE_LEAF_COUNT,
    PROP_NODE_NORMAL_COUNT,
    PROP_NODE_ULTRA_COUNT,
    PROP_MAX_DOWNLOADS,
    PROP_MAX_HOST_DOWNLOADS,
    PROP_MAX_UPLOADS,
    PROP_MAX_UPLOADS_IP,
    PROP_LOCAL_IP,
    PROP_LOCAL_IP6,
    PROP_CURRENT_IP_STAMP,
    PROP_CURRENT_IP6_STAMP,
    PROP_AVERAGE_IP_UPTIME,
    PROP_AVERAGE_IP6_UPTIME,
    PROP_START_STAMP,
    PROP_AVERAGE_SERVENT_UPTIME,
    PROP_LISTEN_PORT,
    PROP_FORCED_LOCAL_IP,
    PROP_FORCED_LOCAL_IP6,
    PROP_CONNECTION_SPEED,
    PROP_COMPUTE_CONNECTION_SPEED,
    PROP_QUERY_RESPONSE_MAX_ITEMS,
    PROP_UL_USAGE_MIN_PERCENTAGE,
    PROP_DOWNLOAD_CONNECTING_TIMEOUT,
    PROP_DOWNLOAD_PUSH_SENT_TIMEOUT,
    PROP_DOWNLOAD_CONNECTED_TIMEOUT,
    PROP_DOWNLOAD_RETRY_TIMEOUT_MIN,
    PROP_DOWNLOAD_RETRY_TIMEOUT_MAX,
    PROP_DOWNLOAD_MAX_RETRIES,
    PROP_DOWNLOAD_RETRY_TIMEOUT_DELAY,
    PROP_DOWNLOAD_RETRY_BUSY_DELAY,
    PROP_DOWNLOAD_RETRY_REFUSED_DELAY,
    PROP_DOWNLOAD_RETRY_STOPPED_DELAY,
    PROP_DOWNLOAD_OVERLAP_RANGE,
    PROP_UPLOAD_CONNECTING_TIMEOUT,
    PROP_UPLOAD_CONNECTED_TIMEOUT,
    PROP_SEARCH_REISSUE_TIMEOUT,
    PROP_BAN_RATIO_FDS,
    PROP_BAN_MAX_FDS,
    PROP_BANNED_COUNT,
    PROP_MAX_BANNED_FD,
    PROP_INCOMING_CONNECTING_TIMEOUT,
    PROP_NODE_CONNECTING_TIMEOUT,
    PROP_NODE_CONNECTED_TIMEOUT,
    PROP_NODE_SENDQUEUE_SIZE,
    PROP_NODE_TX_FLOWC_TIMEOUT,
    PROP_NODE_RX_FLOWC_RATIO,
    PROP_MAX_TTL,
    PROP_MY_TTL,
    PROP_HARD_TTL_LIMIT,
    PROP_DBG,
    PROP_BAN_DEBUG,
    PROP_CPU_DEBUG,
    PROP_DMESH_DEBUG,
    PROP_GMSG_DEBUG,
    PROP_SHARE_DEBUG,
    PROP_NODE_DEBUG,
    PROP_SOCKET_DEBUG,
    PROP_BOOTSTRAP_DEBUG,
    PROP_HTTP_DEBUG,
    PROP_DOWNLOAD_DEBUG,
    PROP_FILEINFO_DEBUG,
    PROP_UPLOAD_DEBUG,
    PROP_LIB_DEBUG,
    PROP_URL_DEBUG,
    PROP_DH_DEBUG,
    PROP_DQ_DEBUG,
    PROP_VMSG_DEBUG,
    PROP_QUERY_DEBUG,
    PROP_SEARCH_DEBUG,
    PROP_UDP_DEBUG,
    PROP_QRP_DEBUG,
    PROP_ROUTING_DEBUG,
    PROP_GGEP_DEBUG,
    PROP_PCACHE_DEBUG,
    PROP_HSEP_DEBUG,
    PROP_TLS_DEBUG,
    PROP_PARQ_DEBUG,
    PROP_PARQ_OPTIMISTIC,
    PROP_PARQ_SIZE_ALWAYS_CONTINUE,
    PROP_PARQ_TIME_ALWAYS_CONTINUE,
    PROP_PARQ_BAN_BAD_MAXCOUNTWAIT,
    PROP_NET_BUFFER_SHORTAGE,
    PROP_STOP_HOST_GET,
    PROP_BW_HTTP_IN_ENABLED,
    PROP_BW_HTTP_OUT_ENABLED,
    PROP_BW_GNET_IN_ENABLED,
    PROP_BW_GNET_LEAF_IN_ENABLED,
    PROP_BW_GNET_OUT_ENABLED,
    PROP_BW_GNET_LEAF_OUT_ENABLED,
    PROP_BW_UL_USAGE_ENABLED,
    PROP_BW_ALLOW_STEALING,
    PROP_AUTOCLEAR_COMPLETED_DOWNLOADS,
    PROP_AUTOCLEAR_FAILED_DOWNLOADS,
    PROP_AUTOCLEAR_UNAVAILABLE_DOWNLOADS,
    PROP_AUTOCLEAR_FINISHED_DOWNLOADS,
    PROP_SEARCH_REMOVE_DOWNLOADED,
    PROP_FORCE_LOCAL_IP,
    PROP_FORCE_LOCAL_IP6,
    PROP_BIND_TO_FORCED_LOCAL_IP,
    PROP_BIND_TO_FORCED_LOCAL_IP6,
    PROP_USE_NETMASKS,
    PROP_ALLOW_PRIVATE_NETWORK_CONNECTION,
    PROP_USE_IP_TOS,
    PROP_DOWNLOAD_DELETE_ABORTED,
    PROP_PROXY_AUTH,
    PROP_SOCKS_USER,
    PROP_SOCKS_PASS,
    PROP_PROXY_ADDR,
    PROP_PROXY_HOSTNAME,
    PROP_PROXY_PORT,
    PROP_PROXY_PROTOCOL,
    PROP_NETWORK_PROTOCOL,
    PROP_USE_IPV6_TRT,
    PROP_IPV6_TRT_PREFIX,
    PROP_HOSTS_IN_CATCHER,
    PROP_HOSTS_IN_ULTRA_CATCHER,
    PROP_HOSTS_IN_BAD_CATCHER,
    PROP_MAX_HOSTS_CACHED,
    PROP_MAX_ULTRA_HOSTS_CACHED,
    PROP_MAX_BAD_HOSTS_CACHED,
    PROP_MAX_HIGH_TTL_MSG,
    PROP_MAX_HIGH_TTL_RADIUS,
    PROP_BW_HTTP_IN,
    PROP_BW_HTTP_OUT,
    PROP_BW_GNET_IN,
    PROP_BW_GNET_OUT,
    PROP_BW_GNET_LIN,
    PROP_BW_GNET_LOUT,
    PROP_SEARCH_QUERIES_FORWARD_SIZE,
    PROP_SEARCH_QUERIES_KICK_SIZE,
    PROP_SEARCH_ANSWERS_FORWARD_SIZE,
    PROP_SEARCH_ANSWERS_KICK_SIZE,
    PROP_SEARCH_MUID_TRACK_AMOUNT,
    PROP_OTHER_MESSAGES_KICK_SIZE,
    PROP_HOPS_RANDOM_FACTOR,
    PROP_SEND_PUSHES,
    PROP_MIN_DUP_MSG,
    PROP_MIN_DUP_RATIO,
    PROP_SCAN_EXTENSIONS,
    PROP_SCAN_IGNORE_SYMLINK_DIRS,
    PROP_SCAN_IGNORE_SYMLINK_REGFILES,
    PROP_SAVE_FILE_PATH,
    PROP_MOVE_FILE_PATH,
    PROP_BAD_FILE_PATH,
    PROP_SHARED_DIRS_PATHS,
    PROP_LOCAL_NETMASKS_STRING,
    PROP_TOTAL_DOWNLOADS,
    PROP_UL_RUNNING,
    PROP_UL_QUICK_RUNNING,
    PROP_UL_REGISTERED,
    PROP_TOTAL_UPLOADS,
    PROP_SERVENT_GUID,
    PROP_KUID,
    PROP_USE_SWARMING,
    PROP_USE_AGGRESSIVE_SWARMING,
    PROP_DL_MINCHUNKSIZE,
    PROP_DL_MAXCHUNKSIZE,
    PROP_AUTO_DOWNLOAD_IDENTICAL,
    PROP_AUTO_FEED_DOWNLOAD_MESH,
    PROP_STRICT_SHA1_MATCHING,
    PROP_IS_FIREWALLED,
    PROP_IS_INET_CONNECTED,
    PROP_IS_UDP_FIREWALLED,
    PROP_RECV_SOLICITED_UDP,
    PROP_GNET_COMPACT_QUERY,
    PROP_DOWNLOAD_OPTIMISTIC_START,
    PROP_LIBRARY_REBUILDING,
    PROP_SHA1_REBUILDING,
    PROP_SHA1_VERIFYING,
    PROP_FILE_MOVING,
    PROP_PREFER_COMPRESSED_GNET,
    PROP_ONLINE_MODE,
    PROP_DOWNLOAD_REQUIRE_URN,
    PROP_DOWNLOAD_REQUIRE_SERVER_NAME,
    PROP_MAX_ULTRAPEERS,
    PROP_QUICK_CONNECT_POOL_SIZE,
    PROP_MAX_LEAVES,
    PROP_SEARCH_HANDLE_IGNORED_FILES,
    PROP_CONFIGURED_PEERMODE,
    PROP_CURRENT_PEERMODE,
    PROP_SYS_NOFILE,
    PROP_SYS_PHYSMEM,
    PROP_DL_QUEUE_COUNT,
    PROP_DL_RUNNING_COUNT,
    PROP_DL_ACTIVE_COUNT,
    PROP_DL_AQUEUED_COUNT,
    PROP_DL_PQUEUED_COUNT,
    PROP_FI_ALL_COUNT,
    PROP_FI_WITH_SOURCE_COUNT,
    PROP_DL_QALIVE_COUNT,
    PROP_DL_BYTE_COUNT,
    PROP_UL_BYTE_COUNT,
    PROP_PFSP_SERVER,
    PROP_PFSP_FIRST_CHUNK,
    PROP_PFSP_MINIMUM_FILESIZE,
    PROP_CRAWLER_VISIT_COUNT,
    PROP_UDP_CRAWLER_VISIT_COUNT,
    PROP_HOST_RUNS_NTP,
    PROP_NTP_DETECTED,
    PROP_CLOCK_SKEW,
    PROP_NODE_MONITOR_UNSTABLE_IP,
    PROP_NODE_MONITOR_UNSTABLE_SERVENTS,
    PROP_DL_REMOVE_FILE_ON_MISMATCH,
    PROP_DL_MISMATCH_BACKOUT,
    PROP_SERVER_HOSTNAME,
    PROP_GIVE_SERVER_HOSTNAME,
    PROP_RESERVE_GTKG_NODES,
    PROP_UNIQUE_NODES,
    PROP_DOWNLOAD_RX_SIZE,
    PROP_NODE_RX_SIZE,
    PROP_UPLOAD_TX_SIZE,
    PROP_DL_HTTP_LATENCY,
    PROP_NODE_LAST_ULTRA_CHECK,
    PROP_NODE_LAST_ULTRA_LEAF_SWITCH,
    PROP_UP_REQ_AVG_SERVENT_UPTIME,
    PROP_UP_REQ_AVG_IP_UPTIME,
    PROP_UP_REQ_NODE_UPTIME,
    PROP_UP_REQ_NOT_FIREWALLED,
    PROP_UP_REQ_ENOUGH_CONN,
    PROP_UP_REQ_ENOUGH_FD,
    PROP_UP_REQ_ENOUGH_MEM,
    PROP_UP_REQ_ENOUGH_BW,
    PROP_UP_REQ_GOOD_UDP,
    PROP_SEARCH_QUEUE_SIZE,
    PROP_SEARCH_QUEUE_SPACING,
    PROP_ENABLE_SHELL,
    PROP_REMOTE_SHELL_TIMEOUT,
    PROP_ENTRY_REMOVAL_TIMEOUT,
    PROP_NODE_WATCH_SIMILAR_QUERIES,
    PROP_NODE_QUERIES_HALF_LIFE,
    PROP_NODE_REQUERY_THRESHOLD,
    PROP_LIBRARY_RESCAN_STARTED,
    PROP_LIBRARY_RESCAN_FINISHED,
    PROP_LIBRARY_RESCAN_DURATION,
    PROP_QRP_INDEXING_STARTED,
    PROP_QRP_INDEXING_DURATION,
    PROP_QRP_MEMORY,
    PROP_QRP_TIMESTAMP,
    PROP_QRP_COMPUTATION_TIME,
    PROP_QRP_PATCH_TIMESTAMP,
    PROP_QRP_PATCH_COMPUTATION_TIME,
    PROP_QRP_GENERATION,
    PROP_QRP_SLOTS,
    PROP_QRP_SLOTS_FILLED,
    PROP_QRP_FILL_RATIO,
    PROP_QRP_CONFLICT_RATIO,
    PROP_QRP_HASHED_KEYWORDS,
    PROP_QRP_PATCH_RAW_LENGTH,
    PROP_QRP_PATCH_LENGTH,
    PROP_QRP_PATCH_COMP_RATIO,
    PROP_ANCIENT_VERSION_LEFT_DAYS,
    PROP_FILE_DESCRIPTOR_SHORTAGE,
    PROP_FILE_DESCRIPTOR_RUNOUT,
    PROP_CONVERT_SPACES,
    PROP_CONVERT_EVIL_CHARS,
    PROP_CONVERT_OLD_FILENAMES,
    PROP_TLS_ENFORCE,
    PROP_GNET_DEFLATE_ENABLED,
    PROP_ENABLE_UDP,
    PROP_PROCESS_OOB_QUERIES,
    PROP_SEND_OOB_QUERIES,
    PROP_PROXY_OOB_QUERIES,
    PROP_UPLOADS_STALLING,
    PROP_USE_GLOBAL_HOSTILES_TXT,
    PROP_USE_SO_LINGER,
    PROP_TCP_DEFER_ACCEPT_TIMEOUT,
    PROP_BROWSE_HOST_ENABLED,
    PROP_HTML_BROWSE_COUNT,
    PROP_HTML_BROWSE_SERVED,
    PROP_QHITS_BROWSE_COUNT,
    PROP_QHITS_BROWSE_SERVED,
    PROP_OVERLOADED_CPU,
    PROP_DOWNLOAD_BUFFER_SIZE,
    PROP_BROWSE_COPIED_TO_PASSIVE,
    PROP_DISPLAY_METRIC_UNITS,
    PROP_RUDP_DEBUG,
    PROP_DHT_DEBUG,
    PROP_OOB_PROXY_DEBUG,
    PROP_ENABLE_LOCAL_SOCKET,
    PROP_MAX_SIMULTANEOUS_DOWNLOADS_PER_FILE,
    PROP_DUMP_RECEIVED_GNUTELLA_PACKETS,
    PROP_SEARCH_RESULTS_EXPOSE_RELATIVE_PATHS,
    PROP_SHELL_DEBUG,
    PROP_DOWNLOAD_QUEUE_FROZEN,
    PROP_SQ_DEBUG,
    PROP_PUSH_PROXY_DEBUG,
    PROP_THEX_FILES_REQUESTED,
    PROP_THEX_FILES_SERVED,
    PROP_TIGERTREE_DEBUG,
    PROP_TTH_REBUILDING,
    PROP_TTH_VERIFYING,
    PROP_SEARCH_MAX_RESULTS,
    PROP_BROWSE_HOST_MAX_RESULTS,
    PROP_LATEST_SVN_RELEASE_DATE,
    PROP_LATEST_SVN_RELEASE_REVISION,
    PROP_LATEST_SVN_RELEASE_SIGNATURE,
    PROP_TLS_CACHE_MAX_HOSTS,
    PROP_TLS_CACHE_MAX_TIME,
    PROP_TTH_AUTO_DISCOVERY,
    PROP_ENABLE_HACKAROUNDS,
    PROP_G2_CACHE_MAX_HOSTS,
    PROP_G2_CACHE_MAX_TIME,
    PROP_G2_DEBUG,
    PROP_DL_RESOURCE_SWITCHING,
    PROP_PARQ_ENABLED,
    PROP_PFSP_LAST_CHUNK,
    PROP_BEAUTIFY_FILENAMES,
    PROP_ENABLE_DHT,
    PROP_CQ_DEBUG,
    PROP_DHT_LOOKUP_DEBUG,
    PROP_DHT_STORAGE_DEBUG,
    PROP_DHT_PUBLISH_DEBUG,
    PROP_BW_DHT_LOOKUP_OUT,
    PROP_BW_DHT_LOOKUP_IN,
    PROP_STICKY_GUID,
    PROP_STICKY_KUID,
    PROP_DHT_ULQ_DEBUG,
    PROP_DHT_STORAGE_IN_MEMORY,
    PROP_DOWNLOAD_TRACE,
    PROP_UPLOAD_TRACE,
    PROP_GNET_TRACE,
    PROP_PUSH_PROXY_TRACE,
    PROP_HTTP_TRACE,
    PROP_VERIFY_DEBUG,
    PROP_LOCAL_ADDR_CACHE_MAX_HOSTS,
    PROP_LOCAL_ADDR_CACHE_MAX_TIME,
    PROP_LOCAL_ADDR_DEBUG,
    PROP_DUMP_TRANSMITTED_GNUTELLA_PACKETS,
    PROP_MQ_TCP_DEBUG,
    PROP_MQ_UDP_DEBUG,
    PROP_NODE_UDP_SENDQUEUE_SIZE,
    PROP_CLOCK_DEBUG,
    PROP_FW_DEBUG,
    PROP_HOST_DEBUG,
    PROP_DHT_ROOTS_DEBUG,
    PROP_LIB_STATS,
    PROP_SPAM_LUT_IN_MEMORY,
    PROP_SPAM_DEBUG,
    PROP_LOCKFILE_DEBUG,
    PROP_ZALLOC_DEBUG,
    PROP_PALLOC_DEBUG,
    PROP_RXBUF_DEBUG,
    PROP_ZALLOC_ALWAYS_GC,
    PROP_VMM_DEBUG,
    PROP_SHUTDOWN_DEBUG,
    PROP_COUNTRY_LIMITS,
    PROP_CTL_DEBUG,
    PROP_LOG_DROPPED_GNUTELLA,
    PROP_WHITELIST_DEBUG,
    PROP_DHT_TCACHE_DEBUG,
    PROP_PUBLISHER_DEBUG,
    PROP_DHT_TRACE,
    PROP_BW_DHT_OUT_ENABLED,
    PROP_BW_DHT_OUT,
    PROP_NODE_DHT_SENDQUEUE_SIZE,
    PROP_BSCHED_DEBUG,
    PROP_DHT_STABLE_DEBUG,
    PROP_RELOAD_DEBUG,
    PROP_MOVE_DEBUG,
    PROP_QHIT_DEBUG,
    PROP_VERSION_DEBUG,
    PROP_CPU_FREQ_MIN,
    PROP_CPU_FREQ_MAX,
    PROP_DHT_BOOT_STATUS,
    PROP_DHT_CONFIGURED_MODE,
    PROP_DHT_CURRENT_MODE,
    PROP_OMALLOC_DEBUG,
    PROP_HCACHE_DEBUG,
    PROP_RANDOMNESS,
    PROP_AVERAGE_SERVENT_DOWNTIME,
    PROP_SHUTDOWN_TIME,
    PROP_ALIVE_DEBUG,
    PROP_VXML_DEBUG,
    PROP_UPNP_DEBUG,
    PROP_SOAP_DEBUG,
    PROP_SOAP_TRACE,
    PROP_ALLOW_FIREWALLED_ULTRA,
    PROP_ENABLE_UPNP,
    PROP_UPNP_POSSIBLE,
    PROP_PORT_MAPPING_REQUIRED,
    PROP_PORT_MAPPING_POSSIBLE,
    PROP_NATPMP_DEBUG,
    PROP_ENABLE_NATPMP,
    PROP_NATPMP_POSSIBLE,
    PROP_TX_DEBUG,
    PROP_RX_DEBUG,
    PROP_INPUTEVT_DEBUG,
    PROP_BG_DEBUG,
    PROP_PORT_MAPPING_SUCCESSFUL,
    PROP_UPLOADS_BW_NO_STEALING,
    PROP_UPLOADS_BW_IGNORE_STOLEN,
    PROP_UPLOADS_BW_UNIFORM,
    PROP_ENABLE_HTTP_PIPELINING,
    PROP_DL_PIPELINE_MAXCHUNKSIZE,
    PROP_ENABLE_GUESS,
    PROP_GUESS_SERVER_DEBUG,
    PROP_GUESS_CLIENT_DEBUG,
    PROP_MAX_GUESS_HOSTS_CACHED,
    PROP_HOSTS_IN_GUESS_CATCHER,
    PROP_MAX_GUESS_INTRO_HOSTS_CACHED,
    PROP_HOSTS_IN_GUESS_INTRO_CATCHER,
    PROP_DBSTORE_DEBUG,
    PROP_SESSION_ID,
    PROP_PFSP_RARE_SERVER,
    PROP_ENABLE_GUESS_CLIENT,
    PROP_BW_GUESS_OUT,
    PROP_MATCHING_DEBUG,
    PROP_TSYNC_DEBUG,
    PROP_QUERY_REQUEST_PARTIALS,
    PROP_QUERY_ANSWER_PARTIALS,
    PROP_QUERY_ANSWER_WHATS_NEW,
    PROP_SEARCH_SMART_STOP,
    PROP_WHATS_NEW_SEARCH_MAX_RESULTS,
    PROP_PASSIVE_SEARCH_MAX_RESULTS,
    PROP_LOG_DUP_GNUTELLA_SAME_NODE,
    PROP_LOG_DUP_GNUTELLA_HIGHER_TTL,
    PROP_LOG_DUP_GNUTELLA_OTHER_NODE,
    PROP_LOG_NEW_GNUTELLA,
    PROP_LOG_GNUTELLA_ROUTING,
    PROP_LOG_BAD_GNUTELLA,
    PROP_LOG_SPAM_QUERY_HIT,
    PROP_MAX_ULTRA6_HOSTS_CACHED,
    PROP_HOSTS_IN_ULTRA6_CATCHER,
    PROP_HOSTS_IN_GUESS6_CATCHER,
    PROP_HOSTS_IN_GUESS6_INTRO_CATCHER,
    PROP_MAX_GUESS6_HOSTS_CACHED,
    PROP_MAX_GUESS6_INTRO_HOSTS_CACHED,
    PROP_XMALLOC_DEBUG,
    PROP_QHIT_BAD_DEBUG,
    PROP_GUID_DEBUG,
    PROP_TX_DEFLATE_DEBUG,
    PROP_TX_DEBUG_ADDRS,
    PROP_DUMP_RX_ADDRS,
    PROP_DUMP_TX_FROM_ADDRS,
    PROP_DUMP_TX_TO_ADDRS,
    PROP_GUESS_MAXIMIZE_BW,
    PROP_UDP_SCHED_DEBUG,
    PROP_TX_UT_DEBUG_FLAGS,
    PROP_RX_DEBUG_ADDRS,
    PROP_RX_UT_DEBUG_FLAGS,
    PROP_LOG_SR_UDP_TX,
    PROP_LOG_SR_UDP_RX,
    PROP_SECURE_OOB_DEBUG,
    PROP_LOG_VMSG_TX,
    PROP_LOG_VMSG_RX,
    PROP_DHT_TCACHE_DEBUG_FLAGS,
    PROP_LOG_WEIRD_DHT_HEADERS,
    PROP_DHT_RPC_DEBUG,
    PROP_LOG_UHC_PINGS_RX,
    PROP_LOG_UHC_PINGS_TX,
    PROP_CLEAN_SHUTDOWN,
    PROP_CLEAN_RESTART,
    PROP_DHT_KEYS_DEBUG,
    PROP_DHT_VALUES_DEBUG,
    PROP_PID,
    PROP_HTTP_RANGE_DEBUG,
    PROP_UPNP_MAPPING_LEASE_TIME,
    PROP_USER_AUTO_RESTART,
    PROP_TM_DEBUG,
    PROP_TMALLOC_DEBUG,
    PROP_EVQ_DEBUG,
    PROP_MAX_G2HUB_HOSTS_CACHED,
    PROP_HOSTS_IN_G2HUB_CATCHER,
    PROP_ENABLE_G2,
    PROP_NODE_G2_COUNT,
    PROP_MAX_G2_HUBS,
    PROP_LOG_BAD_G2,
    PROP_LOG_DROPPED_G2,
    PROP_G2_RPC_DEBUG,
    PROP_LOG_QUERY_HITS,
    PROP_LOG_QUERY_HIT_RECORDS,
    PROP_G2_BROWSE_COUNT,
    PROP_G2_BROWSE_SERVED,
    PROP_LOG_SENDING_G2,
    PROP_SESSION_START_STAMP,
    PROP_TCP_NO_LISTENING,
    PROP_QUERY_TRACE,
    PROP_INPUTEVT_TRACE,
    PROP_LOCK_CONTENTION_TRACE,
    PROP_LOCK_SLEEP_TRACE,
    GNET_PROPERTY_END
} gnet_property_t;

/*
 * Property set stub
 */
const prop_set_stub_t *gnet_prop_get_stub(void);

/*
 * Property definition
 */
prop_def_t *gnet_prop_get_def(property_t);
property_t gnet_prop_get_by_name(const char *);
pslist_t *gnet_prop_get_by_regex(const char *, int *);
const char *gnet_prop_name(property_t);
const char *gnet_prop_type_to_string(property_t);
const char *gnet_prop_to_string(property_t prop);
const char *gnet_prop_default_to_string(property_t);
const char *gnet_prop_description(property_t);
gboolean gnet_prop_is_saved(property_t);
gboolean gnet_prop_is_internal(property_t);
prop_type_t gnet_prop_type(property_t);
void gnet_prop_set_from_string(property_t, const char *);

void gnet_prop_lock(property_t);
void gnet_prop_unlock(property_t);

/*
 * Property-change listeners
 */
void gnet_prop_add_prop_changed_listener(
    property_t, prop_changed_listener_t, gboolean);
void gnet_prop_remove_prop_changed_listener(
    property_t, prop_changed_listener_t);

/*
 * get/set functions
 *
 * The *_val macros are shortcuts for single scalar properties.
 */
void gnet_prop_set_boolean(
    property_t, const gboolean *, size_t, size_t);
gboolean *gnet_prop_get_boolean(
    property_t, gboolean *, size_t, size_t);

static inline void
gnet_prop_set_boolean_val(property_t p, gboolean value)
{
	gnet_prop_set_boolean(p, &value, 0, 1);
}

static inline void
gnet_prop_get_boolean_val(property_t p, gboolean *value_ptr)
{
	gnet_prop_get_boolean(p, value_ptr, 0, 1);
}


void gnet_prop_set_string(property_t, const char *);
char *gnet_prop_get_string(property_t, char *, size_t);

void gnet_prop_set_guint32(
    property_t, const guint32 *, size_t, size_t);
guint32 *gnet_prop_get_guint32(
    property_t, guint32 *, size_t, size_t);

static inline void
gnet_prop_set_guint32_val(property_t p, guint32 value)
{
	gnet_prop_set_guint32(p, &value, 0, 1);
}

static inline void
gnet_prop_get_guint32_val(property_t p, guint32 *value_ptr)
{
	gnet_prop_get_guint32(p, value_ptr, 0, 1);
}

static inline void
gnet_prop_incr_guint32(property_t p)
{
	guint32 value;
	gnet_prop_lock(p);
	gnet_prop_get_guint32_val(p, &value);
	if G_UNLIKELY((guint32) -1 == value) {
		s_carp("%s(): incrementing property \"%s\" would overflow",
			G_STRFUNC, gnet_prop_name(p));
	} else {
		value++;
		gnet_prop_set_guint32_val(p, value);
	}
	gnet_prop_unlock(p);
}

static inline void
gnet_prop_decr_guint32(property_t p)
{
	guint32 value;
	gnet_prop_lock(p);
	gnet_prop_get_guint32_val(p, &value);
	if G_UNLIKELY(0 == value) {
		s_carp("%s(): decrementing property \"%s\" would underflow",
			G_STRFUNC, gnet_prop_name(p));
	} else {
		value--;
		gnet_prop_set_guint32_val(p, value);
	}
	gnet_prop_unlock(p);
}

void gnet_prop_set_guint64(
    property_t, const guint64 *, size_t, size_t);
guint64 *gnet_prop_get_guint64(
    property_t, guint64 *, size_t, size_t);

static inline void
gnet_prop_set_guint64_val(property_t p, guint64 value)
{
	gnet_prop_set_guint64(p, &value, 0, 1);
}

static inline void
gnet_prop_get_guint64_val(property_t p, guint64 *value_ptr)
{
	gnet_prop_get_guint64(p, value_ptr, 0, 1);
}

void gnet_prop_set_timestamp(
    property_t, const time_t *, size_t, size_t);
time_t *gnet_prop_get_timestamp(
    property_t, time_t *, size_t, size_t);

static inline void
gnet_prop_set_timestamp_val(property_t p, time_t value)
{
	gnet_prop_set_timestamp(p, &value, 0, 1);
}

static inline void
gnet_prop_get_timestamp_val(property_t p, time_t *value_ptr)
{
	gnet_prop_get_timestamp(p, value_ptr, 0, 1);
}

void gnet_prop_set_ip(
    property_t, const host_addr_t *, size_t, size_t);
host_addr_t *gnet_prop_get_ip(
    property_t, host_addr_t *, size_t, size_t);

static inline void
gnet_prop_set_ip_val(property_t p, host_addr_t value)
{
	gnet_prop_set_ip(p, &value, 0, 1);
}

static inline void
gnet_prop_get_ip_val(property_t p, host_addr_t *value_ptr)
{
	gnet_prop_get_ip(p, value_ptr, 0, 1);
}

void gnet_prop_set_storage(property_t, gconstpointer, size_t);
gpointer gnet_prop_get_storage(property_t, gpointer, size_t);

#endif /* _gnet_property_h_ */

/* vi: set ts=4 sw=4 cindent: */
