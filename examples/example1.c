#include "../failing_allocs.h"

#include <assert.h>
#include <stdlib.h>

USE_FAILING_ALLOCS

typedef struct{
  int* some_ints;
  size_t count;
} Container;

Container* Container_create(size_t count)
{
  Container* new_container = calloc(1, sizeof(Container));
  if(!new_container) goto error;

  new_container->some_ints = malloc(5 * sizeof(int));
  if(!new_container->some_ints) goto error;
  
  int* new_ints = realloc(new_container->some_ints,
                          count * sizeof(int));
  if(!new_ints) goto error;
  new_container->some_ints = new_ints;

  new_container->count = count;

  return new_container;

error:
  if(new_container) {
    if(new_container->some_ints) free(new_container->some_ints);
    free(new_container);
  }
  return NULL;
}

void Container_delete(Container* container)
{
  if(container) {
    if(container->some_ints) free(container->some_ints);
    free(container);
  }
}

int main(void)
{
  int i = 0;
  for(i = 0; i < 10; i++) {
    Container* container = Container_create(100);  
    if(!container) continue;
    assert(container->count == 100);
    Container_delete(container);
  }

  FREE_FAILING_ALLOCS

  return 0;
}
