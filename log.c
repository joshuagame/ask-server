/**
 * Little Log Engine
 * (C) user2079392 - StackExchange
 * http://codereview.stackexchange.com/questions/23110/little-log-engine-in-c
 */
#include "ask.h"
#include <stdarg.h>
#include <syslog.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* function from pio.c I use */
ssize_t tp_write(int fd, const void *buf, size_t len)
{
    ssize_t ret, wlen = 0;
    const char *ptr;

    ptr = buf;
    while (wlen != len) {
        ret = write(fd, ptr, len-wlen);

        if (ret == -1)
        if (errno == EINTR)
            continue;
        else
            return -1;

        wlen += ret;
        ptr += ret;

    }
    return wlen;
}

/* start of log.c */

static int log_fd = -1;
static int log_level;
static int log_mode;

static const char *level_txt[] = {
        //"DEBUG", "INFO", "ERROR", "EMERG"
        "D", "I", "E", "F"
};

static const int level_syslog[] = {
        LOG_DEBUG, LOG_INFO, LOG_ERR, LOG_EMERG
};

void tp_log_init(int mode, int level, int fd)
{
    assert(mode == TPLM_SYSLOG || mode == TPLM_FILE);
    assert(mode == TPLM_FILE ? fd >= 0 : 1);
    assert(level >= TPL_DEBUG && level <= TPL_EMERG);

    switch (mode) {
        case TPLM_FILE:
            log_fd = fd;
            break;
        case TPLM_SYSLOG:
            openlog(TPL_IDENT, LOG_PID|LOG_CONS, LOG_DAEMON);
            break;
    }
    log_level = level;
    log_mode = mode;
}

#define MAX_TIME_LEN 512
#define MAX_PREFIX_LEN 10
#define MAX_MSG_LEN 1024
#define MAX_POSTFIX_LEN 2

static void tp_vlog_write(int level, const char *fmt, va_list alist)
{
    int ret, len;
    time_t t;
    struct tm *tm;
    char msg[MAX_TIME_LEN+MAX_PREFIX_LEN+MAX_MSG_LEN+MAX_POSTFIX_LEN];

    assert(log_fd);
    assert(fmt);
    assert(level >= TPL_DEBUG && level <= TPL_EMERG);

    if (level < log_level)
        return;

    switch (log_mode) {
        case TPLM_FILE:

            t = time(NULL);
            tm = localtime(&t);
            if (tm == NULL)
                abort();

            len = strftime(msg, MAX_TIME_LEN, "<%d-%m-%Y %T%z>", tm);
            len += snprintf(msg+len, MAX_PREFIX_LEN, "<%s> - ",
                            level_txt[level]);

            ret = vsnprintf(msg+len, MAX_MSG_LEN, fmt, alist);
            if (ret >= MAX_MSG_LEN)
                len += MAX_MSG_LEN-1;
            else
                len += ret;

            snprintf(msg+len, MAX_POSTFIX_LEN, "\n");
            tp_write(log_fd, msg, len+1);
            break;

        case TPLM_SYSLOG:
#ifdef HAVE_VSYSLOG
            vsyslog(level_syslog[level], fmt, alist);
#else
            vsnprintf(msg, MAX_MSG_LEN, fmt, alist);
            syslog(level_syslog[level], "%s", msg);
#endif
            break;
    }
}

void tp_log_write(int level, const char *fmt, ...)
{
    va_list vl;

    va_start(vl, fmt);
    tp_vlog_write(level, fmt, vl);
    va_end(vl);
}

void tp_log_close(void)
{
    assert(log_fd);

    switch (log_mode) {
        case TPLM_SYSLOG:
            closelog();
            break;
        case TPLM_FILE:
            close(log_fd);
            break;
    }
}