#include "../failing_allocs.h"
#include "minunit.h"
#include "dbg.h"

USE_FAILING_ALLOCS

/* Function that performs some allocations that will fail. */
int allocating_function(int** iarray, char** str, double** darray,
                        int** new_iarray)
{
  *iarray = malloc(10 * sizeof(int));
  check_mem(*iarray);

  *str = malloc(10 * sizeof(char));
  check_mem(*str);

  *darray = calloc(10, sizeof(double));
  check_mem(*darray);

  *new_iarray = realloc(*iarray, 100 * sizeof(int));
  check_mem(*new_iarray);
  *iarray = NULL;
  return 0;

error:
  return 1;
}

/*
 * Repeatedly call a function that allocates memory multiple times.
 * Each time it's called, check that the expected allocation failed.
 */
char* test_sequential_failures()
{
  int* iarray = NULL;
  char* str = NULL;
  double* darray = NULL;
  int* new_iarray = NULL;

  int i = 0;
  for(i = 0; i < 5; i++) {
    int retval = allocating_function(&iarray, &str, &darray, &new_iarray);
    switch(i) {
      case 0:
        mu_assert(retval == 1, "Succeeded on first run.");
        mu_assert(iarray == NULL, "First iarray alloc did not fail.");
        break;
      case 1:
        mu_assert(retval == 1, "Succeeded on second run.");
        mu_assert(iarray != NULL, "Second iarray alloc failed.");
        mu_assert(str == NULL, "Second str alloc did not fail.");
        break;
      case 2:
        mu_assert(retval == 1, "Succeeded on third run.");
        mu_assert(iarray != NULL, "Third iarray alloc failed.");
        mu_assert(str != NULL, "Third str alloc failed.");
        mu_assert(darray == NULL, "Third darray alloc did not fail.");
        break;
      case 3:
        mu_assert(retval == 1, "Succeeded on fourth run.");
        mu_assert(iarray != NULL, "Fourth iarray alloc failed.");
        mu_assert(str != NULL, "Fourth str alloc failed.");
        mu_assert(darray != NULL, "Fourth darray alloc failed.");
        mu_assert(new_iarray == NULL, "Fourth new_iarray realloc did not fail.");
        break;
      case 4:
        mu_assert(retval == 0, "Last run did not succeed.");
        mu_assert(iarray == NULL, "Last new_iarray realloc failed.");
        mu_assert(str != NULL, "Last str alloc failed.");
        mu_assert(darray != NULL, "Last darray alloc failed.");
        mu_assert(new_iarray != NULL, "Last new_iarray realloc failed.");
        break;
      default:
        mu_assert(1 == 0, "Should not get here.");
    }

    if(iarray) free(iarray);
    if(str) free(str);
    if(darray) free(darray);
    if(new_iarray) free(new_iarray);

  }

  return NULL;
}


char* all_tests()
{
  mu_suite_start();
  
  mu_run_test(test_sequential_failures);  
  
  FREE_FAILING_ALLOCS

  return NULL;
}

RUN_TESTS(all_tests);
