/*
 * $Id$
 *
 * Copyright (c) 2004, Raphael Manfredi
 *
 * Dynamic querying.
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

#ifndef _dq_h_
#define _dq_h_

/*
 * Public interface.
 */

struct gnutella_node;
struct query_hashvec;

void dq_init(void);
void dq_close(void);

void dq_launch_net(struct gnutella_node *n, struct query_hashvec *qhv);
void dq_node_removed(guint32 node_id);
void dq_got_results(gchar *muid, gint count);
void dq_got_query_status(gchar *muid, guint32 node_id, guint16 kept);

/* vi: set ts=4: */

#endif	/* _dq_h_ */

