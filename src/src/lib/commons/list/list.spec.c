/**
 * @file list.spec.c
 * @brief Unit tests for list module
 *
 * Comprehensive unit tests for the list ADT functions defined in list.h
 */

#include "./list.h"
#include "../../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for list_create()
// ============================================================================

/**
 * Test: list_create should return a valid list instance
 */
bool test_list_create_basic(void) {
  // Act: Create a list
  List list = list_create();

  // Assert: List should be created successfully
  ASSERT_NOT_NULL(list);

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: newly created list should be empty
 */
bool test_list_create_is_empty(void) {
  // Act: Create a list
  List list = list_create();
  ASSERT_NOT_NULL(list);

  // Assert: List should be empty
  ASSERT_TRUE(list_is_empty(list));
  ASSERT_EQUAL(list_size(list), 0);

  // Cleanup
  list_destroy(list);

  return true;
}

// ============================================================================
// Tests for list_insert_back()
// ============================================================================

/**
 * Test: list_insert_back should add element to end of list
 */
bool test_list_insert_back_single_element(void) {
  // Arrange: Create a list
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int value = 42;

  // Act: Insert an element
  bool result = list_insert_back(list, &value);

  // Assert: Insert should succeed and list should not be empty
  ASSERT_TRUE(result);
  ASSERT_FALSE(list_is_empty(list));
  ASSERT_EQUAL(list_size(list), 1);

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: list_insert_back should add multiple elements
 */
bool test_list_insert_back_multiple_elements(void) {
  // Arrange: Create a list
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30, 40, 50};
  int count = 5;

  // Act: Insert multiple elements
  for (int i = 0; i < count; i++) {
    bool result = list_insert_back(list, &values[i]);
    ASSERT_TRUE(result);
  }

  // Assert: List should have correct size
  ASSERT_EQUAL(list_size(list), count);
  ASSERT_FALSE(list_is_empty(list));

  // Cleanup
  list_destroy(list);

  return true;
}

// ============================================================================
// Tests for list_insert_front()
// ============================================================================

/**
 * Test: list_insert_front should add element to beginning of list
 */
bool test_list_insert_front_single_element(void) {
  // Arrange: Create a list
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int value = 42;

  // Act: Insert an element
  bool result = list_insert_front(list, &value);

  // Assert: Insert should succeed
  ASSERT_TRUE(result);
  ASSERT_FALSE(list_is_empty(list));
  ASSERT_EQUAL(list_size(list), 1);

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: list_insert_front should add multiple elements
 */
bool test_list_insert_front_multiple_elements(void) {
  // Arrange: Create a list
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30};

  // Act: Insert multiple elements at front
  for (int i = 0; i < 3; i++) {
    bool result = list_insert_front(list, &values[i]);
    ASSERT_TRUE(result);
  }

  // Assert: List should have correct size and order (reversed)
  ASSERT_EQUAL(list_size(list), 3);
  int *first = (int *)list_get_first(list);
  ASSERT_NOT_NULL(first);
  ASSERT_EQUAL(*first, 30); // Last inserted should be first

  // Cleanup
  list_destroy(list);

  return true;
}

// ============================================================================
// Tests for list_get()
// ============================================================================

/**
 * Test: list_get should return element at index
 */
bool test_list_get_basic(void) {
  // Arrange: Create a list and add elements
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30, 40, 50};

  for (int i = 0; i < 5; i++) {
    list_insert_back(list, &values[i]);
  }

  // Act & Assert: Get elements at different indices
  for (int i = 0; i < 5; i++) {
    int *result = (int *)list_get(list, i);
    ASSERT_NOT_NULL(result);
    ASSERT_EQUAL(*result, values[i]);
  }

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: list_get should return NULL for out of bounds index
 */
bool test_list_get_out_of_bounds(void) {
  // Arrange: Create a list with elements
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    list_insert_back(list, &values[i]);
  }

  // Act & Assert: Try to get out of bounds indices
  ASSERT_NULL(list_get(list, -1));
  ASSERT_NULL(list_get(list, 3));
  ASSERT_NULL(list_get(list, 100));

  // Cleanup
  list_destroy(list);

  return true;
}

// ============================================================================
// Tests for list_get_first() and list_get_last()
// ============================================================================

/**
 * Test: list_get_first should return first element
 */
bool test_list_get_first_basic(void) {
  // Arrange: Create a list and add elements
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    list_insert_back(list, &values[i]);
  }

  // Act: Get first element
  int *result = (int *)list_get_first(list);

  // Assert: Should return first element
  ASSERT_NOT_NULL(result);
  ASSERT_EQUAL(*result, 10);

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: list_get_last should return last element
 */
bool test_list_get_last_basic(void) {
  // Arrange: Create a list and add elements
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    list_insert_back(list, &values[i]);
  }

  // Act: Get last element
  int *result = (int *)list_get_last(list);

  // Assert: Should return last element
  ASSERT_NOT_NULL(result);
  ASSERT_EQUAL(*result, 30);

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: list_get_first and list_get_last on empty list
 */
bool test_list_get_first_last_empty(void) {
  // Arrange: Create an empty list
  List list = list_create();
  ASSERT_NOT_NULL(list);

  // Act & Assert: Should return NULL
  ASSERT_NULL(list_get_first(list));
  ASSERT_NULL(list_get_last(list));

  // Cleanup
  list_destroy(list);

  return true;
}

// ============================================================================
// Tests for list_remove()
// ============================================================================

/**
 * Test: list_remove should remove element from list
 */
bool test_list_remove_basic(void) {
  // Arrange: Create a list and add elements
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    list_insert_back(list, &values[i]);
  }

  // Act: Remove middle element
  bool result = list_remove(list, &values[1]);

  // Assert: Should succeed and reduce size
  ASSERT_TRUE(result);
  ASSERT_EQUAL(list_size(list), 2);

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: list_remove should return false for non-existent element
 */
bool test_list_remove_not_found(void) {
  // Arrange: Create a list and add elements
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30};
  int other = 99;

  for (int i = 0; i < 3; i++) {
    list_insert_back(list, &values[i]);
  }

  // Act: Try to remove element not in list
  bool result = list_remove(list, &other);

  // Assert: Should fail and size unchanged
  ASSERT_FALSE(result);
  ASSERT_EQUAL(list_size(list), 3);

  // Cleanup
  list_destroy(list);

  return true;
}

// ============================================================================
// Tests for list_size()
// ============================================================================

/**
 * Test: list_size should return 0 for new list
 */
bool test_list_size_new_list(void) {
  // Arrange: Create a list
  List list = list_create();
  ASSERT_NOT_NULL(list);

  // Act & Assert: Size should be 0
  ASSERT_EQUAL(list_size(list), 0);

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: list_size should track insert operations
 */
bool test_list_size_after_insert(void) {
  // Arrange: Create a list
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30, 40, 50};

  // Act & Assert: Size should increase with each insert
  for (int i = 0; i < 5; i++) {
    list_insert_back(list, &values[i]);
    ASSERT_EQUAL(list_size(list), i + 1);
  }

  // Cleanup
  list_destroy(list);

  return true;
}

// ============================================================================
// Tests for list_is_empty()
// ============================================================================

/**
 * Test: list_is_empty should return true for new list
 */
bool test_list_is_empty_new_list(void) {
  // Arrange: Create a list
  List list = list_create();
  ASSERT_NOT_NULL(list);

  // Act & Assert: Should be empty
  ASSERT_TRUE(list_is_empty(list));

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: list_is_empty should return false after insert
 */
bool test_list_is_empty_after_insert(void) {
  // Arrange: Create a list and add an element
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int value = 42;
  list_insert_back(list, &value);

  // Act & Assert: Should not be empty
  ASSERT_FALSE(list_is_empty(list));

  // Cleanup
  list_destroy(list);

  return true;
}

// ============================================================================
// Tests for list_clear()
// ============================================================================

/**
 * Test: list_clear should remove all elements
 */
bool test_list_clear_basic(void) {
  // Arrange: Create a list and add elements
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30, 40, 50};

  for (int i = 0; i < 5; i++) {
    list_insert_back(list, &values[i]);
  }

  // Act: Clear the list
  list_clear(list);

  // Assert: List should be empty
  ASSERT_TRUE(list_is_empty(list));
  ASSERT_EQUAL(list_size(list), 0);

  // Cleanup
  list_destroy(list);

  return true;
}

/**
 * Test: list_clear should allow reuse after clearing
 */
bool test_list_clear_reuse(void) {
  // Arrange: Create a list, add elements, and clear
  List list = list_create();
  ASSERT_NOT_NULL(list);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    list_insert_back(list, &values[i]);
  }
  list_clear(list);

  // Act: Add new elements after clearing
  int new_value = 99;
  bool result = list_insert_back(list, &new_value);

  // Assert: Should work correctly
  ASSERT_TRUE(result);
  ASSERT_EQUAL(list_size(list), 1);
  int *retrieved = (int *)list_get_first(list);
  ASSERT_NOT_NULL(retrieved);
  ASSERT_EQUAL(*retrieved, 99);

  // Cleanup
  list_destroy(list);

  return true;
}

// ============================================================================
// Tests for list_destroy()
// ============================================================================

/**
 * Test: list_destroy should handle NULL gracefully
 */
bool test_list_destroy_null(void) {
  // Act & Assert: Should not crash
  list_destroy(NULL);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for list_create
  test_print_section("Testing list_create()");
  test_register("test_list_create_basic", test_list_create_basic);
  test_register("test_list_create_is_empty", test_list_create_is_empty);

  // Register tests for list_insert_back
  test_print_section("Testing list_insert_back()");
  test_register("test_list_insert_back_single_element",
                test_list_insert_back_single_element);
  test_register("test_list_insert_back_multiple_elements",
                test_list_insert_back_multiple_elements);

  // Register tests for list_insert_front
  test_print_section("Testing list_insert_front()");
  test_register("test_list_insert_front_single_element",
                test_list_insert_front_single_element);
  test_register("test_list_insert_front_multiple_elements",
                test_list_insert_front_multiple_elements);

  // Register tests for list_get
  test_print_section("Testing list_get()");
  test_register("test_list_get_basic", test_list_get_basic);
  test_register("test_list_get_out_of_bounds", test_list_get_out_of_bounds);

  // Register tests for list_get_first and list_get_last
  test_print_section("Testing list_get_first() and list_get_last()");
  test_register("test_list_get_first_basic", test_list_get_first_basic);
  test_register("test_list_get_last_basic", test_list_get_last_basic);
  test_register("test_list_get_first_last_empty",
                test_list_get_first_last_empty);

  // Register tests for list_remove
  test_print_section("Testing list_remove()");
  test_register("test_list_remove_basic", test_list_remove_basic);
  test_register("test_list_remove_not_found", test_list_remove_not_found);

  // Register tests for list_size
  test_print_section("Testing list_size()");
  test_register("test_list_size_new_list", test_list_size_new_list);
  test_register("test_list_size_after_insert", test_list_size_after_insert);

  // Register tests for list_is_empty
  test_print_section("Testing list_is_empty()");
  test_register("test_list_is_empty_new_list", test_list_is_empty_new_list);
  test_register("test_list_is_empty_after_insert",
                test_list_is_empty_after_insert);

  // Register tests for list_clear
  test_print_section("Testing list_clear()");
  test_register("test_list_clear_basic", test_list_clear_basic);
  test_register("test_list_clear_reuse", test_list_clear_reuse);

  // Register tests for list_destroy
  test_print_section("Testing list_destroy()");
  test_register("test_list_destroy_null", test_list_destroy_null);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
