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
** protocol.c
** HTTP and routes management functionalities
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

#include "protocol.h"
#include "session.h"
#include "ask.h"

int requestHandler(void* cls, Connection* connection, const char* url, const char* method,
                   const char* version, const char* uploadData, size_t* uploadDataSize, void** ptr)
{
    log(TPL_ERR, ">>>> handling request");
    Response* response;
    Request* request;
    int result;
    const char* header;

    if ((header = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_TYPE)) != NULL) {
//        log(TPL_DEBUG, "Request Content-Type: %s", header);
    }

    if ((header = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONNECTION)) != NULL) {
//        log(TPL_DEBUG, "Request Connection: %s", header);
    }

    if ((request = *ptr) == NULL) {
        if ((request = calloc(1, sizeof(Request))) == NULL) {
            log(TPL_ERR, "unable to alloc request structure");
            return MHD_NO;
        }
        *ptr = request;

        if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
            request->postProcessor = MHD_create_post_processor(connection, 1024, &postParamsIterator, request);
            if (request->postProcessor == NULL) {
                log(TPL_ERR, "Failed to setup post processor for '%s'", url);
                return MHD_NO;
            }
        }

        return MHD_YES;
    }

    if (request->session == NULL) {
        log(TPL_ERR, ">>>> request has no session. getSession()");
        request->session = getSession(connection);
        if (request->session == NULL) {
            log(TPL_ERR, "unable to set up session for '%s'\n", url);
            return MHD_NO;
        } else if (request->session->state == EXPIRED) {
            // is expired (or simply not present), so... ask for a new authentication
            log(TPL_ERR, ">>>> no ACTIVE session for session id => ask for authentication");
            return askForAuthentication(connection, ASK_REALM);
        }
    }

    log(TPL_ERR, ">>>> go on handling request");
    Session* session = request->session;
    session->start = time(NULL);
    if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
        log(TPL_DEBUG, "POST method\n");

        /* eval request post data */
        MHD_post_process(request->postProcessor, uploadData, *uploadDataSize);
        log(TPL_DEBUG,"POST data processed\n");


        if (*uploadDataSize != 0) {
            log(TPL_DEBUG,"upload data size = 0\n");
            *uploadDataSize = 0;
            return MHD_YES;
        }

        /* Ok, here we have done with POST data, now we can serve the response */
        log(TPL_DEBUG,"serving response to client\n");
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

        log(TPL_ERR, ">>>> routing request to %s", routes[i].url);
        result = routes[i].handler(routes[i].handlerCls, routes[i].mime, session, connection);
        if (result != MHD_YES) {
            log(TPL_ERR,"Error handling route to '%s'\n", url);
        }

        return result;
    }

    response = MHD_create_response_from_buffer(strlen(ERROR_ILLEGAL_REQUEST_PAGE), (void*)ERROR_ILLEGAL_REQUEST_PAGE,
                                               MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(response, MHD_HTTP_HEADER_SERVER, "ASK Server 1.0");

    result = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ACCEPTABLE, response);
    MHD_destroy_response(response);

    return result;
}

void requestCompletedCallback(void* cls, Connection* connection,
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

const char* getHeaderValue(Connection* connection, const char* headerName)
{
    //MHD_HTTP_HEADER_AUTHORIZATION
    return MHD_lookup_connection_value(connection, MHD_HEADER_KIND, headerName);

}

int addResponseHeader(Response* response, const char* headerName, const char* headerValue)
{
    return MHD_add_response_header(response, headerName,headerValue);
}

static int homeHandler(const void* cls, const char* mime, Session* session, Connection* connection)
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
    MHD_add_response_header(response, MHD_HTTP_HEADER_SERVER, "ASK Server 1.0");

    /* enqueue response for send */
    result = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return result;
}

static int notFoundHandler(const void* cls, const char* mime, Session* session, Connection* connection)
{
    Response* response = MHD_create_response_from_buffer(strlen(ERROR_NOT_FOUND_PAGE), (void*)ERROR_NOT_FOUND_PAGE,
                                                         MHD_RESPMEM_PERSISTENT);
    int result = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_ENCODING, mime);
    MHD_add_response_header(response, MHD_HTTP_HEADER_SERVER, "ASK Server 1.0");
    MHD_destroy_response(response);

    return result;

}

static int basicAuthHandler(const void* cls, const char* mime, Session* session, Connection* connection)
{
    int result;
    char* reply;
    Response* response;

    if (authenticate(connection, session) == AUTHENTICATED) {
        log(TPL_INFO, "user authenticated");
        log(TPL_INFO, "generate and assign Session ID");
        /* generate and assign Session ID */
        char* sessionUUID = generateSessionUUID();
        snprintf(session->id, sizeof(session->id), "%s", sessionUUID);
//        free(sessionUUID);
        session->rc++;
        session->start = time(NULL);
        log(TPL_INFO, "SessionID: %s", session->id);

        /* put the new session at the head (lifo) of the sessions list */
        session->next = sessions;
        sessions = session;

        /* finally, activate the session */
        session->state == ACTIVE;

        if (asprintf(&reply, "AUTHENTICATED") == -1) {
            /* TODO: check which error is better. Internal Server Error */
            return MHD_NO;
        }
    } else {
        /* user not authenticated or auth credentials not present... ask the client to perform authentication */
        return askForAuthentication(connection, ASK_REALM);
    }

    /* create the response, then add session cookie and Content-Type */
    response = MHD_create_response_from_buffer(strlen(reply), (void*)reply, MHD_RESPMEM_MUST_FREE);
    addSessionCookie(session, response);
    addResponseHeader(response, MHD_HTTP_HEADER_CONTENT_TYPE, mime);

    result = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return result;
}

static int sendAuthenticationResponse()
{
//    /* create the response, then add session cookie and Content-Type */
//    response = MHD_create_response_from_buffer(strlen(reply), (void*)reply, MHD_RESPMEM_MUST_FREE);
//    addSessionCookie(session, response);
//    addResponseHeader(response, MHD_HTTP_HEADER_CONTENT_TYPE, mime);
//
//    result = MHD_queue_response(connection, MHD_HTTP_OK, response);
//    MHD_destroy_response(response);
//
//    return result;
    return 0;
}

static int formBasedAuthHandler(const void* cls, const char* mime, Session* session, Connection* connection)
{
    return 0;
}

static int postParamsIterator(void* cls, enum MHD_ValueKind kind, const char* key, const char* fileName,
                              const char* contentType, const char* transferEncoding, const char* data,
                              uint64_t off, size_t size)
{
    Request* request = cls;
    /* TODO: check for request == NULL */
    Session* session = request->session;

    /* get the form param j_username */
    if (strcmp(J_USERNAME, key) == 0) {
        setSessionUsername(session, size, off, data);
        log(TPL_DEBUG, "j_username: '\%s'", getSessionUsername(session));
        return MHD_YES;
    }

    /* get the form param j_password */
    if (strcmp(J_PASSWORD, key) == 0) {
        setSessionPassword(session, size, off, data);
        log(TPL_DEBUG,"j_password: '\%s'", getSessionPassword(session));
        return MHD_YES;
    }

    fprintf(stderr, "Unsupported form value '%s'\n", key);
    return MHD_YES;
}

/* TODO: check for a better error management. Internal Server Error */
/*       instead of the followinf MHD_NO */
static int askForAuthentication(Connection* connection, const char* realm)
{
    int result;
    Response* response;
    char* headerValue;
    const char* basicRealmPrefix = "Basic realm=";

    response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    if (!response) {
        return MHD_NO;
    }

    headerValue = malloc(strlen(basicRealmPrefix) + strlen(realm) + 1);
    if (!headerValue) {
        return MHD_NO;
    }

    strcpy(headerValue, basicRealmPrefix);
    strcat(headerValue, realm);

    /* add the "WWW-Authenticate" header to the response */
    result = addResponseHeader(response, MHD_HTTP_HEADER_WWW_AUTHENTICATE, headerValue);
    free(headerValue);

    /* add an expired session cookie, to prevent resending cookie for expired sessions */
    addExpiredCookie(response);

    if (!result) {
        MHD_destroy_response(response);
        return MHD_NO;
    }

    result = MHD_queue_response(connection, MHD_HTTP_UNAUTHORIZED, response);
    MHD_destroy_response(response);

    return result;
}
