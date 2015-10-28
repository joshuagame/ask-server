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
**
** ===========================================================================
*/

#include "session.h"
#include <uuid/uuid.h>

Session* sessions = NULL;

char* generateSessionUUID()
{
    uuid_t uuid;
    char uuidString[37];

    uuid_generate_time(uuid);
    uuid_unparse_lower(uuid, &uuidString);
    uuidString[37] = '\0';
    log(TPL_DEBUG, "generated Session UUID: %s, len: %d", uuidString, strlen(uuidString));

    return uuidString;
}

Session* getSession(struct MHD_Connection* connection)
{
    Session* session;
    const char* cookie;

    /*
     * Checking for session id from session cookie:
     *   - if here we have a session cookie, grab the session id and check for it in the active sessions list
     *   -    if we cannot find session id in the list than here we have an expired session => askForAuth
     *   -    if we found the session id in the list we have an authenticated session
     *   - if we have NO session cookie here than generate a new session id
     */
    if (cookie = getSessionCookie(connection)) {
        log(TPL_DEBUG, "checking sessions for ASKSESSION %s", cookie);
        session = sessions;
        while (session != NULL) {
            if (strcmp(cookie, session->id) == 0) break;
            session = session->next;
        }
        if (session != NULL) {
            if (session->state == ACTIVE) {
                log(TPL_INFO, "an active session exists for ASKSESSION %s", cookie);
                session->rc++;
                return session;
            }
        }

        // returning a NULL session the caller will have to ask for a new authentication from the client
        log(TPL_ERR, "no ACTIVE session for cookie %s", cookie);
        session = calloc(1, sizeof(Session));
        session->state = EXPIRED;
        return session;

    }

    /* create a new session */
    session = calloc(1, sizeof(Session));
    session->state = STARTED;
    if (session == NULL) {
        perror("unable to alloc session structure for the request\n");
        return NULL;
    }

    return session;
}

/* TODO: set expires attribute starting from session->expire time */
void addSessionCookie(Session* session, Response* response)
{
    time_t rawtime;
    struct tm expirationTime;
    char expirationTimeBuffer[80];

    time(&rawtime);
    expirationTime = *localtime(&rawtime);
    expirationTime.tm_sec += 3600;
    mktime(&expirationTime);
    strftime(expirationTimeBuffer, 80, "%a, %d-%b-%Y %X GMT", &expirationTime);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s=%s;expires=%s;path=/", ASK_COOKIE_NAME, session->id, expirationTimeBuffer);
    if (MHD_add_response_header(response, MHD_HTTP_HEADER_SET_COOKIE, buffer) == MHD_NO) {
        perror("Unable to set session cookie header.\n");
    }
}

void addExpiredCookie(Response* response)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s=deleted;path=/;expires=Thu, 01 Jan 1970 00:00:00 GMT", ASK_COOKIE_NAME);
    if (MHD_add_response_header(response, MHD_HTTP_HEADER_SET_COOKIE, buffer) == MHD_NO) {
        perror("Unable to set session cookie header.\n");
    }
}

const char* getSessionCookie(Connection* connection)
{
    return MHD_lookup_connection_value(connection, MHD_COOKIE_KIND, ASK_COOKIE_NAME);
}

/* TODO: try and unify the following into one function */
void setSessionUsername(Session* session, size_t size, uint64_t offset, const char* data)
{
    if (size + offset > sizeof(session->fcred.username)) {
        size = sizeof(session->fcred.username) - offset;
    }
    memcpy(&session->fcred.username[offset], data, size);
    if (size + offset < sizeof(session->fcred.username)) {
        session->fcred.username[size + offset] = '\0';
    }
}

void setSessionPassword(Session* session, size_t size, uint64_t offset, const char* data)
{
    if (size + offset > sizeof(session->fcred.password)) {
        size = sizeof(session->fcred.password) - offset;
    }
    memcpy(&session->fcred.password[offset], data, size);
    if (size + offset < sizeof(session->fcred.password)) {
        session->fcred.password[size + offset] = '\0';
    }
}

const char* getSessionUsername(Session* session)
{
    return session->fcred.username;
}

const char* getSessionPassword(Session* session)
{
    return session->fcred.password;
}