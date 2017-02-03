/**
 * Copyright 2012 Tom Regan <code.tom.regan@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

/**
 * @file  logging.c
 * @brief  functions for logging events
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#include "../include/logging.h"

LOG_LEVEL LEVEL;

void
log_init(void)
{
    char *user_level = getenv("PUSHY_LOG_LEVEL");
    if (user_level == NULL) {
        user_level = "INFO";
    }

    if (strncmp("NONE", user_level, 4) == 0) {
        LEVEL = NONE;
    } else if(strncmp("ERROR", user_level, 5) == 0) {
        LEVEL = ERROR;
    } else if(strncmp("INFO", user_level, 5) == 0) {
        LEVEL = INFO;
    } else if(strncmp("FINE", user_level, 5) == 0) {
        LEVEL = FINE;
    } else if(strncmp("FINER", user_level, 5) == 0) {
        LEVEL = FINER;
    } else if(strncmp("FINEST", user_level, 5) == 0) {
        LEVEL = FINEST;
    } else if(strncmp("DEBUG", user_level, 5) == 0) {
        LEVEL = DEBUG;
    } else if(strncmp("MEM_DEBUG", user_level, 5) == 0) {
        LEVEL = MEM_DEBUG;
    }
    log_ln(INFO, "Logging level is set to %s\n", user_level);
}


char*
_label(LOG_LEVEL level)
{
    char *label;

    switch (level) {
    case ERROR:
        label = "[error]\t";
        break;
    case INFO:
        label = "[info]\t";
        break;
    case FINE:
        label = "[fine]\t";
        break;
    case FINER:
        label = "[finer]\t";
        break;
    case FINEST:
        label = "[finest]\t";
        break;
    case DEBUG:
        label = "[debug]\t";
        break;
    case MEM_DEBUG:
        label = "[memory_debug]\t";
        break;
    default:
        label = "[oops]\t";
    }
    return label;
}

void
log_ln(LOG_LEVEL level, char *fmt, ...)
{
    char    log_buf [LOG_BUFFER_LEN + 1];
    va_list ap;

    if (LEVEL < level) {
        return;
    }

    strncpy(log_buf, _label(level), LOG_BUFFER_LEN);
    strncat(log_buf, fmt, (LOG_BUFFER_LEN - strlen(log_buf)));
    va_start(ap, fmt);
    vprintf(log_buf, ap);
    va_end(ap);
}

void
log_conn(LOG_LEVEL level, struct sockaddr_in *sockaddr, char *fmt, ...)
{
    char    log_buf [LOG_BUFFER_LEN + 1], tmp_buf[LOG_BUFFER_LEN + 1];
    va_list ap;

    if (LEVEL < level) {
        return;
    }

    strncpy(log_buf, _label(level), LOG_BUFFER_LEN);
    sprintf(tmp_buf, "[%s:%hu] ", inet_ntoa(sockaddr->sin_addr), ntohs(sockaddr->sin_port));
    strncat(log_buf, tmp_buf, (LOG_BUFFER_LEN - strlen(log_buf)));
    strncat(log_buf, fmt, (LOG_BUFFER_LEN - strlen(log_buf)));
    va_start(ap, fmt);
    vprintf(log_buf, ap);
    va_end(ap);
}
