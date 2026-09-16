#include "Allocator.h"
#include <stdlib.h>

void *defaultAllocate(void *p_userData, size_t size, size_t alignment) {
  return malloc(size);
}
void defaultDeallocate(void *p_userData, void *ptr) {
  free(ptr);
  ptr = NULL;
}

const Allocator *getDefaultAllocator(void) {
  static const Allocator g_defaultAllocator = {
      .pfnAllocate = defaultAllocate,
      .pfnDeallocate = defaultDeallocate,
      .pUserData = NULL,
  };
  return &g_defaultAllocator;
}
