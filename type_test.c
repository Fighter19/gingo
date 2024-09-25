#include <glib-object.h>
#include <glib.h>

void test_g_strconcat()
{
  gchar *result = g_strconcat("Hello, ", "world!", NULL);
  g_assert(result != NULL);
  g_assert(memcmp(result, "Hello, world!", 14) == 0);
  g_free(result);
}

int main()
{
  test_g_strconcat();
	// g_type_init();
}
