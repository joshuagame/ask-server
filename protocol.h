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

/** routes */
typedef int (*RouteHandler)(const void* cls, const char* mime, Session* session, Connection* connection);

typedef struct Route {
    const char* url;
    const char* mime;
    RouteHandler handler;
    const void* handlerCls;
    bool checkSession;
} Route;

static int postParamsIterator(void* cls, enum MHD_ValueKind kind, const char* key, const char* fileName,
                              const char* contentType, const char* transferEncoding, const char* data,
                              uint64_t off, size_t size);
static int homeHandler(const void* cls, const char* mime, Session* session, Connection* connection);
static int basicAuthHandler(const void* cls, const char* mime, Session* session, Connection* connection);
static int formBasedAuthHandler(const void* cls, const char* mime, Session* session, Connection* connection);
static int homeHandler(const void* cls, const char* mime, Session* session, Connection* connection);
static int notFoundHandler(const void* cls, const char* mime, Session* session, Connection* connection);
static int basicAuthHandler(const void* cls, const char* mime, Session* session, Connection* connection);
static int formBasedAuthHandler(const void* cls, const char* mime, Session* session, Connection* connection);
static int askForAuthentication(Connection* connection, const char* realm);

/* Ask Server Routes */
static Route routes[] = {
        {"/", "text/html", &homeHandler, HOME_PAGE, false},
        {"/ask", "text/html", &homeHandler, API_HOME_PAGE, false},
        {"/ask/authb", "application/json; charset=utf-8", &basicAuthHandler, API_HOME_PAGE, true},
        {"/ask/authf", NULL, &formBasedAuthHandler, API_HOME_PAGE, true},
        {"/ask/login", NULL, &homeHandler, API_HOME_PAGE, false},
        {NULL, NULL, &notFoundHandler, NULL}
};

#endif //ASK_SERVER_PROTOCOL_H
