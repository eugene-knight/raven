#include "Allocator.h"
#include <stdlib.h>

void *default_allocate(void *p_userData, size_t size, size_t alignment) {
  return malloc(size);
}
void default_deallocate(void *p_userData, void *ptr) {
    free(ptr);
    ptr = NULL;
}

const Allocator g_defaultAllocator = {
    .pfn_allocate = default_allocate,
    .pfn_deallocate = default_deallocate,
    .p_userData = NULL,
};
