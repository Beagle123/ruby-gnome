/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/*
 *  Copyright (C) 2011  Ruby-GNOME2 Project Team
 *  Copyright (C) 2006  Kouhei Sutou
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

#include "rbgprivate.h"
#include "rbglib.h"

#define RG_TARGET_NAMESPACE mUniChar

#define DEF_IS_UNICHAR(name)                                         \
static VALUE                                                         \
rbglib_m_unichar_is ## name(G_GNUC_UNUSED VALUE self, VALUE unichar) \
{                                                                    \
    return CBOOL2RVAL(g_unichar_is ## name(NUM2UINT(unichar)));      \
}

DEF_IS_UNICHAR(alnum)
DEF_IS_UNICHAR(alpha)
DEF_IS_UNICHAR(cntrl)
DEF_IS_UNICHAR(digit)
DEF_IS_UNICHAR(graph)
DEF_IS_UNICHAR(lower)
DEF_IS_UNICHAR(print)
DEF_IS_UNICHAR(punct)
DEF_IS_UNICHAR(space)
DEF_IS_UNICHAR(upper)
DEF_IS_UNICHAR(xdigit)
DEF_IS_UNICHAR(title)
DEF_IS_UNICHAR(defined)
DEF_IS_UNICHAR(wide)
#if GLIB_CHECK_VERSION(2,12,0)
DEF_IS_UNICHAR(wide_cjk)
#endif

#undef DEF_IS_UNICHAR

static VALUE
rbglib_m_unichar_toupper(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return UINT2NUM(g_unichar_toupper(NUM2UINT(unichar)));
}

static VALUE
rbglib_m_unichar_tolower(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return UINT2NUM(g_unichar_tolower(NUM2UINT(unichar)));
}

static VALUE
rbglib_m_unichar_totitle(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return UINT2NUM(g_unichar_totitle(NUM2UINT(unichar)));
}

static VALUE
rbglib_m_unichar_digit_value(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return INT2NUM(g_unichar_digit_value(NUM2UINT(unichar)));
}

static VALUE
rbglib_m_unichar_xdigit_value(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return INT2NUM(g_unichar_xdigit_value(NUM2UINT(unichar)));
}

static VALUE
rbglib_m_unichar_type(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return GENUM2RVAL(g_unichar_type(NUM2UINT(unichar)),
                      G_TYPE_UNICODE_TYPE);
}

static VALUE
rbglib_m_unichar_break_type(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return GENUM2RVAL(g_unichar_break_type(NUM2UINT(unichar)),
                      G_TYPE_UNICODE_BREAK_TYPE);
}

#if GLIB_CHECK_VERSION(2,4,0)
static VALUE
rbglib_m_unichar_get_mirror_char(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    gunichar mirrored_char;

    if (g_unichar_get_mirror_char(NUM2UINT(unichar), &mirrored_char)) {
        return UINT2NUM(mirrored_char);
    } else {
        return unichar;
    }
}
#endif

#if GLIB_CHECK_VERSION(2,14,0)
static VALUE
rbglib_m_unichar_combining_class(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return INT2NUM(g_unichar_combining_class(NUM2UINT(unichar)));
}

static VALUE
rbglib_m_unichar_get_script(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return GENUM2RVAL(g_unichar_get_script(NUM2UINT(unichar)),
                      G_TYPE_UNICODE_SCRIPT);
}

static VALUE
rbglib_m_unichar_ismark(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return CBOOL2RVAL(g_unichar_ismark(NUM2UINT(unichar)));
}

static VALUE
rbglib_m_unichar_iszerowidth(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    return CBOOL2RVAL(g_unichar_iszerowidth(NUM2UINT(unichar)));
}
#endif

static VALUE
rbglib_m_unichar_to_utf8(G_GNUC_UNUSED VALUE self, VALUE unichar)
{
    gchar utf8[6];
    gint len;

    len = g_unichar_to_utf8(NUM2UINT(unichar), utf8);
    return CSTR2RVAL_LEN(utf8, len);
}

void
Init_glib_unichar(void)
{
    VALUE RG_TARGET_NAMESPACE = rb_define_module_under(mGLib, "UniChar");

    rb_define_module_function(RG_TARGET_NAMESPACE, "alnum?",
                              rbglib_m_unichar_isalnum, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "alpha?",
                              rbglib_m_unichar_isalpha, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "cntrl?",
                              rbglib_m_unichar_iscntrl, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "digit?",
                              rbglib_m_unichar_isdigit, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "graph?",
                              rbglib_m_unichar_isgraph, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "lower?",
                              rbglib_m_unichar_islower, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "print?",
                              rbglib_m_unichar_isprint, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "punct?",
                              rbglib_m_unichar_ispunct, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "space?",
                              rbglib_m_unichar_isspace, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "upper?",
                              rbglib_m_unichar_isupper, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "xdigit?",
                              rbglib_m_unichar_isxdigit, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "title?",
                              rbglib_m_unichar_istitle, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "defined?",
                              rbglib_m_unichar_isdefined, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "wide?",
                              rbglib_m_unichar_iswide, 1);
#if GLIB_CHECK_VERSION(2,12,0)
    rb_define_module_function(RG_TARGET_NAMESPACE, "wide_cjk?",
                              rbglib_m_unichar_iswide_cjk, 1);
#endif

    rb_define_module_function(RG_TARGET_NAMESPACE, "to_upper",
                              rbglib_m_unichar_toupper, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "to_lower",
                              rbglib_m_unichar_tolower, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "to_title",
                              rbglib_m_unichar_totitle, 1);

    rb_define_module_function(RG_TARGET_NAMESPACE, "digit_value",
                              rbglib_m_unichar_digit_value, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "xdigit_value",
                              rbglib_m_unichar_xdigit_value, 1);

    rb_define_module_function(RG_TARGET_NAMESPACE, "type", rbglib_m_unichar_type, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "break_type",
                              rbglib_m_unichar_break_type, 1);

#if GLIB_CHECK_VERSION(2,4,0)
    rb_define_module_function(RG_TARGET_NAMESPACE, "get_mirror_char",
                              rbglib_m_unichar_get_mirror_char, 1);
#endif

#if GLIB_CHECK_VERSION(2,14,0)
    rb_define_module_function(RG_TARGET_NAMESPACE, "combining_class",
			      rbglib_m_unichar_combining_class, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "get_script",
                              rbglib_m_unichar_get_script, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "mark?",
			      rbglib_m_unichar_ismark, 1);
    rb_define_module_function(RG_TARGET_NAMESPACE, "zero_width?",
			      rbglib_m_unichar_iszerowidth, 1);
#endif

    rb_define_module_function(RG_TARGET_NAMESPACE, "to_utf8",
                              rbglib_m_unichar_to_utf8, 1);
}

