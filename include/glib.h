#pragma once

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

gchar *g_strconcat(const gchar *string1, ...)
{
  va_list args;
  va_start(args, string1);
  gchar *result = strdup(string1);

  const gchar *string;
  while ((string = va_arg(args, const gchar *)))
  {
    result = realloc(result, strlen(result) + strlen(string) + 1);
    strcat(result, string);
  }

  va_end(args);
  return result;
}

gchar *g_strdup(const gchar *string)
{
  return strdup(string);
}

#define g_print printf