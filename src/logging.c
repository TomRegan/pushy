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
			label = "<error>  ";
			break;
		case INFO:
			label = "<info>  ";
			break;
		case FINE:
			label = "<fine>  ";
			break;
		case FINER:
			label = "<finer>  ";
			break;
		case FINEST:
			label = "<finest>  ";
			break;
		case DEBUG:
			label = "<debug>  ";
			break;
		default:
			label = "<oops>  ";
	}

	strncpy(log_buf, label, sizeof(log_buf));
	strncat(log_buf, fmt, sizeof(log_buf));
	va_start(ap, fmt);
	vprintf(log_buf, ap);
	va_end(ap);
}
