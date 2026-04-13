/**
 * @file file_reader.spec.c
 * @brief Unit tests for file_reader module
 *
 * Comprehensive unit tests for the file reading and data management
 * functions defined in file_reader.h
 */

#include "file_reader.h"
#include "../commons/queue/queue.h"
#include "../test_framework/test_framework.h"

#include <stdio.h>
#include <string.h>

// ============================================================================
// Test Helper Functions
// ============================================================================

/**
 * Creates a temporary test file with specified content
 * @param filename Name of the file to create
 * @param lines Array of strings to write (NULL-terminated)
 * @return true if successful, false otherwise
 */
static bool create_test_file(const char *filename, const char **lines) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    return false;
  }

  for (int i = 0; lines[i] != NULL; i++) {
    fprintf(file, "%s\n", lines[i]);
  }

  fclose(file);
  return true;
}

/**
 * Removes a test file
 * @param filename Name of the file to remove
 */
static void remove_test_file(const char *filename) { remove(filename); }

// ============================================================================
// Tests for file_data_create()
// ============================================================================

/**
 * Test: file_data_create should successfully read a valid file
 */
bool test_file_data_create_valid_file(void) {
  // Arrange: Create a test file
  const char *test_lines[] = {"c 1 10.5 20.3 5.0 red",
                              "r 2 15.0 25.0 10.0 8.0 blue",
                              "t 3 30.0 40.0 Hello", NULL};
  const char *filepath = "/tmp/test_file_reader_valid.geo";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData
  FileData file_data = file_data_create(filepath);

  // Assert: FileData should be created successfully
  ASSERT_NOT_NULL(file_data);

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

/**
 * Test: file_data_create should return NULL for non-existent file
 */
bool test_file_data_create_nonexistent_file(void) {
  // Arrange: Use a path that doesn't exist
  const char *filepath = "/tmp/nonexistent_file_12345.geo";

  // Act: Try to create FileData
  FileData file_data = file_data_create(filepath);

  // Assert: Should return NULL
  ASSERT_NULL(file_data);

  return true;
}

/**
 * Test: file_data_create should handle empty file
 */
bool test_file_data_create_empty_file(void) {
  // Arrange: Create an empty test file
  const char *test_lines[] = {NULL};
  const char *filepath = "/tmp/test_file_reader_empty.geo";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData
  FileData file_data = file_data_create(filepath);

  // Assert: FileData should be created successfully
  ASSERT_NOT_NULL(file_data);

  // Verify queue is empty
  Queue lines = get_file_lines_queue(file_data);
  ASSERT_NOT_NULL(lines);
  ASSERT_TRUE(queue_is_empty(lines));

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

/**
 * Test: file_data_create should handle file with single line
 */
bool test_file_data_create_single_line(void) {
  // Arrange: Create a test file with one line
  const char *test_lines[] = {"c 1 10.5 20.3 5.0 red", NULL};
  const char *filepath = "/tmp/test_file_reader_single.geo";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData
  FileData file_data = file_data_create(filepath);

  // Assert: FileData should be created successfully
  ASSERT_NOT_NULL(file_data);

  // Verify queue has one line
  Queue lines = get_file_lines_queue(file_data);
  ASSERT_NOT_NULL(lines);
  ASSERT_FALSE(queue_is_empty(lines));

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

/**
 * Test: file_data_create should handle file with multiple lines
 */
bool test_file_data_create_multiple_lines(void) {
  // Arrange: Create a test file with multiple lines
  const char *test_lines[] = {
      "c 1 10.5 20.3 5.0 red",     "r 2 15.0 25.0 10.0 8.0 blue",
      "t 3 30.0 40.0 Hello",       "l 4 0.0 0.0 100.0 100.0 green",
      "c 5 50.0 50.0 25.0 yellow", NULL};
  const char *filepath = "/tmp/test_file_reader_multiple.geo";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData
  FileData file_data = file_data_create(filepath);

  // Assert: FileData should be created successfully
  ASSERT_NOT_NULL(file_data);

  // Verify queue has correct number of lines
  Queue lines = get_file_lines_queue(file_data);
  ASSERT_NOT_NULL(lines);

  // Count lines by dequeuing
  int line_count = 0;
  while (!queue_is_empty(lines)) {
    void *line = queue_dequeue(lines);
    ASSERT_NOT_NULL(line);
    line_count++;
  }
  ASSERT_EQUAL(line_count, 5);

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

// ============================================================================
// Tests for get_file_path()
// ============================================================================

/**
 * Test: get_file_path should return the correct file path
 */
bool test_get_file_path_basic(void) {
  // Arrange: Create a test file
  const char *test_lines[] = {"c 1 10.5 20.3 5.0 red", NULL};
  const char *filepath = "/tmp/test_file_reader_path.geo";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData and get path
  FileData file_data = file_data_create(filepath);
  ASSERT_NOT_NULL(file_data);
  const char *result_path = get_file_path(file_data);

  // Assert: Path should match
  ASSERT_NOT_NULL(result_path);
  ASSERT_STR_EQUAL(result_path, filepath);

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

/**
 * Test: get_file_path should work with absolute paths
 */
bool test_get_file_path_absolute(void) {
  // Arrange: Create a test file with absolute path
  const char *test_lines[] = {"test line", NULL};
  const char *filepath = "/tmp/absolute_path_test.txt";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData and get path
  FileData file_data = file_data_create(filepath);
  ASSERT_NOT_NULL(file_data);
  const char *result_path = get_file_path(file_data);

  // Assert: Path should match exactly
  ASSERT_STR_EQUAL(result_path, filepath);

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

// ============================================================================
// Tests for get_file_name()
// ============================================================================

/**
 * Test: get_file_name should extract filename from path with directory
 */
bool test_get_file_name_with_directory(void) {
  // Arrange: Create a test file
  const char *test_lines[] = {"test line", NULL};
  const char *filepath = "/tmp/test_filename.geo";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData and get filename
  FileData file_data = file_data_create(filepath);
  ASSERT_NOT_NULL(file_data);
  const char *result_name = get_file_name(file_data);

  // Assert: Should return only the filename
  ASSERT_NOT_NULL(result_name);
  ASSERT_STR_EQUAL(result_name, "test_filename.geo");

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

/**
 * Test: get_file_name should return full path when no directory separator
 */
bool test_get_file_name_no_directory(void) {
  // Arrange: Create a test file in current directory
  const char *test_lines[] = {"test line", NULL};
  const char *filepath = "simple_file.txt";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData and get filename
  FileData file_data = file_data_create(filepath);
  ASSERT_NOT_NULL(file_data);
  const char *result_name = get_file_name(file_data);

  // Assert: Should return the same as filepath
  ASSERT_NOT_NULL(result_name);
  ASSERT_STR_EQUAL(result_name, filepath);

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

/**
 * Test: get_file_name should handle nested directory paths
 */
bool test_get_file_name_nested_path(void) {
  // Arrange: Create a test file with nested path
  const char *test_lines[] = {"test line", NULL};
  const char *filepath = "/tmp/nested/path/to/file.qry";

  // Create nested directories
  system("mkdir -p /tmp/nested/path/to");
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData and get filename
  FileData file_data = file_data_create(filepath);
  ASSERT_NOT_NULL(file_data);
  const char *result_name = get_file_name(file_data);

  // Assert: Should return only the filename
  ASSERT_NOT_NULL(result_name);
  ASSERT_STR_EQUAL(result_name, "file.qry");

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);
  system("rm -rf /tmp/nested");

  return true;
}

// ============================================================================
// Tests for get_file_lines_queue()
// ============================================================================

/**
 * Test: get_file_lines_queue should return valid queue
 */
bool test_get_file_lines_queue_basic(void) {
  // Arrange: Create a test file
  const char *test_lines[] = {"line 1", "line 2", "line 3", NULL};
  const char *filepath = "/tmp/test_queue.txt";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData and get queue
  FileData file_data = file_data_create(filepath);
  ASSERT_NOT_NULL(file_data);
  Queue lines = get_file_lines_queue(file_data);

  // Assert: Queue should be valid and not empty
  ASSERT_NOT_NULL(lines);
  ASSERT_FALSE(queue_is_empty(lines));

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

/**
 * Test: get_file_lines_queue should preserve line order
 */
bool test_get_file_lines_queue_order(void) {
  // Arrange: Create a test file with specific lines
  const char *test_lines[] = {"first line", "second line", "third line", NULL};
  const char *filepath = "/tmp/test_queue_order.txt";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData and get queue
  FileData file_data = file_data_create(filepath);
  ASSERT_NOT_NULL(file_data);
  Queue lines = get_file_lines_queue(file_data);
  ASSERT_NOT_NULL(lines);

  // Assert: Lines should be in correct order
  char *line1 = (char *)queue_dequeue(lines);
  ASSERT_NOT_NULL(line1);
  ASSERT_STR_EQUAL(line1, "first line");

  char *line2 = (char *)queue_dequeue(lines);
  ASSERT_NOT_NULL(line2);
  ASSERT_STR_EQUAL(line2, "second line");

  char *line3 = (char *)queue_dequeue(lines);
  ASSERT_NOT_NULL(line3);
  ASSERT_STR_EQUAL(line3, "third line");

  // Queue should now be empty
  ASSERT_TRUE(queue_is_empty(lines));

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

/**
 * Test: get_file_lines_queue should handle lines with special characters
 */
bool test_get_file_lines_queue_special_chars(void) {
  // Arrange: Create a test file with special characters
  const char *test_lines[] = {"line with spaces   and   tabs",
                              "line-with-dashes", "line_with_underscores",
                              "line.with.dots", NULL};
  const char *filepath = "/tmp/test_special_chars.txt";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create FileData and get queue
  FileData file_data = file_data_create(filepath);
  ASSERT_NOT_NULL(file_data);
  Queue lines = get_file_lines_queue(file_data);
  ASSERT_NOT_NULL(lines);

  // Assert: Lines should be preserved exactly
  char *line1 = (char *)queue_dequeue(lines);
  ASSERT_STR_EQUAL(line1, "line with spaces   and   tabs");

  char *line2 = (char *)queue_dequeue(lines);
  ASSERT_STR_EQUAL(line2, "line-with-dashes");

  char *line3 = (char *)queue_dequeue(lines);
  ASSERT_STR_EQUAL(line3, "line_with_underscores");

  char *line4 = (char *)queue_dequeue(lines);
  ASSERT_STR_EQUAL(line4, "line.with.dots");

  // Cleanup
  file_data_destroy(file_data);
  remove_test_file(filepath);

  return true;
}

// ============================================================================
// Tests for file_data_destroy()
// ============================================================================

/**
 * Test: file_data_destroy should handle NULL gracefully
 */
bool test_file_data_destroy_null(void) {
  // Act & Assert: Should not crash
  file_data_destroy(NULL);

  return true;
}

/**
 * Test: file_data_destroy should free all resources
 */
bool test_file_data_destroy_cleanup(void) {
  // Arrange: Create a test file
  const char *test_lines[] = {"line 1", "line 2", "line 3", NULL};
  const char *filepath = "/tmp/test_destroy.txt";
  ASSERT_TRUE(create_test_file(filepath, test_lines));

  // Act: Create and destroy FileData
  FileData file_data = file_data_create(filepath);
  ASSERT_NOT_NULL(file_data);

  // This should free all allocated memory without crashing
  file_data_destroy(file_data);

  // Cleanup
  remove_test_file(filepath);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for file_data_create
  test_print_section("Testing file_data_create()");
  test_register("test_file_data_create_valid_file",
                test_file_data_create_valid_file);
  test_register("test_file_data_create_nonexistent_file",
                test_file_data_create_nonexistent_file);
  test_register("test_file_data_create_empty_file",
                test_file_data_create_empty_file);
  test_register("test_file_data_create_single_line",
                test_file_data_create_single_line);
  test_register("test_file_data_create_multiple_lines",
                test_file_data_create_multiple_lines);

  // Register tests for get_file_path
  test_print_section("Testing get_file_path()");
  test_register("test_get_file_path_basic", test_get_file_path_basic);
  test_register("test_get_file_path_absolute", test_get_file_path_absolute);

  // Register tests for get_file_name
  test_print_section("Testing get_file_name()");
  test_register("test_get_file_name_with_directory",
                test_get_file_name_with_directory);
  test_register("test_get_file_name_no_directory",
                test_get_file_name_no_directory);
  test_register("test_get_file_name_nested_path",
                test_get_file_name_nested_path);

  // Register tests for get_file_lines_queue
  test_print_section("Testing get_file_lines_queue()");
  test_register("test_get_file_lines_queue_basic",
                test_get_file_lines_queue_basic);
  test_register("test_get_file_lines_queue_order",
                test_get_file_lines_queue_order);
  test_register("test_get_file_lines_queue_special_chars",
                test_get_file_lines_queue_special_chars);

  // Register tests for file_data_destroy
  test_print_section("Testing file_data_destroy()");
  test_register("test_file_data_destroy_null", test_file_data_destroy_null);
  test_register("test_file_data_destroy_cleanup",
                test_file_data_destroy_cleanup);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
