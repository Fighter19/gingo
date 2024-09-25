#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

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

// Stubbed function pointers for now
typedef void (*GBaseInitFunc)();
typedef void (*GBaseFinalizeFunc)();
typedef void (*GClassInitFunc)(gpointer klass, gpointer class_data);
typedef void (*GClassFinalizeFunc)(gpointer klass, gpointer class_data);
typedef void (*GInstanceInitFunc)(gpointer object, gpointer klass);

typedef void (*GInterfaceInitFunc)(gpointer interface, gpointer interface_data);
typedef void (*GInterfaceFinalizeFunc)();

/** Contains information required to create instances of a class. */
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

/** Contains information about to (de-)initialize an interface.
 *
 * This is used to initialize the vtable of an interface.
 */
typedef struct _GInterfaceInfo {
  // Called when the vtable of the interface is being constructed
  GInterfaceInitFunc interface_init_func;
  // Called when the vtable of the interface is being destroyed
  GInterfaceFinalizeFunc interface_finalize_func;
  // User data that is passed to the init and finalize functions
  void *user_data;
} GInterfaceInfo;

/*
    GTypeInterface *type_interface;
    const gchar *type_name;
*/

typedef int GType;

typedef struct _InterfaceEntry {
  /* Type of the interface implemented by the class */
  GType *type;
  /* Information on how to construct the vtable of the interface */
  GInterfaceInfo *info;
  /* Virtual function table of the interface implemented by the class */
  gpointer vtable;
  /* Size of virtual function table */
  gsize vtable_size;
} InterfaceEntry;

#define MAX_INTERFACES_PER_TYPE 4
/** Cotains info about class hierarchy */
typedef struct _GTypeClass {
  GType type;
  struct _GTypeClass *parent;
  // Interfaces that the class implements
  InterfaceEntry interfaces[MAX_INTERFACES_PER_TYPE];
  gsize n_interfaces;
} GTypeClass;

/** Contains the type of an instance of a class. */
typedef struct _GTypeInstance {
  GType type;
} GTypeInstance;

/** Contains information about a type that specifies an interface. */
typedef struct _GTypeInterface {
  GType type;
  struct _GTypeInterface *parent;
  const GInterfaceInfo *info;
} GTypeInterface;

/** Contains information about a type that specifies an object. */
typedef struct _GObjectClass {
  GTypeClass base;
} GObjectClass;

/** Contains information about an instance of a class.
 *
 * This is the base class for all objects.
 * It also includes reference counting.
 */
typedef struct _GObject {
  GTypeInstance base;
  int ref_count;
} GObject;

static void *g_malloc(gsize count) { return malloc(count); }

/** Called for tracking memory allocations with static lifetime. */
static void *g_malloc_static(gsize count) { return g_malloc(count); }

static void g_free(void *ptr) { free(ptr); }

#define MAX_TYPES 20

struct _GTypePool {
  int last;
  GTypeInfo pool[MAX_TYPES];
  GTypeClass classes[MAX_TYPES];
  const char *type_names[MAX_TYPES];
} g_typePool = {.last = 0, .pool = {}, .classes = {}};

typedef enum _GTypeFlags { TYPE_FLAG_NONE } GTypeFlags;

enum BuiltinTypes {
  G_TYPE_TYPE,
  G_TYPE_OBJECT,
  G_TYPE_INTERFACE,

  G_TYPE_BUILTIN_COUNT
};

static GType g_type_register_static(GType parent, const gchar *type_name,
                                    const GTypeInfo *type_info,
                                    GTypeFlags flags) {
  g_typePool.pool[g_typePool.last] = *type_info;
  g_typePool.classes[g_typePool.last].type = g_typePool.last;
  g_typePool.type_names[g_typePool.last] = type_name;
  g_typePool.classes[g_typePool.last].parent = &g_typePool.classes[parent];
  return g_typePool.last++;
}

/** Adds an interface to a class.
 * @param instance_type The type of the class implementing the interface.
 * @param interface_type The type of the interface.
 * @param info Function pointer table used to initialize the vtable of the
 * interface.
 */
static void g_type_add_interface_static(GType instance_type,
                                        GType interface_type,
                                        const GInterfaceInfo *info) {
  InterfaceEntry entry = {.type = &g_typePool.classes[interface_type].type,
                          .info = (GInterfaceInfo *)info};
  gsize n_interfaces = g_typePool.classes[instance_type].n_interfaces;
  g_typePool.classes[instance_type].interfaces[n_interfaces] = entry;
  g_typePool.classes[instance_type].n_interfaces++;
}

#define g_type_class_peek_parent(klass)                                        \
  g_type_class_peek_parent_actual((GTypeClass *)klass)
static GTypeClass *g_type_class_peek_parent_actual(GTypeClass *klass) {
  return klass->parent;
}

#define g_type_interface_peek_parent(interface)                                \
  (gpointer) g_type_interface_peek_parent_actual((GTypeInterface *)interface)
static GTypeInterface *
g_type_interface_peek_parent_actual(GTypeInterface *interface) {
  if (interface == NULL) {
    return NULL;
  }
  return interface->parent;
}

__attribute__((constructor)) static void g_type_init() {
  g_assert(g_typePool.last == 0);
  static const GTypeInfo g_type_class_type_info = {
      .class_size = sizeof(GTypeClass),
      .base_init_func = 0,
      .base_finalize_func = 0,
      .class_init_func = 0,
      .class_finalize_func = 0,
      .class_data = 0,
      .object_size = sizeof(GTypeInstance),
      .preallocs = 0,
      .instance_init = 0,
      ._unused = 0};
  static const GTypeInfo g_object_type_info = {.class_size =
                                                   sizeof(GObjectClass),
                                               .base_init_func = 0,
                                               .base_finalize_func = 0,
                                               .class_init_func = 0,
                                               .class_finalize_func = 0,
                                               .class_data = 0,
                                               .object_size = sizeof(GObject),
                                               .preallocs = 0,
                                               .instance_init = 0,
                                               ._unused = 0};
  static const GTypeInfo g_interface_type_info = {.class_size =
                                                      sizeof(GObjectClass),
                                                  .base_init_func = 0,
                                                  .base_finalize_func = 0,
                                                  .class_init_func = 0,
                                                  .class_finalize_func = 0,
                                                  .class_data = 0,
                                                  .object_size = 0,
                                                  .preallocs = 0,
                                                  .instance_init = 0,
                                                  ._unused = 0};

  g_typePool.pool[G_TYPE_TYPE] = g_type_class_type_info;
  g_typePool.pool[G_TYPE_OBJECT] = g_object_type_info;
  g_typePool.pool[G_TYPE_INTERFACE] = g_interface_type_info;
  // TODO: Maybe this can be avoided by looking at the address the class is
  // stored at
  g_typePool.classes[G_TYPE_TYPE].type = G_TYPE_TYPE;
  g_typePool.classes[G_TYPE_OBJECT].type = G_TYPE_OBJECT;
  g_typePool.classes[G_TYPE_INTERFACE].type = G_TYPE_INTERFACE;

  g_typePool.classes[G_TYPE_TYPE].parent = NULL;
  g_typePool.classes[G_TYPE_OBJECT].parent = &g_typePool.classes[G_TYPE_TYPE];
  g_typePool.classes[G_TYPE_INTERFACE].parent = NULL;

  g_typePool.type_names[G_TYPE_TYPE] = "GTypeInstance";
  g_typePool.type_names[G_TYPE_OBJECT] = "GObject";
  g_typePool.type_names[G_TYPE_INTERFACE] = "GInterface";

  g_typePool.last = G_TYPE_BUILTIN_COUNT;
}

static GTypeClass *g_type_get_class(GType type) {
  return &g_typePool.classes[type];
}

static GTypeInfo *g_type_get_type_info(GType type) {
  return &g_typePool.pool[type];
}

static gpointer private_g_type_get_interface(GTypeInstance *obj, GType type,
                                             gsize vtable_size) {
  GTypeClass *klass = g_type_get_class(obj->type);
  for (gsize i = 0; i < klass->n_interfaces; i++) {
    if (*klass->interfaces[i].type == type) {
      InterfaceEntry *entry = &klass->interfaces[i];
      entry->vtable_size = vtable_size;
      // Lazily initialize the vtable (I haven't found another way to obtain the
      // vtable size)
      if (entry->vtable == NULL) {
        entry->vtable = g_malloc_static(vtable_size);
        GTypeInfo *class_type_info = g_type_get_type_info(type);
        g_assert(class_type_info);
        if (class_type_info->class_init_func) {
          class_type_info->class_init_func(entry->vtable,
                                           entry->info->user_data);
        }
        entry->info->interface_init_func(entry->vtable, entry->info->user_data);
      }
      return (gpointer)klass->interfaces[i].vtable;
    }
  }
  return NULL;
}

// According to the doc, a no-op is acceptable
// However g_autoptr probably won't work
#define G_DEFINE_AUTOPTR_CLEANUP_FUNC(type_name, func)
#define g_return_val_if_fail(cond, val)                                        \
  if (!(cond))                                                                 \
    return val;
#define G_TYPE_INSTANCE_GET_INTERFACE(obj, type_id, interface_name)            \
  private_g_type_get_interface((GTypeInstance *)obj, type_id,                  \
                               sizeof(interface_name))

// These functions are probably implemented in a thread-safe way in a
// multi-threaded environment
static bool g_once_init_enter(volatile gsize *location) {
  if (*location == 0) {
    return true;
  }
  return false;
}

static void g_once_init_leave(volatile gsize *location, int value) {
  *location = value;
}

// GObject

static void g_object_ref(GObject *obj) {
  g_assert(obj);
  obj->ref_count++;
}

#define g_object_unref(obj) g_object_unref_actual((GObject *)obj)

static void g_object_unref_actual(GObject *obj) {
  g_assert(obj);
  g_assert(obj->ref_count > 0);

  obj->ref_count--;

  if (obj->ref_count == 0) {
    // IIRC, finalize of the class of the instance would be called
    g_free(obj);
  }
}

// Originally, this was used internally to check if the instance is of a
// fundamental type However this was added before realizing that a GObject
// likely derives from GTypeInstance
static bool g_type_check_instance_is_fundamentally_a(GTypeClass *instance,
                                                     GType fundamental_type) {
  // Iterate through the class hierarchy
  GTypeClass *klass = instance;
  while (klass) {
    GTypeClass *next = g_type_class_peek_parent(klass);
    if (next) {
      klass = next;
    } else {
      // We reached the top of the hierarchy
      if (klass->type == fundamental_type) {
        return true;
      }
      return false;
    }
  }
  return false;
}

static bool g_type_check_type_is_a(GType derived, GType base) {
  // Iterate through the class hierarchy (assuming single inheritance)
  GTypeClass *klass;
  for (klass = g_type_get_class(derived); klass;
       klass = g_type_class_peek_parent(klass)) {
    if (klass->type == base) {
      return true;
    }
  }
}

static void *g_object_new(GType type, const gchar *first_property_name, ...) {
  // g_type_init() should be called before this function
  assert(g_typePool.last >= G_TYPE_BUILTIN_COUNT);
  GObject *obj = g_malloc(g_typePool.pool[type].object_size);
  g_assert(obj);
  if (g_type_check_type_is_a(type, G_TYPE_TYPE)) {
    obj->base.type = type;
  }
  if (g_type_check_type_is_a(type, G_TYPE_OBJECT)) {
    obj->ref_count = 1;
  }

  if (g_typePool.pool[type].instance_init)
    g_typePool.pool[type].instance_init(obj, NULL);

  return obj;
}