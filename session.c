/*
**     _    ____  _  __
**    / \  / ___|| |/ /  ASK
**   / _ \ \___ \| ' /   Authentication Sessions Keeper Server
**  / ___ \ ___) | . \   -------------------------------------
** /_/   \_\____/|_|\_\  www.codegazoline.it/ask
**                 v0.1
** ===============================================================================
** Copyright (C) 2015, Luca Stasio - joshuagame@gmail.com //The CodeGazoline Team/
**
** session.c
** ASK Server session management functionalities
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
*/

#include "session.h"
#include <uuid/uuid.h>

session_t* sessions = NULL;

char* generate_session_id()
{
        uuid_t uuid;
        char session_id[37];

        uuid_generate_time(uuid);
        uuid_unparse_lower(uuid, &session_id);
        session_id[37] = '\0';
        asklog(TPL_DEBUG, "generated session id: %s, len: %d", session_id, strlen(session_id));

        return session_id;
}

session_t* get_session(struct MHD_Connection* connection)
{
        session_t* session;
        const char* cookie;

        /*
         * Checking for session id from session cookie:
         *   - if here we have a session cookie, grab the session id and check for it in the active sessions list
         *   -    if we cannot find session id in the list than here we have an expired session => askForAuth
         *   -    if we found the session id in the list we have an authenticated session
         *   - if we have NO session cookie here than generate a new session id
         */
        if (cookie = get_session_cookie(connection)) {
                asklog(TPL_DEBUG, "checking sessions for ASKSESSION %s", cookie);
                HASH_FIND_STR(sessions, cookie, session);
                if (session != NULL) {
                        if (session->state == ACTIVE) {
                                asklog(TPL_INFO, "an active session exists for ASKSESSION %s", cookie);
                                session->rc++;
                                return session;
                        }
                }

                // returning a NULL session the caller will have to ask for a new authentication from the client
                asklog(TPL_ERR, "no ACTIVE session for cookie %s", cookie);
                session = calloc(1, sizeof(session_t));
                session->state = EXPIRED;
                return session;

        }

        /* create a new session */
        session = calloc(1, sizeof(session_t));
        session->state = STARTED;
        time_t now;
        time(&now);
        session->expiration = now + 3600;

        if (session == NULL) {
                perror("unable to alloc session structure for the request\n");
                return NULL;
        }

        return session;
}

/* TODO: set expires attribute starting from session->expire time */
void add_session_cookie(session_t* session, response_t* response)
{
        time_t rawtime = session->expiration;
        struct tm expiration_time;
        char expiration_time_buffer[80];

        expiration_time = *localtime(&rawtime);
        expiration_time.tm_sec += 3600;
        mktime(&expiration_time);
        strftime(expiration_time_buffer, 80, "%a, %d-%b-%Y %X GMT", &expiration_time);

        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s=%s;expires=%s;path=/", ASK_COOKIE_NAME, session->id,
                 expiration_time_buffer);
        if (MHD_add_response_header(response, MHD_HTTP_HEADER_SET_COOKIE, buffer) == MHD_NO) {
                perror("Unable to set session cookie header.\n");
        }
}

void add_expired_cookie(response_t* response)
{
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s=deleted;path=/;expires=Thu, 01 Jan 1970 00:00:00 GMT", ASK_COOKIE_NAME);
        if (MHD_add_response_header(response, MHD_HTTP_HEADER_SET_COOKIE, buffer) == MHD_NO) {
                perror("Unable to set session cookie header.\n");
        }
}

const char* get_session_cookie(connection_t* connection)
{
        return MHD_lookup_connection_value(connection, MHD_COOKIE_KIND, ASK_COOKIE_NAME);
}

/* TODO: try and unify the following into one function */
void set_session_username(session_t* session, size_t size, uint64_t offset, const char* data)
{
        if (size + offset > sizeof(session->fcred.username)) {
                size = sizeof(session->fcred.username) - offset;
        }
        memcpy(&session->fcred.username[offset], data, size);
        if (size + offset < sizeof(session->fcred.username)) {
                session->fcred.username[size + offset] = '\0';
        }
}

void set_session_password(session_t* session, size_t size, uint64_t offset, const char* data)
{
        if (size + offset > sizeof(session->fcred.password)) {
                size = sizeof(session->fcred.password) - offset;
        }
        memcpy(&session->fcred.password[offset], data, size);
        if (size + offset < sizeof(session->fcred.password)) {
                session->fcred.password[size + offset] = '\0';
        }
}

const char* get_session_username(session_t* session)
{
        return session->fcred.username;
}

const char* get_session_password(session_t* session)
{
        return session->fcred.password;
}