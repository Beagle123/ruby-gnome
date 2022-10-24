/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/*
 *  Copyright (C) 2015-2022  Ruby-GNOME Project Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include "rb-gtk4-private.h"

static void
rb_gtk4_tree_view_mark(gpointer object)
{
    GtkTreeView *tree_view = object;

    GList *columns = gtk_tree_view_get_columns(tree_view);
    GList *node;
    for (node = columns; node; node = g_list_next(node)) {
        GtkTreeViewColumn *column = node->data;
        rbgobj_gc_mark_instance(column);
    }
    g_list_free(columns);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    rbgobj_gc_mark_instance(selection);
}

void
rb_gtk4_tree_view_init(void)
{
    rbgobj_register_mark_func(GTK_TYPE_TREE_VIEW, rb_gtk4_tree_view_mark);
}
