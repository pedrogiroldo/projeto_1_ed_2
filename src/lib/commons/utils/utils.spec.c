/**
 * @file utils.spec.c
 * @brief Unit tests for utils module
 *
 * Comprehensive unit tests for utility functions defined in utils.h
 */

#include "utils.h"
#include "../../test_framework/test_framework.h"

#include <stdlib.h>
#include <string.h>

// ============================================================================
// Tests for duplicate_string()
// ============================================================================

/**
 * Test: duplicate_string should duplicate a normal string
 */
bool test_duplicate_string_basic(void) {
  // Arrange: Create a test string
  const char *original = "Hello, World!";

  // Act: Duplicate the string
  char *duplicate = duplicate_string(original);

  // Assert: Should create a valid duplicate
  ASSERT_NOT_NULL(duplicate);
  ASSERT_STR_EQUAL(duplicate, original);

  // Cleanup
  free(duplicate);

  return true;
}

/**
 * Test: duplicate_string should create independent copy
 */
bool test_duplicate_string_independent_copy(void) {
  // Arrange: Create a test string
  const char *original = "test";

  // Act: Duplicate the string and modify it
  char *duplicate = duplicate_string(original);
  ASSERT_NOT_NULL(duplicate);
  duplicate[0] = 'b'; // Change 't' to 'b'

  // Assert: Original should be unchanged
  ASSERT_STR_EQUAL(original, "test");
  ASSERT_STR_EQUAL(duplicate, "best");

  // Cleanup
  free(duplicate);

  return true;
}

/**
 * Test: duplicate_string should handle empty string
 */
bool test_duplicate_string_empty(void) {
  // Arrange: Create an empty string
  const char *original = "";

  // Act: Duplicate the string
  char *duplicate = duplicate_string(original);

  // Assert: Should create a valid empty string
  ASSERT_NOT_NULL(duplicate);
  ASSERT_STR_EQUAL(duplicate, "");

  // Cleanup
  free(duplicate);

  return true;
}

/**
 * Test: duplicate_string should handle NULL input
 */
bool test_duplicate_string_null(void) {
  // Act: Try to duplicate NULL
  char *duplicate = duplicate_string(NULL);

  // Assert: Should return NULL
  ASSERT_NULL(duplicate);

  return true;
}

/**
 * Test: duplicate_string should handle long strings
 */
bool test_duplicate_string_long(void) {
  // Arrange: Create a long test string
  const char *original =
      "This is a very long string that contains many characters and should "
      "still be duplicated correctly by the duplicate_string function.";

  // Act: Duplicate the string
  char *duplicate = duplicate_string(original);

  // Assert: Should create a valid duplicate
  ASSERT_NOT_NULL(duplicate);
  ASSERT_STR_EQUAL(duplicate, original);

  // Cleanup
  free(duplicate);

  return true;
}

/**
 * Test: duplicate_string should handle special characters
 */
bool test_duplicate_string_special_chars(void) {
  // Arrange: Create a string with special characters
  const char *original = "Hello\nWorld\t!\r\n";

  // Act: Duplicate the string
  char *duplicate = duplicate_string(original);

  // Assert: Should preserve special characters
  ASSERT_NOT_NULL(duplicate);
  ASSERT_STR_EQUAL(duplicate, original);

  // Cleanup
  free(duplicate);

  return true;
}

// ============================================================================
// Tests for invert_color()
// ============================================================================

/**
 * Test: invert_color should invert hex color
 */
bool test_invert_color_hex_basic(void) {
  // Arrange: Create a hex color
  const char *original = "#000000";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return inverted color
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "#FFFFFF");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should invert white to black
 */
bool test_invert_color_hex_white(void) {
  // Arrange: Create a white color
  const char *original = "#ffffff";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return black
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "#000000");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should invert arbitrary hex color
 */
bool test_invert_color_hex_arbitrary(void) {
  // Arrange: Create an arbitrary hex color
  const char *original = "#aabbcc";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return inverted color (55 44 33)
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "#554433");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should handle named color - red
 */
bool test_invert_color_named_red(void) {
  // Arrange: Use named color
  const char *original = "red";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return inverted color
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "#00FFFF");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should handle named color - blue
 */
bool test_invert_color_named_blue(void) {
  // Arrange: Use named color
  const char *original = "blue";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return inverted color
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "#FFFF00");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should handle named color - green
 */
bool test_invert_color_named_green(void) {
  // Arrange: Use named color
  const char *original = "green";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return inverted color (green is 0,128,0 -> inverted is
  // 255,127,255)
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "#FF7FFF");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should handle named color - yellow
 */
bool test_invert_color_named_yellow(void) {
  // Arrange: Use named color
  const char *original = "yellow";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return inverted color
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "#0000FF");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should handle named color - black
 */
bool test_invert_color_named_black(void) {
  // Arrange: Use named color
  const char *original = "black";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return white
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "#FFFFFF");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should handle unrecognized color
 */
bool test_invert_color_unrecognized(void) {
  // Arrange: Use unrecognized color
  const char *original = "unknown_color";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return duplicate of original
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "unknown_color");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should handle NULL input
 */
bool test_invert_color_null(void) {
  // Act: Try to invert NULL
  char *inverted = invert_color(NULL);

  // Assert: Should return NULL
  ASSERT_NULL(inverted);

  return true;
}

/**
 * Test: invert_color should handle uppercase hex
 */
bool test_invert_color_hex_uppercase(void) {
  // Arrange: Create an uppercase hex color
  const char *original = "#AABBCC";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return inverted color in uppercase
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "#554433");

  // Cleanup
  free(inverted);

  return true;
}

/**
 * Test: invert_color should handle mixed case named colors
 */
bool test_invert_color_named_mixed_case(void) {
  // Arrange: Use mixed case named color
  const char *original = "Red";

  // Act: Invert the color
  char *inverted = invert_color(original);

  // Assert: Should return duplicate (case-sensitive, "Red" != "red")
  ASSERT_NOT_NULL(inverted);
  ASSERT_STR_EQUAL(inverted, "Red");

  // Cleanup
  free(inverted);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for duplicate_string
  test_print_section("Testing duplicate_string()");
  test_register("test_duplicate_string_basic", test_duplicate_string_basic);
  test_register("test_duplicate_string_independent_copy",
                test_duplicate_string_independent_copy);
  test_register("test_duplicate_string_empty", test_duplicate_string_empty);
  test_register("test_duplicate_string_null", test_duplicate_string_null);
  test_register("test_duplicate_string_long", test_duplicate_string_long);
  test_register("test_duplicate_string_special_chars",
                test_duplicate_string_special_chars);

  // Register tests for invert_color
  test_print_section("Testing invert_color()");
  test_register("test_invert_color_hex_basic", test_invert_color_hex_basic);
  test_register("test_invert_color_hex_white", test_invert_color_hex_white);
  test_register("test_invert_color_hex_arbitrary",
                test_invert_color_hex_arbitrary);
  test_register("test_invert_color_named_red", test_invert_color_named_red);
  test_register("test_invert_color_named_blue", test_invert_color_named_blue);
  test_register("test_invert_color_named_green", test_invert_color_named_green);
  test_register("test_invert_color_named_yellow",
                test_invert_color_named_yellow);
  test_register("test_invert_color_named_black", test_invert_color_named_black);
  test_register("test_invert_color_unrecognized",
                test_invert_color_unrecognized);
  test_register("test_invert_color_null", test_invert_color_null);
  test_register("test_invert_color_hex_uppercase",
                test_invert_color_hex_uppercase);
  test_register("test_invert_color_named_mixed_case",
                test_invert_color_named_mixed_case);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
