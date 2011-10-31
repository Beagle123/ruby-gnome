/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/*
 *  Copyright (C) 2011  Ruby-GNOME2 Project Team
 *  Copyright (C) 2004-2006  Ruby-GNOME2 Project Team
 *  Copyright (C) 2002,2003  Masahiro Sakai
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
#include <ctype.h>

#define RG_TARGET_NAMESPACE rbgobj_cFlags

VALUE RG_TARGET_NAMESPACE;

static ID id_new;
static ID id_module_eval;
static ID id_or;

extern char *rbgobj_constant_lookup(const char *name);
extern VALUE resolve_enum_value(VALUE klass, VALUE nick);

/**********************************************************************/

static VALUE
resolve_flags_value(VALUE klass, VALUE nick_or_nicks)
{
    int i, len;
    VALUE flags_value;

    if (!RVAL2CBOOL(rb_obj_is_kind_of(nick_or_nicks, rb_cArray)))
        return resolve_enum_value(klass, nick_or_nicks);

    len = RARRAY_LEN(nick_or_nicks);
    flags_value = rb_funcall(klass, id_new, 0);
    for (i = 0; i < len; i++) {
        VALUE value;

        value = resolve_enum_value(klass, RARRAY_PTR(nick_or_nicks)[i]);
        if (NIL_P(value))
            return Qnil;

	flags_value = rb_funcall(flags_value, id_or, 1, value);
    }

    return flags_value;
}

void
rbgobj_flags_add_constants(VALUE mod, GType flags_type,
                           const gchar *strip_prefix)
{
    GFlagsClass *gclass;
    guint i;
    int prefix_len = strlen(strip_prefix);

    gclass = G_FLAGS_CLASS(g_type_class_ref(flags_type));

    for (i = 0; i < gclass->n_values; i++) {
        const GFlagsValue* value = &gclass->values[i];

        if (strncmp(value->value_name, strip_prefix, prefix_len)) {
            g_warning("\"%s\" doesn't have prefix \"%s\"",
                      value->value_name, strip_prefix);
        } else {
            const char* name = value->value_name + prefix_len;
            rbgobj_define_const(mod, name,
                                rbgobj_make_flags(value->value, flags_type));

        }
    }

    g_type_class_unref(gclass);
}

/**********************************************************************/

typedef struct {
    GFlagsClass* gclass;
    guint value;
    GFlagsValue* info;
} flags_holder;

static void
flags_free(flags_holder* p)
{
    g_type_class_unref(p->gclass);
    free(p);
}

static flags_holder*
flags_get_holder(VALUE obj)
{
    flags_holder* p;
    Data_Get_Struct(obj, flags_holder, p);
    return p;
}

static VALUE
make_flags(guint n, VALUE klass)
{
    return rb_funcall(klass, id_new, 1, UINT2NUM(n));
}

VALUE
rbgobj_make_flags(guint n, GType gtype)
{
    return make_flags(n, GTYPE2CLASS(gtype));
}

guint
rbgobj_get_flags(VALUE obj, GType gtype)
{
    VALUE klass;

    if (!g_type_is_a(gtype, G_TYPE_FLAGS))
        rb_raise(rb_eTypeError, "%s is not a %s",
                 g_type_name(gtype), g_type_name(G_TYPE_FLAGS));

    /* for compatibility */
    if (rb_obj_is_kind_of(obj, rb_cInteger))
        obj = rbgobj_make_flags(NUM2UINT(obj), gtype);

    klass = GTYPE2CLASS(gtype);

    if (!rb_obj_is_kind_of(obj, klass)) {
        VALUE flags_value = Qnil;

        flags_value = resolve_flags_value(klass, obj);
        if (!NIL_P(flags_value))
            obj = flags_value;
    }

    if (rb_obj_is_kind_of(obj, klass))
        return flags_get_holder(obj)->value;
    else
        rb_raise(rb_eTypeError, "not a %s: %s",
		 rb_class2name(klass), RBG_INSPECT(obj));
}

/**********************************************************************/

void
rbgobj_init_flags_class(VALUE klass)
{
    GFlagsClass* gclass = g_type_class_ref(CLASS2GTYPE(klass));
    GString* source = g_string_new(NULL);
    guint i;

    for (i = 0; i < gclass->n_values; i++) {
        GFlagsValue* entry = &(gclass->values[i]);
        gchar* nick;
        gchar* p;
        gchar* replace_nick;

        replace_nick = rbgobj_constant_lookup(entry->value_nick);
        if (replace_nick){
            nick = g_strdup(replace_nick);
        } else {
            nick = g_strdup(entry->value_nick);
        }

        for (p = nick; *p; p++)
            if (*p == '-' || *p == ' ')
                *p = '_';
            else
                *p = tolower(*p);

        g_string_append_printf(
            source,
            "def %s%s?; self >= self.class.new(%d); end\n",
            g_ascii_isdigit(nick[0]) ? "_" : "",
            nick, entry->value);

        for (p = nick; *p; p++)
            *p = g_ascii_toupper(*p);

#if 0
        {
            ID id = rb_intern(nick);
            if (rb_is_const_id(id)) {
                rb_define_const(klass, nick, make_flags(entry->value, klass));
            }
        }
#else
        {
            rbgobj_define_const(klass, nick, make_flags(entry->value, klass));
        }
#endif

        g_free(nick);
    }

    rb_funcall(klass, id_module_eval, 1, rb_str_new2(source->str));
    g_string_free(source, TRUE);

    g_type_class_unref(gclass);
}

static VALUE
flags_s_mask(VALUE klass)
{
    GFlagsClass* gclass = g_type_class_ref(CLASS2GTYPE(klass));
    VALUE result = UINT2NUM(gclass->mask);
    g_type_class_unref(gclass);
    return result;
}

static VALUE
flags_s_values(VALUE klass)
{
    GFlagsClass *gclass;
    VALUE result;
    guint i;

    gclass = g_type_class_ref(CLASS2GTYPE(klass));
    result = rb_ary_new();
    for (i = 0; i < gclass->n_values; i++) {
        GFlagsValue *p = &(gclass->values[i]);
        rb_ary_push(result, make_flags(p->value, klass));
    }
    g_type_class_unref(gclass);

    return result;
}

static VALUE
flags_s_allocate(VALUE self)
{
    GType gtype = CLASS2GTYPE(self);

    if (G_TYPE_IS_ABSTRACT(gtype)) {
        rb_raise(rb_eTypeError, "abstract class");
    } else {
        flags_holder* p;
        VALUE result = Data_Make_Struct(self, flags_holder, NULL, flags_free, p);
        p->gclass = g_type_class_ref(gtype);
        p->value  = 0;
        p->info   = NULL;
        return result;
    }
}

static VALUE
flags_initialize(int argc, VALUE* argv, VALUE self)
{
    flags_holder* p = flags_get_holder(self);
    VALUE arg;

    rb_scan_args(argc, argv, "01", &arg);

    if (argc == 0) {
        p->value = 0;
    } else {
        if (rb_respond_to(arg, rb_intern("to_str"))) {
            const char* str = StringValuePtr(arg);
            p->info = g_flags_get_value_by_name(p->gclass, str);
            if (!p->info)
                p->info = g_flags_get_value_by_nick(p->gclass, str);
            if (!p->info)
                rb_raise(rb_eArgError, "invalid argument");
            p->value = p->info->value;
        } else {
            p->value = NUM2UINT(arg);
        }
    }

    if (!p->info) {
        guint i;
        for (i = 0; i < p->gclass->n_values; i++){
            GFlagsValue* val = &(p->gclass->values[i]);
            if (val->value == p->value){
                p->info = val;
                break;
            }
        }
    }

    return Qnil;
}

static VALUE
flags_to_i(VALUE self)
{
    flags_holder* p = flags_get_holder(self);
    return UINT2NUM(p->value);
}

static VALUE
flags_name(VALUE self)
{
    flags_holder* p = flags_get_holder(self);
    return p->info ? rb_str_new2(p->info->value_name) : Qnil;
}

static VALUE
flags_nick(VALUE self)
{
    flags_holder* p = flags_get_holder(self);
    return p->info ? rb_str_new2(p->info->value_nick) : Qnil;
}

static VALUE
flags_compare(VALUE self, VALUE rhs)
{
    flags_holder* p = flags_get_holder(self);
    GType gtype = G_TYPE_FROM_CLASS(p->gclass);
    guint rhs_val;

    if (CLASS_OF(rhs) != CLASS_OF(self) &&
        !rb_obj_is_kind_of(rhs, rb_cInteger))
        return Qnil;

    rhs_val = rbgobj_get_flags(rhs, gtype);

    if (p->value == rhs_val)
        return INT2FIX(0);
    else if ((p->value & rhs_val) == rhs_val)
        return INT2FIX(1);
    else if ((p->value & rhs_val) == p->value)
        return INT2FIX(-1);
    else
        return Qnil;
}

static VALUE
flags_eqv(VALUE self, VALUE rhs)
{
    flags_holder* p = flags_get_holder(self);
    GType gtype = G_TYPE_FROM_CLASS(p->gclass);
    guint rhs_val;

    if (CLASS_OF(rhs) != CLASS_OF(self) &&
        !rb_obj_is_kind_of(rhs, rb_cInteger))
        return Qnil;

    rhs_val = rbgobj_get_flags(rhs, gtype);
    return CBOOL2RVAL(p->value == rhs_val);
}

static VALUE
flags_gt_eq(VALUE self, VALUE rhs)
{
    flags_holder* p = flags_get_holder(self);
    GType gtype = G_TYPE_FROM_CLASS(p->gclass);
    guint rhs_val;

    if (CLASS_OF(rhs) != CLASS_OF(self) &&
        !rb_obj_is_kind_of(rhs, rb_cInteger))
        return Qnil;

    rhs_val = rbgobj_get_flags(rhs, gtype);
    return CBOOL2RVAL((p->value & rhs_val) == rhs_val);
}

static VALUE
flags_lt_eq(VALUE self, VALUE rhs)
{
    flags_holder* p = flags_get_holder(self);
    GType gtype = G_TYPE_FROM_CLASS(p->gclass);
    guint rhs_val;

    if (CLASS_OF(rhs) != CLASS_OF(self) &&
        !rb_obj_is_kind_of(rhs, rb_cInteger))
        return Qnil;

    rhs_val = rbgobj_get_flags(rhs, gtype);
    return CBOOL2RVAL((p->value & rhs_val) == p->value);
}

static VALUE
flags_gt(VALUE self, VALUE rhs)
{
    flags_holder* p = flags_get_holder(self);
    GType gtype = G_TYPE_FROM_CLASS(p->gclass);
    guint rhs_val;

    if (CLASS_OF(rhs) != CLASS_OF(self) &&
        !rb_obj_is_kind_of(rhs, rb_cInteger))
        return Qnil;

    rhs_val = rbgobj_get_flags(rhs, gtype);
    return CBOOL2RVAL((p->value & rhs_val) == rhs_val &&
                      p->value != rhs_val);
}

static VALUE
flags_lt(VALUE self, VALUE rhs)
{
    flags_holder* p = flags_get_holder(self);
    GType gtype = G_TYPE_FROM_CLASS(p->gclass);
    guint rhs_val;

    if (CLASS_OF(rhs) != CLASS_OF(self) &&
        !rb_obj_is_kind_of(rhs, rb_cInteger))
        return Qnil;

    rhs_val = rbgobj_get_flags(rhs, gtype);
    return CBOOL2RVAL((p->value & rhs_val) == p->value &&
                      p->value != rhs_val);
}

static VALUE
flags_not(VALUE self, G_GNUC_UNUSED VALUE rhs)
{
    flags_holder* p = flags_get_holder(self);
    return rbgobj_make_flags((~ p->value) & p->gclass->mask,
                             G_TYPE_FROM_CLASS(p->gclass));
}

#define LIFT_BINARY_OP(funcname, op) \
    static VALUE \
    funcname(VALUE self, VALUE rhs) \
    { \
        flags_holder* p = flags_get_holder(self); \
        GType gtype = G_TYPE_FROM_CLASS(p->gclass); \
        return rbgobj_make_flags(p->value op rbgobj_get_flags(rhs, gtype), \
                                 gtype); \
    }

LIFT_BINARY_OP(flags_and, &)
LIFT_BINARY_OP(flags_or, |)
LIFT_BINARY_OP(flags_xor, ^)

static VALUE
flags_minus(VALUE self, VALUE rhs)
{
    flags_holder* p = flags_get_holder(self);
    GType gtype = G_TYPE_FROM_CLASS(p->gclass);
    return rbgobj_make_flags(p->value & ~rbgobj_get_flags(rhs, gtype),
                             gtype);
}

static VALUE
flags_empty_p(VALUE self)
{
    flags_holder* p = flags_get_holder(self);
    return CBOOL2RVAL(p->value == 0);
}

static VALUE
flags_hash(VALUE self)
{
    flags_holder* p = flags_get_holder(self);
    return UINT2NUM(p->value ^ G_TYPE_FROM_CLASS(p->gclass));
}

static VALUE
flags_coerce(VALUE self, VALUE other)
{
    flags_holder *holder;
    GType gtype;

    if (rb_obj_is_kind_of(other, rb_cInteger))
        rb_raise(rb_eTypeError, "can't coerce");

    holder = flags_get_holder(self);
    gtype = G_TYPE_FROM_CLASS(holder->gclass);
    other = rbgobj_make_flags(NUM2UINT(other), gtype);
    return rb_ary_new3(2, other, self);
}

static VALUE
flags_nonzero_p(VALUE self)
{
    flags_holder* p = flags_get_holder(self);
    return CBOOL2RVAL(p->value != 0);
}

/**********************************************************************/

void
Init_gobject_gflags(void)
{
    id_module_eval = rb_intern("module_eval");
    id_new = rb_intern("new");
    id_or = rb_intern("|");

    RG_TARGET_NAMESPACE = G_DEF_CLASS(G_TYPE_FLAGS, "Flags", mGLib);

    rb_define_singleton_method(RG_TARGET_NAMESPACE, "gtype", generic_s_gtype, 0);
    rb_define_method(RG_TARGET_NAMESPACE, "gtype", generic_gtype, 0);

    rb_define_singleton_method(RG_TARGET_NAMESPACE, "mask", flags_s_mask, 0);
    rb_define_singleton_method(RG_TARGET_NAMESPACE, "values", flags_s_values, 0);

    rb_define_alloc_func(RG_TARGET_NAMESPACE, flags_s_allocate);

    rb_define_method(RG_TARGET_NAMESPACE, "initialize", flags_initialize, -1);

    rb_define_method(RG_TARGET_NAMESPACE, "to_i", flags_to_i, 0);
    rb_define_alias(RG_TARGET_NAMESPACE, "to_int", "to_i");
    rb_define_method(RG_TARGET_NAMESPACE, "name", flags_name, 0);
    rb_define_method(RG_TARGET_NAMESPACE, "nick", flags_nick, 0);

    /*
    rb_define_method(RG_TARGET_NAMESPACE, "inspect", flags_inspect, 0);
    */

    rb_define_method(RG_TARGET_NAMESPACE, "<=>", flags_compare, 1);
    rb_define_method(RG_TARGET_NAMESPACE, "==", flags_eqv, 1);
    rb_define_method(RG_TARGET_NAMESPACE, ">=", flags_gt_eq, 1);
    rb_define_method(RG_TARGET_NAMESPACE, "<=", flags_lt_eq, 1);
    rb_define_method(RG_TARGET_NAMESPACE, ">", flags_gt, 1);
    rb_define_method(RG_TARGET_NAMESPACE, "<", flags_lt, 1);
    rb_define_method(RG_TARGET_NAMESPACE, "~", flags_not, 0);
    rb_define_method(RG_TARGET_NAMESPACE, "&", flags_and, 1);
    rb_define_method(RG_TARGET_NAMESPACE, "|", flags_or, 1);
    rb_define_method(RG_TARGET_NAMESPACE, "^", flags_xor, 1);
    rb_define_method(RG_TARGET_NAMESPACE, "-", flags_minus, 1);

    rb_define_method(RG_TARGET_NAMESPACE, "empty?", flags_empty_p, 0);

    rb_define_method(RG_TARGET_NAMESPACE, "hash", flags_hash, 0);
    rb_define_method(RG_TARGET_NAMESPACE, "eql?", flags_eqv, 1);

    /* for compatibility */
    rb_define_method(RG_TARGET_NAMESPACE, "coerce", flags_coerce, 1);
    rb_define_method(RG_TARGET_NAMESPACE, "zero?", flags_empty_p, 0);
    rb_define_method(RG_TARGET_NAMESPACE, "nonzero?", flags_nonzero_p, 0);
}

