/*
**     _    ____  _  __
**    / \  / ___|| |/ /  ASK
**   / _ \ \___ \| ' /   Authentication Sessions Keeper Server
**  / ___ \ ___) | . \   -------------------------------------
** /_/   \_\____/|_|\_\  www.codegazoline.it/ask
**                 v0.1
** ===========================================================================
**
** Copyright (C) 2015, The CodeGazoline Team - gargantua AT codegazoline DOT it
** Luca {joshuagame} Stasio - joshuagame AT gmail DOT com
**
** ask.h
** main header file for the ASK Server
**
** This file is part of the ASK Server.
**
** The ASK Server is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** The ASK Server is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Nome - Programma.If not, see <http://www.gnu.org/licenses/>.
**
** ===========================================================================
*/

#ifndef ASK_SERVER_ASK_H
#define ASK_SERVER_ASK_H

#define _GNU_SOURCE         /*!< this is for asprintf() */

#include "version.h"
#include "ini.h"
#include "uthash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <microhttpd.h>

#ifndef __APPLE__
#ifdef __CYGWIN__

#include <sys/errno.h>

#else
#include <bits/errno.h>
#endif
#endif

#define ASK_REALM "\"ask\""
#define ASK_COOKIE_NAME "ASKSESSION"
#define AUTHENTICATED MHD_YES
#define NOT_AUTHENTICATED MHD_NO
#define NO_BASIC_AUTH_INFO 0x100

/* html pages */
#define HOME_PAGE "<html><head><title>ASK Server</title></head>"\
                    "<body>This is the Authentication Sessions Keeper Server up and running instance.</body></html>"
#define API_HOME_PAGE "<html><head><title>ASK Server</title></head>"\
                    "<body>This is the Authentication Sessions Keeper Server API Home:<br>"\
                    "<b>/ask/ab</b>: Basic authentication<br>"\
                    "<b>/ask/af</b>: Form-based authentication<br></body></html>"
#define ERROR_ILLEGAL_REQUEST_PAGE "<html><head><title>ASK Server</title></head>"\
                    "<body>Illegal request.</body></html>"
#define ERROR_NOT_FOUND_PAGE "<html><head><title>ASK Server</title></head>"\
                    "<body>Not found.</body></html>"

/** configuration */

struct config {
        int port;
        bool ssl;
        const char* name;
        char* http_auth_url;
        bool http_auth_ssl;
};

enum CL_CONF {
        NONE = 0,
        PORT = 1,
        _SSL = 2
};

extern struct config global_config;
extern const char* config_file_name;
extern unsigned int command_line_configured_params;

/** request and session */

#define ASK_UNAME "ask_username"
#define ASK_PWD "ask_password"

typedef struct form_credentials {
        char username[256];
        char password[64];
} form_credentials_t;

typedef enum session_state {
        STARTED, ACTIVE, EXPIRED
} session_state_t;

typedef struct session {
//    struct Session* next;
        char id[37];
        unsigned int rc;
        time_t start;
        time_t expiration;
        form_credentials_t fcred;
        session_state_t state;
        UT_hash_handle hh;
} session_t;

typedef struct request {
        session_t* session;
        struct MHD_PostProcessor* post_processor;
        const char* post_url;
} request_t;

typedef struct MHD_Response response_t;
typedef struct MHD_Connection connection_t;

/** simple log engine structures */
#define TPL_IDENT "ask"

enum tp_log_level {
        TPL_DEBUG,
        TPL_INFO,
        TPL_ERR,
        TPL_EMERG,
};

enum tp_log_mode {
        TPLM_SYSLOG,
        TPLM_FILE,
};

/** forward (only public) function declarations */

/* configure.c */
void configure(int argc, char* const* argv);

/* session.c */
char* generate_session_id();

session_t* get_session(struct MHD_Connection* connection);

void add_session_cookie(session_t* session, response_t* response);

void add_expired_cookie(response_t* response);

const char* get_session_cookie(connection_t* connection);

void set_session_username(session_t* session, size_t size, uint64_t offset, const char* data);

void set_session_password(session_t* session, size_t size, uint64_t offset, const char* data);

const char* get_session_username(session_t* session);

const char* get_session_password(session_t* session);

/* protocol.c */

int request_handler(void* cls, struct MHD_Connection* connection, const char* url, const char* method,
                    const char* version, const char* upload_data, size_t* upload_data_size, void** ptr);

void request_completed_callback(void* cls, struct MHD_Connection* connection,
                                void** con_cls, enum MHD_RequestTerminationCode toe);

const char* get_header_value(connection_t* connection, const char* header_name);

/* authentication.c */
int authenticate(connection_t* connection, session_t* session);

/* http_auth_client.c */
int http_basic_authentication(const char* username, const char* basic_auth);

/* log.c */
void log_init(int mode, int level, int fd);

void asklog(int level, const char* fmt, ...);

void log_dispose(void);

#endif //ASK_SERVER_ASK_H
