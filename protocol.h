//
// Created by joshuagame on 17/09/15.
//

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

/* Ask Server Routes */
static Route routes[] = {
        {"/", "text/html", &homeHandler, HOME_PAGE, false},
        {"/ask", "text/html", &homeHandler, API_HOME_PAGE, false},
        {"/ask/authb", NULL, &basicAuthHandler, API_HOME_PAGE, true},
        {"/ask/authf", NULL, &formBasedAuthHandler, API_HOME_PAGE, true},
        {"/ask/login", NULL, &homeHandler, API_HOME_PAGE, false},
        {NULL, NULL, &notFoundHandler, NULL}
};

#endif //ASK_SERVER_PROTOCOL_H
