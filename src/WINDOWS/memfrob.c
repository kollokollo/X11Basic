#include <string.h>

void *memfrob(void *s, size_t n) {
  while(n--) *((char *)s++)^=42;
}
