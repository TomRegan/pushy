#include "../include/logging.h"

void
write_log(int level, char *fmt, ...)
{
	char		*label;
	char		log_buf [LOG_BUFFER_LEN+1];
	va_list		ap;

	if (LEVEL < level) {
		return;
	}

	switch (level) {
		case ERROR:
			label = "<error>\t";
			break;
		case INFO:
			label = "<info>\t";
			break;
		case FINE:
			label = "<fine>\t";
			break;
		case FINER:
			label = "<finer>\t";
			break;
		case FINEST:
			label = "<finest>\t";
			break;
		case DEBUG:
			label = "<debug>\t";
			break;
		default:
			label = "<oops>\t";
	}

	strncpy(log_buf, label, sizeof(log_buf));
	strncat(log_buf, fmt, sizeof(log_buf));
	va_start(ap, fmt);
	vprintf(log_buf, ap);
	va_end(ap);
}
