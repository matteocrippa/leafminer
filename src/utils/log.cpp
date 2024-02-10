#include "utils/log.h"

/**
 * @brief Logs an informational message.
 *
 * This function logs an informational message with the specified tag and message.
 *
 * @param TAG The tag associated with the log message.
 * @param message The message to be logged.
 * @param ... Additional arguments (optional) to be formatted into the message using printf-style formatting.
 */
void l_info(const char *TAG, PGM_P message, ...)
{
#if LOG_LEVEL >= 2
  Serial.print("[I] ");
  Serial.print(TAG);
  Serial.print(": ");
  va_list args;
  va_start(args, message);

  int size = vsnprintf(nullptr, 0, message, args) + 1;
  char *buffer = new char[size];
  vsnprintf(buffer, size, message, args);

  Serial.println(buffer);
  delete[] buffer;

  va_end(args);
#endif
}

/**
 * @brief Logs an error message.
 *
 * This function is used to log an error message with a specified tag and message.
 *
 * @param TAG The tag associated with the error message.
 * @param message The error message to be logged.
 * @param ... Additional arguments for formatting the error message.
 */
void l_error(const char *TAG, PGM_P message, ...)
{
#if LOG_LEVEL >= 1
  Serial.print("[E] ");
  Serial.print(TAG);
  Serial.print(": ");
  va_list args;
  va_start(args, message);

  int size = vsnprintf(nullptr, 0, message, args) + 1;
  char *buffer = new char[size];
  vsnprintf(buffer, size, message, args);

  Serial.println(buffer);
  delete[] buffer;

  va_end(args);
#endif
}

/**
 * @brief Writes a debug log message.
 *
 * This function is used to write a debug log message with a specified tag and message.
 * The message can be formatted using printf-style format specifiers.
 *
 * @param TAG The tag for the log message.
 * @param message The log message format string.
 * @param ... Additional arguments to be formatted into the log message.
 */
void l_debug(const char *TAG, PGM_P message, ...)
{
#if LOG_LEVEL >= 3
  Serial.print("[D] ");
  Serial.print(TAG);
  Serial.print(": ");
  va_list args;
  va_start(args, message);

  int size = vsnprintf(nullptr, 0, message, args) + 1;
  char *buffer = new char[size];
  vsnprintf(buffer, size, message, args);

  Serial.println(buffer);
  delete[] buffer;

  va_end(args);
#endif
}