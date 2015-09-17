/*
**     _    ____  _  __
**    / \  / ___|| |/ /  ASK
**   / _ \ \___ \| ' /   Authentication Sessions Keeper Server
**  / ___ \ ___) | . \   -------------------------------------
** /_/   \_\____/|_|\_\  www.codegazoline.it/ask
**                 v0.1
** ===========================================================================
**
** Copyright (C) 2015, The CodeGazoline Team - gargantua@codegazoline.it
**
** askd.c
** main entry point for the ASK Server
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
** along with Nome - Programma.If not, see <http:**www.gnu.org/licenses/>.
**
** ===========================================================================
*/

/**/
#define _GNU_SOURCE         /*!< this is for asprintf() */

#include "version.h"
#include "ini.h"

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
#include <bits/errno.h>
#endif

#define REALM "\"ask\""
#define TEST_USER "test"
#define TEST_PASSWORD "password"
#define DEFAULT_PORT 5080
#define DEFAULT_SSL_PORT 5443
#define ASK_COOKIE_NAME "ASKSESSION"

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

struct Config {
    int port;
    bool ssl;
    const char* name;
} globalConfig = {
        .port = DEFAULT_PORT,
        .ssl = false
};

const char* configFileName = "ask.ini";

enum CL_CONF {
    NONE = 0,
    PORT = 1,
    SSL = 2
};

unsigned int commandLineConfiguredParams = NONE;

/** request and session */

struct FormCredentials {
    char username[64];
    char password[64];
};

typedef struct Session {
    struct Session* next;
    char id[33];
    unsigned int rc;
    time_t start;
    struct FormCredentials fcred;
} Session;

/** sessions linked list. TODO: use an HashMap! */
static Session* sessions;

typedef struct {
    Session* session;
    struct MHD_PostProcessor* postProcessor;
    const char* postUrl;
} Request;

typedef struct MHD_Response Response;

/** routes */
typedef int (*RouteHandler)(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection);

typedef struct Route {
    const char* url;
    const char* mime;
    RouteHandler handler;
    const void* handlerCls;
    bool checkSession;
} Route;

/** forward function declarations */
void usage();
static int loadConfigurationHandler(void* user, const char* section, const char* name, const char* value);
int loadConfiguration();
void printConfiguration();
void startServer();
static Session* getSession(struct MHD_Connection* connection);
static int postParamsIterator(void* cls, enum MHD_ValueKind kind, const char* key, const char* fileName,
                              const char* contentType, const char* transferEncoding, const char* data,
                              uint64_t off, size_t size);
static int requestHandler(void* cls, struct MHD_Connection* connection, const char* url, const char* method,
                          const char* version, const char* uploadData, size_t* uploadDataSize, void** ptr);
static void requestCompletedCallback(void* cls, struct MHD_Connection* connection,
                                     void** conCls, enum MHD_RequestTerminationCode toe);
static int homeHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection);
static int basicAuthHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection);
static int formBasedAuthHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection);
static int homeHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection);
static int notFoundHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection);
static int basicAuthHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection);
static int formBasedAuthHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection);

/* ------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------- */



/**
 * Main
 */
int main(int argc, char *const *argv) {
    int c;
    const char *shortOptions = "hp:sC:";
    static const struct option longOptions[] = {
            {"help", no_argument, NULL, 'h'},
            {"port", required_argument, NULL, 'p'},
            {"ssl", no_argument, NULL, 's'},
            {"config-file", required_argument, NULL, 'C'},
            {NULL, 0, NULL, 0}
    };
    int longIndex = 0;

    globalConfig.name = argv[0];

    int opt = getopt_long( argc, argv, shortOptions, longOptions, &longIndex );
    while (opt != -1) {
        switch (opt) {
            case 'h':
                usage();
                exit(0);
                //break;
            case 'p':
                globalConfig.port = atoi(optarg);
                commandLineConfiguredParams |= PORT;
                break;
            case 's':
                globalConfig.ssl = true;
                commandLineConfiguredParams |= SSL;

                /* if no port value as command line param, then set default ssl port value */
                if ((commandLineConfiguredParams & PORT) == 0) {
                    globalConfig.port = DEFAULT_SSL_PORT;
                }
                break;
            case 'C':
                configFileName = optarg;
                break;
            default:
                break;
        }
        opt = getopt_long( argc, argv, shortOptions, longOptions, &longIndex );
    }

    printf("\n>> ASK server (%s) initialization\n", globalConfig.name);
    printf(">> Loading configuration from %s\n", configFileName);

    /* load configuration from file */
    loadConfiguration();
    printConfiguration();

    /* start the server */
    startServer();

    return 0;
}

/* ------------------------------------------------------------------------------------------------------------- */
/* CONFIGURATION MANAGEMENT                                                                                      */
/* ------------------------------------------------------------------------------------------------------------- */

void usage()
{
    printf("Usage: %s [OPTIONS]\n", globalConfig.name);
    printf("  -p, --port port           ASK server port number\n");
    printf("  -s, --ssl                 enable SSL\n");
    printf("  -C, --config-file         configuration file\n");
    printf("  -h, --help                print this help and exit\n");
    printf("\n");
}


static int loadConfigurationHandler(void* user, const char* section, const char* name,
                   const char* value)
{
    struct Config* pconfig = (struct Config*)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("server", "port")) {
        if ((commandLineConfiguredParams & PORT) == 0) {
            /* if no port value as command line param now we have the default value,
             * so here we can get the port specified in the INI file */
            pconfig->port = atoi(value);
        }
    } else if (MATCH("server", "ssl")) {
        if ((commandLineConfiguredParams & SSL) == 0) {
            /* if no SSL value as command line param here we get INI configuration */
            pconfig->ssl = strcmp(strdup(value), "true") == 0 ? true : false;
        }
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}


int loadConfiguration()
{
    if (ini_parse(configFileName, loadConfigurationHandler, &globalConfig)) {
        perror("Unable to load configuration");
        exit(-1);
    }
}

void printConfiguration()
{
    printf("=====================================================================\n");
    printf("%s configuration\n", globalConfig.name);
    printf("---------------------------------------------------------------------\n");
    printf("  Version         :     %s\n", ASK_VERSION);
    printf("  Listening port  :     %d\n", globalConfig.port);
    printf("  SSL             :     %s\n", globalConfig.ssl ? "enabled" : "disabled");
    printf("  Http Endpoint   :     http%s://localhost:%d//ask-api/auth\n", globalConfig.ssl ? "s" : "", globalConfig.port);
    printf("=====================================================================\n");
}

void startServer()
{
    struct MHD_Daemon* mhdd;
    struct timeval tv;
    struct timeval* tvp;
    fd_set readFdSet;
    fd_set writeFdSet;
    fd_set exceptionFdSet;
    int maxFd;
    unsigned MHD_LONG_LONG mhdTimeout;

    /** TODO: set connection timeout in config INI */
    srandom((unsigned int)time(NULL));
    mhdd = MHD_start_daemon(MHD_USE_DEBUG, globalConfig.port, NULL, NULL, &requestHandler, NULL,
                              MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int)15,
                              MHD_OPTION_NOTIFY_COMPLETED, &requestCompletedCallback, NULL,
                              MHD_OPTION_END);

    if (mhdd == NULL) {
        perror("Unable to start server: error initializing internal MHD server Daemon\n");
        exit(-1);
    }

    bool running = true;
    while(running) {
        //handleExpiredSessions();
        maxFd = 0;
        FD_ZERO(&readFdSet);
        FD_ZERO(&writeFdSet);
        FD_ZERO(&exceptionFdSet);

        if (MHD_get_fdset(mhdd, &readFdSet, &writeFdSet, &exceptionFdSet, &maxFd) != MHD_YES) {
            perror("Unable to start server: fatal internal error occurred\n");
            exit(-1);
        }

        tvp = NULL;
        if (MHD_get_timeout(mhdd, &mhdTimeout) == MHD_YES) {
            tv.tv_sec = mhdTimeout / 1000;
            tv.tv_usec = (mhdTimeout - (tv.tv_sec * 1000)) * 1000;
            tvp = &tv;
        }

        select(maxFd + 1, &readFdSet, &writeFdSet, &exceptionFdSet, tvp);

        /* finally, fire up the server and make it running! */
        MHD_run(mhdd);
    }

    MHD_stop_daemon(mhdd);
}

/* ------------------------------------------------------------------------------------------------------------- */
/* ROUTES & HANLDERS                                                                                                */
/* ------------------------------------------------------------------------------------------------------------- */


/* Ask Server Routes */
static Route routes[] = {
        {"/", "text/html", &homeHandler, HOME_PAGE, false},
        {"/ask", "text/html", &homeHandler, API_HOME_PAGE, false},
        {"/ask/authb", NULL, &basicAuthHandler, API_HOME_PAGE, true},
        {"/ask/authf", NULL, &formBasedAuthHandler, API_HOME_PAGE, true},
        {"/ask/login", NULL, &homeHandler, API_HOME_PAGE, false},
        {NULL, NULL, &notFoundHandler, NULL}
};

static int homeHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection)
{
    int result;
    const char* htmlContent = cls;
    char* responseContent;
    Response* response;

    if (asprintf(&responseContent, "%s", htmlContent) == -1) {
        return MHD_NO;
    }

    /* prepare the response */
    response = MHD_create_response_from_buffer(strlen(responseContent), (void*)responseContent, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_ENCODING, mime);

    /* enqueue response for send */
    result = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return result;
}

static int notFoundHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection)
{
    Response* response = MHD_create_response_from_buffer(strlen(ERROR_NOT_FOUND_PAGE), (void*)ERROR_NOT_FOUND_PAGE,
                                                         MHD_RESPMEM_PERSISTENT);
    int result = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_ENCODING, mime);
    MHD_destroy_response(response);

    return result;

}

static int basicAuthHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection)
{
    return 0;
}

static int formBasedAuthHandler(const void* cls, const char* mime, Session* session, struct MHD_Connection* connection)
{
    return 0;
}

/* ------------------------------------------------------------------------------------------------------------- */
/* PROTOCOL                                                                                                      */
/* ------------------------------------------------------------------------------------------------------------- */


static int postParamsIterator(void* cls, enum MHD_ValueKind kind, const char* key, const char* fileName,
                              const char* contentType, const char* transferEncoding, const char* data,
                              uint64_t off, size_t size)
{
    return MHD_YES;
}

static int requestHandler(void* cls, struct MHD_Connection* connection, const char* url, const char* method,
                          const char* version, const char* uploadData, size_t* uploadDataSize, void** ptr)
{
    Response* response;
    Request* request;
    int result;

    if ((request = *ptr) == NULL) {
        if ((request = calloc(1, sizeof(Request))) == NULL) {
            //fprintf(stderr, "unable to calloc request structure: %s\n", strerror(errno));
            perror("unable to alloc request structure\n");
            return MHD_NO;
        }
        *ptr = request;

        if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
            request->postProcessor = MHD_create_post_processor(connection, 1024, &postParamsIterator, request);
            if (request->postProcessor == NULL) {
                fprintf(stderr, "Failed to setup post processor for '%s'\n", url);
                return MHD_NO;
            }
        }

        return MHD_YES;
    }

    if (request->session == NULL) {
        request->session = getSession(connection);
        if (request->session == NULL) {
            fprintf(stderr, "unable to set up session for '%s'\n", url);
            return MHD_NO;
        }
    }

    Session* session = request->session;
    session->start = time(NULL);
    if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
        fprintf(stdout, "POST method\n");

        /* eval request post data */
        MHD_post_process(request->postProcessor, uploadData, *uploadDataSize);
        fprintf(stdout, "POST data processed\n");


        if (*uploadDataSize != 0) {
            fprintf(stdout, "upload data size = 0\n");
            *uploadDataSize = 0;
            return MHD_YES;
        }

        /* Ok, here we have done with POST data, now we can serve the response */
        fprintf(stdout, "serving response to client\n");
        MHD_destroy_post_processor(request->postProcessor);
        request->postProcessor = NULL;

        /* here we perform an internal fake GET request to the POST url */
        method = MHD_HTTP_METHOD_GET;
        if (request->postUrl != NULL) {
            url = request->postUrl;
        }
    }

    if ((strcmp(method, MHD_HTTP_METHOD_GET) == 0) || (strcmp(method, MHD_HTTP_METHOD_HEAD) == 0)) {
        /* find route by url. TODO: improve this using an HashMap instead of an array for routes */
        unsigned int i = 0;
        while ((routes[i].url != NULL) && (strcmp(routes[i].url, url) != 0)) {
            i++;
        }

        result = routes[i].handler(routes[i].handlerCls, routes[i].mime, session, connection);
        if (result != MHD_YES) {
            fprintf(stderr, "Error handling route to '%s'\n", url);
        }

        return result;
    }

    response = MHD_create_response_from_buffer(strlen(ERROR_ILLEGAL_REQUEST_PAGE), (void*)ERROR_ILLEGAL_REQUEST_PAGE,
                                               MHD_RESPMEM_PERSISTENT);
    result = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ACCEPTABLE, response);
    MHD_destroy_response(response);

    return result;
}

static void requestCompletedCallback(void* cls, struct MHD_Connection* connection,
                                     void** conCls, enum MHD_RequestTerminationCode toe)
{
    Request* request = *conCls;

    if (request != NULL) {
        return;
    }

    if (request->session != NULL) {
        request->session->rc--;
    }
    if (request->postProcessor != NULL) {
        MHD_destroy_post_processor(request->postProcessor);
    }

    free(request);
}

/* ------------------------------------------------------------------------------------------------------------- */
/* SESSION MANAGEMENT                                                                                            */
/* ------------------------------------------------------------------------------------------------------------- */

static Session* getSession(struct MHD_Connection* connection)
{
    Session* session;
    const char* cookie;

    /* search for an existing session for this connection */
    if ((cookie = MHD_lookup_connection_value(connection, MHD_COOKIE_KIND, ASK_COOKIE_NAME)) != NULL) {
        session = sessions;
        while (session != NULL) {
            if (strcmp(cookie, session->id) == 0) break;
            session = session->next;
        }
        if (session != NULL) {
            session->rc++;
            return session;
        }
    }

    /* create a new session */
    session = calloc(1, sizeof(Session));
    if (session == NULL) {
        perror("unable to alloc session structure for the request\n");
        return NULL;
    }

    /* generate a random unique session id.
     * Note: this is not so secure, so, change the way we do it!
     */
    unsigned int v1 = (unsigned int)random();
    unsigned int v2 = (unsigned int)random();
    unsigned int v3 = (unsigned int)random();
    unsigned int v4 = (unsigned int)random();
    snprintf(session->id, sizeof(session->id), "%X%X%X%X", v1, v2, v3, v4);
    session->rc++;
    session->start = time(NULL);

    /* put the new session at the head (lifo) of the sessions list */
    session->next = sessions;
    sessions = session;

    return session;
}

static void addSessionCookie(Session* session, Response* response)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s=%s", ASK_COOKIE_NAME, session->id);
    if (MHD_add_response_header(response, MHD_HTTP_HEADER_SET_COOKIE, buffer) == MHD_NO) {
        perror("Unable to set session cookie header.\n");
    }
}

/* ------------------------------------------------------------------------------------------------------------- */
/* AUTHENTICATION                                                                                                */
/* ------------------------------------------------------------------------------------------------------------- */
