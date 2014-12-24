failing_allocs
==============

[![Build
Status](https://travis-ci.org/mckinsel/failing_allocs.svg?branch=master)](https://travis-ci.org/mckinsel/failing_allocs)

Background
----------

C library code often needs to do something decorous if a dynamic memory
allocation fails. For example, it may clean up any successfully
allocated memory and indicate an error condition to its caller. This is
generally done by checking if malloc, calloc, or realloc returns a null
pointer and then calling some error handling code.

Testing the error handling can be a hassle because during tests, all
memory allocations will likely succeed. Moreover, you want to test that
error handling does the right thing for every possible failed
allocation, so you want each call to malloc/calloc/realloc to fail at
least once during your tests.

This is what failing\_allocs helps with. It overrides the standard
malloc, calloc, and realloc with functions that will return a null
pointer whenever they are called with a previously unobserved backtrace.
Then, in the test code, the function being tested can be called
repeatedly. Each time it is called, a different allocation will fail
until, finally, all allocations succeed.

Using failing_allocs
--------------------

There are three steps to converting test code to use failing\_allocs:

1. Include failing\_allocs.h. failing\_allocs is header-only.
2. Use the USE\_FAILING\_ALLOCS macro. This actually does the overriding
of malloc, calloc, and realloc.
3. At the end of the tests, use the FREE\_FAILING\_ALLOCS macro. This
frees memory the allocation functions use internally to keep track
of observed backtraces.

For example, consider the code below. The function `Container_create`
does some allocations and returns a pointer to a new `Container` struct.
If anything goes wrong, it should free any allocated memory and return
a null pointer.

```cpp
#include <assert.h>
#include <stdlib.h>

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
    
  new_container->some_ints = realloc(new_container->some_ints,
                                     count * sizeof(int));
  if(!new_container->some_ints) goto error;

  new_container->count = count;

  return new_container;

error:
  if(new_container->some_ints) free(new_container->some_ints);
  if(new_container) free(new_container);
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
  Container* container = Container_create(100);
  assert(container->count == 100);
  Container_delete(container);
  return 0;
}
```

Right now, the only thing that is being checked is that the count is
correct after the `Container` is created. The code for handling failed
allocations is not run. To check that, we can use failing_allocs:

```cpp
#include "failing_allocs.h"

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
  
  /* Superfluous realloc for demonstration. */
  new_container->some_ints = realloc(new_container->some_ints,
                                     count * sizeof(int));
  if(!new_container->some_ints) goto error;

  new_container->count = count;

  return new_container;

error:
  if(new_container->some_ints) free(new_container->some_ints);
  if(new_container) free(new_container);
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
    assert(container->count == 100);
    Container_delete(container);
  }

  FREE_FAILING_ALLOCS

  return 0;

}
```

The `Container_create` function is run multiple times since each run
leads to a new allocation failure. Now, the cleanup code is being run,
and this code should segfault because it tries to free
`new_container->some_ints` even if the allocation of `new_container`
failed.

If we fix that, we can run the test again under valgrind. That would
reveal a memory leak when the first malloc succeeds but the subsequent
realloc fails. The memory allocated by the first malloc becomes
unreachable.

So, failing_allocs lets you verify the behavior of your code against
the failure of every dynamic memory allocation.

Notes
-----

failing\_allocs needs libdl, so you may need to add -ldl to your
LDLIBS.

failing\_allocs also relies on the RLTD\_NEXT symbol, which is only 
available when you define \_GNU\_SOURCE. So, if the test or library
code does not itself  define \_GNU\_SOURCE, you should include
failing\_allocs.h before including anything else.
