#include <pebble.h>

#include "tools.h"

static void log_int(int num)
{
	static char log_buffer[100]; // We need "static" so the buffer persists...?
	snprintf(log_buffer, sizeof(log_buffer), "%d", num);

	APP_LOG(APP_LOG_LEVEL_INFO, log_buffer);
}