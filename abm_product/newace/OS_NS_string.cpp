#include "OS_NS_string.h"
char *strtok_r(char *s, const char *tokens, char **lasts)
{
  if (s == 0)
    s = *lasts;
  else
    *lasts = s;
  if (*s == 0)                  // We have reached the end
    return 0;
  size_t l_org = strlen (s);
  s = strtok (s, tokens);
  if (s == 0)
    return 0;
  const size_t l_sub = strlen (s);
  if (s + l_sub < *lasts + l_org)
    *lasts = s + l_sub + 1;
  else
    *lasts = s + l_sub;
  return s ;
}

