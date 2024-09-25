#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define G_GNUC_CONST
#define g_assert assert

typedef int gint;
typedef unsigned int gsize;
typedef unsigned char gchar;
typedef unsigned short guint16;
typedef void *gpointer;
typedef const void *gconstpointer;

typedef struct _GParamSpec {
} GParamSpec;

typedef struct _GTypeInterface {
} GTypeInterface;

// Stubbed function pointers for now
typedef void (*GBaseInitFunc)();
typedef void (*GBaseFinalizeFunc)();
typedef void (*GClassInitFunc)(gpointer klass, gpointer class_data);
typedef void (*GClassFinalizeFunc)(gpointer klass, gpointer class_data);
typedef void (*GInstanceInitFunc)(gpointer object, gpointer klass);

typedef void (*GInterfaceInitFunc)(gpointer interface, gpointer interface_data);
typedef void (*GInterfaceFinalizeFunc)();

typedef struct _GTypeInfo {
	guint16 class_size;
	GBaseInitFunc base_init_func;
	GBaseFinalizeFunc base_finalize_func;
	GClassInitFunc class_init_func;
	GClassFinalizeFunc class_finalize_func;
	gconstpointer class_data;
	guint16 object_size;
	guint16 preallocs;
	GInstanceInitFunc instance_init;
	gpointer *_unused;
} GTypeInfo;

typedef struct _GInterfaceInfo {
	GInterfaceInitFunc interface_init_func;
	GInterfaceFinalizeFunc interface_finalize_func;
} GInterfaceInfo;

/*
    GTypeInterface *type_interface;
    const gchar *type_name;
*/

typedef int GType;

typedef struct _GObjectClass {
	GType type;
} GObjectClass;

typedef struct _GObject {
	int ref_count;
} GObject;

static void *g_malloc(gsize count)
{
	return malloc(count);
}

static void g_free(void *ptr)
{
	free(ptr);
}

void g_object_unref(GObject *obj)
{
	g_assert(obj);
	g_assert(obj->ref_count > 0);

	obj->ref_count--;

	if (obj->ref_count == 0) {
		// IIRC, finalize of the class of the instance would be called
		g_free(obj);
	}
}

#define MAX_TYPES 20

struct _GTypePool {
	int last;
	GTypeInfo pool[MAX_TYPES];
} g_typePool = { .last = 0, .pool = {} };

typedef enum _GTypeFlags { TYPE_FLAG_NONE } GTypeFlags;

enum BuiltinTypes
{
	G_TYPE_OBJECT,
	G_TYPE_INTERFACE,

	G_TYPE_BUILTIN_COUNT
};

static GType g_type_register_static(GType parent, const gchar *type_name,
				    const GTypeInfo *type_info,
				    GTypeFlags flags)
{
	// Handle G_TYPE_OBJECT and G_TYPE_INTERFACE
	g_assert(false);
}

static void g_type_init()
{
	g_assert(g_typePool.last == 0);
	static const GTypeInfo g_object_type_info = {
		.class_size = sizeof(GObjectClass),
		.base_init_func = 0,
		.base_finalize_func = 0,
		.class_init_func = 0,
		.class_finalize_func = 0,
		.class_data = 0,
		.object_size = sizeof(GObject),
		.preallocs = 0,
		.instance_init = 0,
		._unused = 0
	};
    static const GTypeInfo g_interface_type_info = {
        .class_size = sizeof(GObjectClass),
		.base_init_func = 0,
		.base_finalize_func = 0,
		.class_init_func = 0,
		.class_finalize_func = 0,
		.class_data = 0,
		.object_size = sizeof(GObject),
		.preallocs = 0,
		.instance_init = 0,
		._unused = 0
    };

	g_typePool.pool[G_TYPE_OBJECT] = g_object_type_info;
	g_typePool.pool[G_TYPE_INTERFACE] = g_interface_type_info;
	g_typePool.last = G_TYPE_BUILTIN_COUNT;
}

static gpointer private_g_type_get_interface(GType type)
{
}

// According to the doc, a no-op is acceptable
// However g_autoptr probably won't work
#define G_DEFINE_AUTOPTR_CLEANUP_FUNC(type_name, func)
#define g_return_val_if_fail(cond, val) \
	if (!(cond))                    \
		return val;
#define G_TYPE_INSTANCE_GET_INTERFACE(obj, type_id, interface_name) private_g_type_get_interface(type_id)
