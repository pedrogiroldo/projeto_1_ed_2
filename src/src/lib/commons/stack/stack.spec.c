/**
 * @file stack.spec.c
 * @brief Unit tests for stack module
 *
 * Comprehensive unit tests for the stack ADT functions defined in stack.h
 */

#include "stack.h"
#include "../../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for stack_create()
// ============================================================================

/**
 * Test: stack_create should return a valid stack instance
 */
bool test_stack_create_basic(void) {
  // Act: Create a stack
  Stack stack = stack_create();

  // Assert: Stack should be created successfully
  ASSERT_NOT_NULL(stack);

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: newly created stack should be empty
 */
bool test_stack_create_is_empty(void) {
  // Act: Create a stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);

  // Assert: Stack should be empty
  ASSERT_TRUE(stack_is_empty(stack));
  ASSERT_EQUAL(stack_size(stack), 0);

  // Cleanup
  stack_destroy(stack);

  return true;
}

// ============================================================================
// Tests for stack_push()
// ============================================================================

/**
 * Test: stack_push should add element to stack
 */
bool test_stack_push_single_element(void) {
  // Arrange: Create a stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int value = 42;

  // Act: Push an element
  bool result = stack_push(stack, &value);

  // Assert: Push should succeed and stack should not be empty
  ASSERT_TRUE(result);
  ASSERT_FALSE(stack_is_empty(stack));
  ASSERT_EQUAL(stack_size(stack), 1);

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_push should add multiple elements
 */
bool test_stack_push_multiple_elements(void) {
  // Arrange: Create a stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30, 40, 50};
  int count = 5;

  // Act: Push multiple elements
  for (int i = 0; i < count; i++) {
    bool result = stack_push(stack, &values[i]);
    ASSERT_TRUE(result);
  }

  // Assert: Stack should have correct size
  ASSERT_EQUAL(stack_size(stack), count);
  ASSERT_FALSE(stack_is_empty(stack));

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_push should handle NULL data
 */
bool test_stack_push_null_data(void) {
  // Arrange: Create a stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);

  // Act: Push NULL
  bool result = stack_push(stack, NULL);

  // Assert: Should succeed (NULL is valid data)
  ASSERT_TRUE(result);
  ASSERT_EQUAL(stack_size(stack), 1);

  // Cleanup
  stack_destroy(stack);

  return true;
}

// ============================================================================
// Tests for stack_pop()
// ============================================================================

/**
 * Test: stack_pop should remove and return top element
 */
bool test_stack_pop_single_element(void) {
  // Arrange: Create a stack and add an element
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int value = 42;
  stack_push(stack, &value);

  // Act: Pop the element
  int *result = (int *)stack_pop(stack);

  // Assert: Should return the correct element
  ASSERT_NOT_NULL(result);
  ASSERT_EQUAL(*result, 42);
  ASSERT_TRUE(stack_is_empty(stack));
  ASSERT_EQUAL(stack_size(stack), 0);

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_pop should maintain LIFO order
 */
bool test_stack_pop_lifo_order(void) {
  // Arrange: Create a stack and add multiple elements
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30, 40, 50};
  int count = 5;

  for (int i = 0; i < count; i++) {
    stack_push(stack, &values[i]);
  }

  // Act & Assert: Pop elements and verify LIFO order
  for (int i = count - 1; i >= 0; i--) {
    int *result = (int *)stack_pop(stack);
    ASSERT_NOT_NULL(result);
    ASSERT_EQUAL(*result, values[i]);
  }

  // Stack should be empty now
  ASSERT_TRUE(stack_is_empty(stack));

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_pop should return NULL on empty stack
 */
bool test_stack_pop_empty_stack(void) {
  // Arrange: Create an empty stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);

  // Act: Try to pop from empty stack
  void *result = stack_pop(stack);

  // Assert: Should return NULL
  ASSERT_NULL(result);

  // Cleanup
  stack_destroy(stack);

  return true;
}

// ============================================================================
// Tests for stack_peek()
// ============================================================================

/**
 * Test: stack_peek should return top element without removing it
 */
bool test_stack_peek_basic(void) {
  // Arrange: Create a stack and add elements
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    stack_push(stack, &values[i]);
  }

  // Act: Peek at top element
  int *result = (int *)stack_peek(stack);

  // Assert: Should return last pushed element without removing it
  ASSERT_NOT_NULL(result);
  ASSERT_EQUAL(*result, 30);
  ASSERT_EQUAL(stack_size(stack), 3); // Size should not change

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_peek should return NULL on empty stack
 */
bool test_stack_peek_empty_stack(void) {
  // Arrange: Create an empty stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);

  // Act: Try to peek at empty stack
  void *result = stack_peek(stack);

  // Assert: Should return NULL
  ASSERT_NULL(result);

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_peek should not modify stack
 */
bool test_stack_peek_no_modification(void) {
  // Arrange: Create a stack and add an element
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int value = 42;
  stack_push(stack, &value);

  // Act: Peek multiple times
  int *result1 = (int *)stack_peek(stack);
  int *result2 = (int *)stack_peek(stack);
  int *result3 = (int *)stack_peek(stack);

  // Assert: All peeks should return the same element
  ASSERT_NOT_NULL(result1);
  ASSERT_NOT_NULL(result2);
  ASSERT_NOT_NULL(result3);
  ASSERT_EQUAL(*result1, 42);
  ASSERT_EQUAL(*result2, 42);
  ASSERT_EQUAL(*result3, 42);
  ASSERT_EQUAL(stack_size(stack), 1);

  // Cleanup
  stack_destroy(stack);

  return true;
}

// ============================================================================
// Tests for stack_peek_at()
// ============================================================================

/**
 * Test: stack_peek_at should return element at specified index
 */
bool test_stack_peek_at_basic(void) {
  // Arrange: Create a stack and add elements
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30, 40, 50};

  for (int i = 0; i < 5; i++) {
    stack_push(stack, &values[i]);
  }

  // Act & Assert: Peek at different indices
  int *result0 = (int *)stack_peek_at(stack, 0); // Top element (50)
  ASSERT_NOT_NULL(result0);
  ASSERT_EQUAL(*result0, 50);

  int *result2 = (int *)stack_peek_at(stack, 2); // Third from top (30)
  ASSERT_NOT_NULL(result2);
  ASSERT_EQUAL(*result2, 30);

  int *result4 = (int *)stack_peek_at(stack, 4); // Bottom element (10)
  ASSERT_NOT_NULL(result4);
  ASSERT_EQUAL(*result4, 10);

  // Size should not change
  ASSERT_EQUAL(stack_size(stack), 5);

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_peek_at should return NULL for invalid index
 */
bool test_stack_peek_at_invalid_index(void) {
  // Arrange: Create a stack with 3 elements
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    stack_push(stack, &values[i]);
  }

  // Act: Try to peek at invalid indices
  void *result_negative = stack_peek_at(stack, -1);
  void *result_too_large = stack_peek_at(stack, 5);

  // Assert: Should return NULL
  ASSERT_NULL(result_negative);
  ASSERT_NULL(result_too_large);

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_peek_at should return NULL on empty stack
 */
bool test_stack_peek_at_empty_stack(void) {
  // Arrange: Create an empty stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);

  // Act: Try to peek at index 0
  void *result = stack_peek_at(stack, 0);

  // Assert: Should return NULL
  ASSERT_NULL(result);

  // Cleanup
  stack_destroy(stack);

  return true;
}

// ============================================================================
// Tests for stack_is_empty()
// ============================================================================

/**
 * Test: stack_is_empty should return true for new stack
 */
bool test_stack_is_empty_new_stack(void) {
  // Arrange: Create a stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);

  // Act & Assert: Should be empty
  ASSERT_TRUE(stack_is_empty(stack));

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_is_empty should return false after push
 */
bool test_stack_is_empty_after_push(void) {
  // Arrange: Create a stack and add an element
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int value = 42;
  stack_push(stack, &value);

  // Act & Assert: Should not be empty
  ASSERT_FALSE(stack_is_empty(stack));

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_is_empty should return true after popping all elements
 */
bool test_stack_is_empty_after_pop_all(void) {
  // Arrange: Create a stack, add and remove elements
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    stack_push(stack, &values[i]);
  }

  // Act: Pop all elements
  for (int i = 0; i < 3; i++) {
    stack_pop(stack);
  }

  // Assert: Should be empty
  ASSERT_TRUE(stack_is_empty(stack));

  // Cleanup
  stack_destroy(stack);

  return true;
}

// ============================================================================
// Tests for stack_size()
// ============================================================================

/**
 * Test: stack_size should return 0 for new stack
 */
bool test_stack_size_new_stack(void) {
  // Arrange: Create a stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);

  // Act & Assert: Size should be 0
  ASSERT_EQUAL(stack_size(stack), 0);

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_size should track push operations
 */
bool test_stack_size_after_push(void) {
  // Arrange: Create a stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30, 40, 50};

  // Act & Assert: Size should increase with each push
  for (int i = 0; i < 5; i++) {
    stack_push(stack, &values[i]);
    ASSERT_EQUAL(stack_size(stack), i + 1);
  }

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_size should track pop operations
 */
bool test_stack_size_after_pop(void) {
  // Arrange: Create a stack and add elements
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30, 40, 50};

  for (int i = 0; i < 5; i++) {
    stack_push(stack, &values[i]);
  }

  // Act & Assert: Size should decrease with each pop
  for (int i = 0; i < 5; i++) {
    stack_pop(stack);
    ASSERT_EQUAL(stack_size(stack), 4 - i);
  }

  // Cleanup
  stack_destroy(stack);

  return true;
}

// ============================================================================
// Tests for stack_clear()
// ============================================================================

/**
 * Test: stack_clear should remove all elements
 */
bool test_stack_clear_basic(void) {
  // Arrange: Create a stack and add elements
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30, 40, 50};

  for (int i = 0; i < 5; i++) {
    stack_push(stack, &values[i]);
  }

  // Act: Clear the stack
  stack_clear(stack);

  // Assert: Stack should be empty
  ASSERT_TRUE(stack_is_empty(stack));
  ASSERT_EQUAL(stack_size(stack), 0);

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_clear should allow reuse after clearing
 */
bool test_stack_clear_reuse(void) {
  // Arrange: Create a stack, add elements, and clear
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    stack_push(stack, &values[i]);
  }
  stack_clear(stack);

  // Act: Add new elements after clearing
  int new_value = 99;
  bool result = stack_push(stack, &new_value);

  // Assert: Should work correctly
  ASSERT_TRUE(result);
  ASSERT_EQUAL(stack_size(stack), 1);
  int *retrieved = (int *)stack_peek(stack);
  ASSERT_NOT_NULL(retrieved);
  ASSERT_EQUAL(*retrieved, 99);

  // Cleanup
  stack_destroy(stack);

  return true;
}

/**
 * Test: stack_clear on empty stack should not cause issues
 */
bool test_stack_clear_empty_stack(void) {
  // Arrange: Create an empty stack
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);

  // Act: Clear empty stack
  stack_clear(stack);

  // Assert: Should still be empty
  ASSERT_TRUE(stack_is_empty(stack));
  ASSERT_EQUAL(stack_size(stack), 0);

  // Cleanup
  stack_destroy(stack);

  return true;
}

// ============================================================================
// Tests for stack_destroy()
// ============================================================================

/**
 * Test: stack_destroy should handle NULL gracefully
 */
bool test_stack_destroy_null(void) {
  // Act & Assert: Should not crash
  stack_destroy(NULL);

  return true;
}

/**
 * Test: stack_destroy should free all resources
 */
bool test_stack_destroy_cleanup(void) {
  // Arrange: Create a stack and add elements
  Stack stack = stack_create();
  ASSERT_NOT_NULL(stack);
  int values[] = {10, 20, 30, 40, 50};

  for (int i = 0; i < 5; i++) {
    stack_push(stack, &values[i]);
  }

  // Act: Destroy the stack (should not crash)
  stack_destroy(stack);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for stack_create
  test_print_section("Testing stack_create()");
  test_register("test_stack_create_basic", test_stack_create_basic);
  test_register("test_stack_create_is_empty", test_stack_create_is_empty);

  // Register tests for stack_push
  test_print_section("Testing stack_push()");
  test_register("test_stack_push_single_element",
                test_stack_push_single_element);
  test_register("test_stack_push_multiple_elements",
                test_stack_push_multiple_elements);
  test_register("test_stack_push_null_data", test_stack_push_null_data);

  // Register tests for stack_pop
  test_print_section("Testing stack_pop()");
  test_register("test_stack_pop_single_element", test_stack_pop_single_element);
  test_register("test_stack_pop_lifo_order", test_stack_pop_lifo_order);
  test_register("test_stack_pop_empty_stack", test_stack_pop_empty_stack);

  // Register tests for stack_peek
  test_print_section("Testing stack_peek()");
  test_register("test_stack_peek_basic", test_stack_peek_basic);
  test_register("test_stack_peek_empty_stack", test_stack_peek_empty_stack);
  test_register("test_stack_peek_no_modification",
                test_stack_peek_no_modification);

  // Register tests for stack_peek_at
  test_print_section("Testing stack_peek_at()");
  test_register("test_stack_peek_at_basic", test_stack_peek_at_basic);
  test_register("test_stack_peek_at_invalid_index",
                test_stack_peek_at_invalid_index);
  test_register("test_stack_peek_at_empty_stack",
                test_stack_peek_at_empty_stack);

  // Register tests for stack_is_empty
  test_print_section("Testing stack_is_empty()");
  test_register("test_stack_is_empty_new_stack", test_stack_is_empty_new_stack);
  test_register("test_stack_is_empty_after_push",
                test_stack_is_empty_after_push);
  test_register("test_stack_is_empty_after_pop_all",
                test_stack_is_empty_after_pop_all);

  // Register tests for stack_size
  test_print_section("Testing stack_size()");
  test_register("test_stack_size_new_stack", test_stack_size_new_stack);
  test_register("test_stack_size_after_push", test_stack_size_after_push);
  test_register("test_stack_size_after_pop", test_stack_size_after_pop);

  // Register tests for stack_clear
  test_print_section("Testing stack_clear()");
  test_register("test_stack_clear_basic", test_stack_clear_basic);
  test_register("test_stack_clear_reuse", test_stack_clear_reuse);
  test_register("test_stack_clear_empty_stack", test_stack_clear_empty_stack);

  // Register tests for stack_destroy
  test_print_section("Testing stack_destroy()");
  test_register("test_stack_destroy_null", test_stack_destroy_null);
  test_register("test_stack_destroy_cleanup", test_stack_destroy_cleanup);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
