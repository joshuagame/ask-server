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

int request_handler(void *cls, connection_t *connection, const char *url, const char *method,
                    const char *version, const char *upload_data, size_t *upload_data_size, void **ptr)
{
        response_t *response;
        request_t *request;
        int result;

        if ((request = *ptr) == NULL) {
                if ((request = calloc(1, sizeof(request_t))) == NULL) {
                        log(TPL_ERR, "unable to alloc request structure");
                        return MHD_NO;
                }
                *ptr = request;

                if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
                        request->post_processor = MHD_create_post_processor(connection, 1024, &post_params_iterator,
                                                                            request);
                        if (request->post_processor == NULL) {
                                log(TPL_ERR, "Failed to setup post processor for '%s'", url);
                                return MHD_NO;
                        }
                }

                return MHD_YES;
        }

        if (request->session == NULL) {
                request->session = get_session(connection);
                if (request->session == NULL) {
                        log(TPL_ERR, "unable to set up session for '%s'\n", url);
                        return MHD_NO;
                } else if (request->session->state == EXPIRED) {
                        // is expired (or simply not present), so... ask for a new authentication
                        log(TPL_ERR, "session expired or null");
                        return ask_for_authentication(connection, ASK_REALM);
                } else if (request->session->state == ACTIVE) {
                        // OK, here we go with an authenticated active session
                        char *body;
                        if (asprintf(&body, res_auth_ok) == -1) {
                                return MHD_NO;
                        }
                        return send_authentication_response(connection, request->session, body,
                                                            "application/json; charset=utf-8");
                }
        }

        session_t *session = request->session;
        session->start = time(NULL);
        if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
                log(TPL_DEBUG, "POST method\n");

                /* eval request post data */
                MHD_post_process(request->post_processor, upload_data, *upload_data_size);
                log(TPL_DEBUG, "POST data processed\n");


                if (*upload_data_size != 0) {
                        log(TPL_DEBUG, "upload data size = 0\n");
                        *upload_data_size = 0;
                        return MHD_YES;
                }

                /* Ok, here we have done with POST data, now we can serve the response */
                log(TPL_DEBUG, "serving response to client\n");
                MHD_destroy_post_processor(request->post_processor);
                request->post_processor = NULL;

                /* here we perform an internal fake GET request to the POST url */
                method = MHD_HTTP_METHOD_GET;
                if (request->post_url != NULL) {
                        url = request->post_url;
                }
        }

        if ((strcmp(method, MHD_HTTP_METHOD_GET) == 0) || (strcmp(method, MHD_HTTP_METHOD_HEAD) == 0)) {
                /* find route by url. TODO: improve this using an HashMap instead of an array for routes */
                unsigned int i = 0;
                while ((routes[i].url != NULL) && (strcmp(routes[i].url, url) != 0)) {
                        i++;
                }

                log(TPL_ERR, ">>>> routing request to %s", routes[i].url);
                result = routes[i].handler(routes[i].handler_cls, routes[i].mime, session, connection);
                if (result != MHD_YES) {
                        log(TPL_ERR, "Error handling route to '%s'\n", url);
                }

                return result;
        }

        response = MHD_create_response_from_buffer(strlen(ERROR_ILLEGAL_REQUEST_PAGE),
                                                   (void *) ERROR_ILLEGAL_REQUEST_PAGE,
                                                   MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, MHD_HTTP_HEADER_SERVER, "ASK Server 1.0");

        result = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ACCEPTABLE, response);
        MHD_destroy_response(response);

        return result;
}

void request_completed_callback(void *cls, connection_t *connection,
                                void **con_cls, enum MHD_RequestTerminationCode toe)
{
        request_t *request = *con_cls;

        if (request != NULL) {
                return;
        }

        if (request->session != NULL) {
                request->session->rc--;
        }
        if (request->post_processor != NULL) {
                MHD_destroy_post_processor(request->post_processor);
        }

        free(request);
}

const char *get_header_value(connection_t *connection, const char *headerName)
{
        //MHD_HTTP_HEADER_AUTHORIZATION
        return MHD_lookup_connection_value(connection, MHD_HEADER_KIND, headerName);

}

int add_response_header(response_t *response, const char *headerName, const char *headerValue)
{
        return MHD_add_response_header(response, headerName, headerValue);
}

static int home_handler(const void *cls, const char *mime, session_t *session, connection_t *connection)
{
        int result;
        const char *htmlContent = cls;
        char *responseContent;
        response_t *response;

        if (asprintf(&responseContent, "%s", htmlContent) == -1) {
                return MHD_NO;
        }

        /* prepare the response */
        response = MHD_create_response_from_buffer(strlen(responseContent), (void *) responseContent,
                                                   MHD_RESPMEM_MUST_FREE);
        MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_ENCODING, mime);
        MHD_add_response_header(response, MHD_HTTP_HEADER_SERVER, "ASK Server 1.0");

        /* enqueue response for send */
        result = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);

        return result;
}

static int not_found_handler(const void *cls, const char *mime, session_t *session, connection_t *connection)
{
        response_t *response;

        response = MHD_create_response_from_buffer(strlen(ERROR_NOT_FOUND_PAGE), (void *) ERROR_NOT_FOUND_PAGE,
                                                   MHD_RESPMEM_PERSISTENT);
        int result = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
        MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_ENCODING, mime);
        MHD_add_response_header(response, MHD_HTTP_HEADER_SERVER, "ASK Server 1.0");
        MHD_destroy_response(response);

        return result;

}

static int basic_auth_handler(const void *cls, const char *mime, session_t *session, connection_t *connection)
{
        int result;

        if (authenticate(connection, session) == AUTHENTICATED) {
                log(TPL_INFO, "user authenticated");
                log(TPL_INFO, "generate and assign Session ID");

                /* generate and assign Session ID */
                char *sessionUUID = generate_session_id();
                snprintf(session->id, sizeof(session->id), "%s", sessionUUID);
                session->rc++;
                session->start = time(NULL);

                /* activate the session */
                session->state = ACTIVE;
                log(TPL_INFO, "SessionID: %s - State: %d", session->id, session->state);

                /* save the new session into the Sessions hash table */
                HASH_ADD_STR(sessions, id, session);
                log(TPL_DEBUG, "there are %u active sessions", HASH_COUNT(sessions));

                char *body;
                if (asprintf(&body, res_auth_ok) == -1) {
                        /* TODO: check which error is better. Internal Server Error */
                        return MHD_NO;
                }

                result = send_authentication_response(connection, session, body, mime);
        } else {
                /* user not authenticated or auth credentials not present... ask the client to perform authentication */
                result = ask_for_authentication(connection, ASK_REALM);
        }

        return result;
}

/* create the response, then add session cookie and Content-Type */
static int sendAuthenticationResponse(connection_t *connection, session_t *session, char *body, const char *mime)
{
        response_t *response;
        response = MHD_create_response_from_buffer(strlen(body), (void *) body, MHD_RESPMEM_MUST_FREE);
        add_response_header(response, MHD_HTTP_HEADER_SERVER, "ASK Server 1.0");

        if (session != NULL) {
                add_session_cookie(session, response);
        }

        if (mime != NULL) {
                add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, mime);
        }

        int result = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);

        return result;
}

static int formBasedAuthHandler(const void *cls, const char *mime, session_t *session, connection_t *connection)
{
        return 0;
}

static int postParamsIterator(void *cls, enum MHD_ValueKind kind, const char *key, const char *fileName,
                              const char *contentType, const char *transferEncoding, const char *data,
                              uint64_t off, size_t size)
{
        request_t *request = cls;
        /* TODO: check for request == NULL */
        session_t *session = request->session;

        /* get the form param j_username */
        if (strcmp(ASK_UNAME, key) == 0) {
                set_session_username(session, size, off, data);
                log(TPL_DEBUG, "j_username: %s", get_session_username(session));
                return MHD_YES;
        }

        /* get the form param j_password */
        if (strcmp(ASK_PWD, key) == 0) {
                set_session_password(session, size, off, data);
                log(TPL_DEBUG, "j_password: %s", get_session_password(session));
                return MHD_YES;
        }

        fprintf(stderr, "Unsupported form value '%s'\n", key);
        return MHD_YES;
}

/* TODO: check for a better error management. Internal Server Error */
/*       instead of the followinf MHD_NO */
static int askForAuthentication(connection_t *connection, const char *realm)
{
        int result;
        response_t *response;
        char *headerValue;
        const char *basicRealmPrefix = "Basic realm=";

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
        result = add_response_header(response, MHD_HTTP_HEADER_WWW_AUTHENTICATE, headerValue);
        free(headerValue);

        /* add an expired session cookie, to prevent resending cookie for expired sessions */
        add_expired_cookie(response);

        if (!result) {
                MHD_destroy_response(response);
                return MHD_NO;
        }

        result = MHD_queue_response(connection, MHD_HTTP_UNAUTHORIZED, response);
        MHD_destroy_response(response);

        return result;
}
