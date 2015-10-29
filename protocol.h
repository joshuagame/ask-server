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
** protocol.h
** header for HTTP and routes management functionalities
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

#ifndef ASK_SERVER_PROTOCOL_H
#define ASK_SERVER_PROTOCOL_H

#include "ask.h"

const char* res_auth_ok = "{ \"status\": \"success\", \"message\": \"user authenticated\" }";

/** routes */
typedef int (* route_handler_t)(const void* cls, const char* mime, session_t* session, connection_t* connection);

typedef struct route {
        const char* url;
        const char* mime;
        route_handler_t handler;
        const void* handler_cls;
        bool checkSession;
} route_t;

static int post_params_iterator(void* cls, enum MHD_ValueKind kind, const char* key, const char* fileName,
                                const char* contentType, const char* transferEncoding, const char* data,
                                uint64_t off, size_t size);

static int home_handler(const void* cls, const char* mime, session_t* session, connection_t* connection);

static int basic_auth_handler(const void* cls, const char* mime, session_t* session, connection_t* connection);

static int form_based_auth_handler(const void* cls, const char* mime, session_t* session, connection_t* connection);

static int not_found_handler(const void* cls, const char* mime, session_t* session, connection_t* connection);;

static int ask_for_authentication(connection_t* connection, const char* realm);

static int send_authentication_response(connection_t* connection, session_t* session, char* body, const char* mime);

/* Ask Server Routes */
static route_t routes[] = {
        {"/",         "text/html",                       &home_handler,       HOME_PAGE,     false},
        {"/ask",      "text/html",                       &home_handler,       API_HOME_PAGE, false},
        {"/ask/auth", "application/json; charset=utf-8", &basic_auth_handler, API_HOME_PAGE, true},
        {"/ask/login", NULL,                             &home_handler,       API_HOME_PAGE, false},
        {NULL,         NULL,                             &not_found_handler,  NULL}
};

#endif //ASK_SERVER_PROTOCOL_H
