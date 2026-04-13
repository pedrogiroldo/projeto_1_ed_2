/**
 * @file queue.spec.c
 * @brief Unit tests for queue module
 *
 * Comprehensive unit tests for the queue ADT functions defined in queue.h
 */

#include "./queue.h"
#include "../../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for queue_create()
// ============================================================================

/**
 * Test: queue_create should return a valid queue instance
 */
bool test_queue_create_basic(void) {
  // Act: Create a queue
  Queue queue = queue_create();

  // Assert: Queue should be created successfully
  ASSERT_NOT_NULL(queue);

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: newly created queue should be empty
 */
bool test_queue_create_is_empty(void) {
  // Act: Create a queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);

  // Assert: Queue should be empty
  ASSERT_TRUE(queue_is_empty(queue));
  ASSERT_EQUAL(queue_size(queue), 0);

  // Cleanup
  queue_destroy(queue);

  return true;
}

// ============================================================================
// Tests for queue_enqueue()
// ============================================================================

/**
 * Test: queue_enqueue should add element to queue
 */
bool test_queue_enqueue_single_element(void) {
  // Arrange: Create a queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int value = 42;

  // Act: Enqueue an element
  bool result = queue_enqueue(queue, &value);

  // Assert: Enqueue should succeed and queue should not be empty
  ASSERT_TRUE(result);
  ASSERT_FALSE(queue_is_empty(queue));
  ASSERT_EQUAL(queue_size(queue), 1);

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_enqueue should add multiple elements
 */
bool test_queue_enqueue_multiple_elements(void) {
  // Arrange: Create a queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int values[] = {10, 20, 30, 40, 50};
  int count = 5;

  // Act: Enqueue multiple elements
  for (int i = 0; i < count; i++) {
    bool result = queue_enqueue(queue, &values[i]);
    ASSERT_TRUE(result);
  }

  // Assert: Queue should have correct size
  ASSERT_EQUAL(queue_size(queue), count);
  ASSERT_FALSE(queue_is_empty(queue));

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_enqueue should handle NULL data
 */
bool test_queue_enqueue_null_data(void) {
  // Arrange: Create a queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);

  // Act: Enqueue NULL
  bool result = queue_enqueue(queue, NULL);

  // Assert: Should succeed (NULL is valid data)
  ASSERT_TRUE(result);
  ASSERT_EQUAL(queue_size(queue), 1);

  // Cleanup
  queue_destroy(queue);

  return true;
}

// ============================================================================
// Tests for queue_dequeue()
// ============================================================================

/**
 * Test: queue_dequeue should remove and return front element
 */
bool test_queue_dequeue_single_element(void) {
  // Arrange: Create a queue and add an element
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int value = 42;
  queue_enqueue(queue, &value);

  // Act: Dequeue the element
  int *result = (int *)queue_dequeue(queue);

  // Assert: Should return the correct element
  ASSERT_NOT_NULL(result);
  ASSERT_EQUAL(*result, 42);
  ASSERT_TRUE(queue_is_empty(queue));
  ASSERT_EQUAL(queue_size(queue), 0);

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_dequeue should maintain FIFO order
 */
bool test_queue_dequeue_fifo_order(void) {
  // Arrange: Create a queue and add multiple elements
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int values[] = {10, 20, 30, 40, 50};
  int count = 5;

  for (int i = 0; i < count; i++) {
    queue_enqueue(queue, &values[i]);
  }

  // Act & Assert: Dequeue elements and verify FIFO order
  for (int i = 0; i < count; i++) {
    int *result = (int *)queue_dequeue(queue);
    ASSERT_NOT_NULL(result);
    ASSERT_EQUAL(*result, values[i]);
  }

  // Queue should be empty now
  ASSERT_TRUE(queue_is_empty(queue));

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_dequeue should return NULL on empty queue
 */
bool test_queue_dequeue_empty_queue(void) {
  // Arrange: Create an empty queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);

  // Act: Try to dequeue from empty queue
  void *result = queue_dequeue(queue);

  // Assert: Should return NULL
  ASSERT_NULL(result);

  // Cleanup
  queue_destroy(queue);

  return true;
}

// ============================================================================
// Tests for queue_peek()
// ============================================================================

/**
 * Test: queue_peek should return front element without removing it
 */
bool test_queue_peek_basic(void) {
  // Arrange: Create a queue and add elements
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    queue_enqueue(queue, &values[i]);
  }

  // Act: Peek at front element
  int *result = (int *)queue_peek(queue);

  // Assert: Should return first element without removing it
  ASSERT_NOT_NULL(result);
  ASSERT_EQUAL(*result, 10);
  ASSERT_EQUAL(queue_size(queue), 3); // Size should not change

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_peek should return NULL on empty queue
 */
bool test_queue_peek_empty_queue(void) {
  // Arrange: Create an empty queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);

  // Act: Try to peek at empty queue
  void *result = queue_peek(queue);

  // Assert: Should return NULL
  ASSERT_NULL(result);

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_peek should not modify queue
 */
bool test_queue_peek_no_modification(void) {
  // Arrange: Create a queue and add an element
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int value = 42;
  queue_enqueue(queue, &value);

  // Act: Peek multiple times
  int *result1 = (int *)queue_peek(queue);
  int *result2 = (int *)queue_peek(queue);
  int *result3 = (int *)queue_peek(queue);

  // Assert: All peeks should return the same element
  ASSERT_NOT_NULL(result1);
  ASSERT_NOT_NULL(result2);
  ASSERT_NOT_NULL(result3);
  ASSERT_EQUAL(*result1, 42);
  ASSERT_EQUAL(*result2, 42);
  ASSERT_EQUAL(*result3, 42);
  ASSERT_EQUAL(queue_size(queue), 1);

  // Cleanup
  queue_destroy(queue);

  return true;
}

// ============================================================================
// Tests for queue_is_empty()
// ============================================================================

/**
 * Test: queue_is_empty should return true for new queue
 */
bool test_queue_is_empty_new_queue(void) {
  // Arrange: Create a queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);

  // Act & Assert: Should be empty
  ASSERT_TRUE(queue_is_empty(queue));

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_is_empty should return false after enqueue
 */
bool test_queue_is_empty_after_enqueue(void) {
  // Arrange: Create a queue and add an element
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int value = 42;
  queue_enqueue(queue, &value);

  // Act & Assert: Should not be empty
  ASSERT_FALSE(queue_is_empty(queue));

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_is_empty should return true after dequeuing all elements
 */
bool test_queue_is_empty_after_dequeue_all(void) {
  // Arrange: Create a queue, add and remove elements
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    queue_enqueue(queue, &values[i]);
  }

  // Act: Dequeue all elements
  for (int i = 0; i < 3; i++) {
    queue_dequeue(queue);
  }

  // Assert: Should be empty
  ASSERT_TRUE(queue_is_empty(queue));

  // Cleanup
  queue_destroy(queue);

  return true;
}

// ============================================================================
// Tests for queue_size()
// ============================================================================

/**
 * Test: queue_size should return 0 for new queue
 */
bool test_queue_size_new_queue(void) {
  // Arrange: Create a queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);

  // Act & Assert: Size should be 0
  ASSERT_EQUAL(queue_size(queue), 0);

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_size should track enqueue operations
 */
bool test_queue_size_after_enqueue(void) {
  // Arrange: Create a queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int values[] = {10, 20, 30, 40, 50};

  // Act & Assert: Size should increase with each enqueue
  for (int i = 0; i < 5; i++) {
    queue_enqueue(queue, &values[i]);
    ASSERT_EQUAL(queue_size(queue), i + 1);
  }

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_size should track dequeue operations
 */
bool test_queue_size_after_dequeue(void) {
  // Arrange: Create a queue and add elements
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int values[] = {10, 20, 30, 40, 50};

  for (int i = 0; i < 5; i++) {
    queue_enqueue(queue, &values[i]);
  }

  // Act & Assert: Size should decrease with each dequeue
  for (int i = 0; i < 5; i++) {
    queue_dequeue(queue);
    ASSERT_EQUAL(queue_size(queue), 4 - i);
  }

  // Cleanup
  queue_destroy(queue);

  return true;
}

// ============================================================================
// Tests for queue_clear()
// ============================================================================

/**
 * Test: queue_clear should remove all elements
 */
bool test_queue_clear_basic(void) {
  // Arrange: Create a queue and add elements
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int values[] = {10, 20, 30, 40, 50};

  for (int i = 0; i < 5; i++) {
    queue_enqueue(queue, &values[i]);
  }

  // Act: Clear the queue
  queue_clear(queue);

  // Assert: Queue should be empty
  ASSERT_TRUE(queue_is_empty(queue));
  ASSERT_EQUAL(queue_size(queue), 0);

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_clear should allow reuse after clearing
 */
bool test_queue_clear_reuse(void) {
  // Arrange: Create a queue, add elements, and clear
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int values[] = {10, 20, 30};

  for (int i = 0; i < 3; i++) {
    queue_enqueue(queue, &values[i]);
  }
  queue_clear(queue);

  // Act: Add new elements after clearing
  int new_value = 99;
  bool result = queue_enqueue(queue, &new_value);

  // Assert: Should work correctly
  ASSERT_TRUE(result);
  ASSERT_EQUAL(queue_size(queue), 1);
  int *retrieved = (int *)queue_peek(queue);
  ASSERT_NOT_NULL(retrieved);
  ASSERT_EQUAL(*retrieved, 99);

  // Cleanup
  queue_destroy(queue);

  return true;
}

/**
 * Test: queue_clear on empty queue should not cause issues
 */
bool test_queue_clear_empty_queue(void) {
  // Arrange: Create an empty queue
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);

  // Act: Clear empty queue
  queue_clear(queue);

  // Assert: Should still be empty
  ASSERT_TRUE(queue_is_empty(queue));
  ASSERT_EQUAL(queue_size(queue), 0);

  // Cleanup
  queue_destroy(queue);

  return true;
}

// ============================================================================
// Tests for queue_destroy()
// ============================================================================

/**
 * Test: queue_destroy should handle NULL gracefully
 */
bool test_queue_destroy_null(void) {
  // Act & Assert: Should not crash
  queue_destroy(NULL);

  return true;
}

/**
 * Test: queue_destroy should free all resources
 */
bool test_queue_destroy_cleanup(void) {
  // Arrange: Create a queue and add elements
  Queue queue = queue_create();
  ASSERT_NOT_NULL(queue);
  int values[] = {10, 20, 30, 40, 50};

  for (int i = 0; i < 5; i++) {
    queue_enqueue(queue, &values[i]);
  }

  // Act: Destroy the queue (should not crash)
  queue_destroy(queue);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for queue_create
  test_print_section("Testing queue_create()");
  test_register("test_queue_create_basic", test_queue_create_basic);
  test_register("test_queue_create_is_empty", test_queue_create_is_empty);

  // Register tests for queue_enqueue
  test_print_section("Testing queue_enqueue()");
  test_register("test_queue_enqueue_single_element",
                test_queue_enqueue_single_element);
  test_register("test_queue_enqueue_multiple_elements",
                test_queue_enqueue_multiple_elements);
  test_register("test_queue_enqueue_null_data", test_queue_enqueue_null_data);

  // Register tests for queue_dequeue
  test_print_section("Testing queue_dequeue()");
  test_register("test_queue_dequeue_single_element",
                test_queue_dequeue_single_element);
  test_register("test_queue_dequeue_fifo_order", test_queue_dequeue_fifo_order);
  test_register("test_queue_dequeue_empty_queue",
                test_queue_dequeue_empty_queue);

  // Register tests for queue_peek
  test_print_section("Testing queue_peek()");
  test_register("test_queue_peek_basic", test_queue_peek_basic);
  test_register("test_queue_peek_empty_queue", test_queue_peek_empty_queue);
  test_register("test_queue_peek_no_modification",
                test_queue_peek_no_modification);

  // Register tests for queue_is_empty
  test_print_section("Testing queue_is_empty()");
  test_register("test_queue_is_empty_new_queue", test_queue_is_empty_new_queue);
  test_register("test_queue_is_empty_after_enqueue",
                test_queue_is_empty_after_enqueue);
  test_register("test_queue_is_empty_after_dequeue_all",
                test_queue_is_empty_after_dequeue_all);

  // Register tests for queue_size
  test_print_section("Testing queue_size()");
  test_register("test_queue_size_new_queue", test_queue_size_new_queue);
  test_register("test_queue_size_after_enqueue", test_queue_size_after_enqueue);
  test_register("test_queue_size_after_dequeue", test_queue_size_after_dequeue);

  // Register tests for queue_clear
  test_print_section("Testing queue_clear()");
  test_register("test_queue_clear_basic", test_queue_clear_basic);
  test_register("test_queue_clear_reuse", test_queue_clear_reuse);
  test_register("test_queue_clear_empty_queue", test_queue_clear_empty_queue);

  // Register tests for queue_destroy
  test_print_section("Testing queue_destroy()");
  test_register("test_queue_destroy_null", test_queue_destroy_null);
  test_register("test_queue_destroy_cleanup", test_queue_destroy_cleanup);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
