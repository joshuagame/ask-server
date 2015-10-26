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

static char* generateSessionUUID()
{
    uuid_t uuid;
    char* uuidString = malloc(37);

//    uuid_generate_time(uuid);
    uuid_generate_time_safe(uuid);
    uuid_unparse_lower(uuid, uuidString);
    uuidString[strlen(uuidString)] = '\0';
    tp_log_write(TPL_DEBUG, "generated Session UUID: %s, len: %d", uuidString, strlen(uuidString));

    return uuidString;
}

Session* getSession(struct MHD_Connection* connection)
{
    Session* session;
    const char* cookie;

    /* search for an existing session for this connection */
    if ((cookie = MHD_lookup_connection_value(connection, MHD_COOKIE_KIND, ASK_COOKIE_NAME)) != NULL) {
        tp_log_write(TPL_DEBUG, "checking sessions for ASKSESSION %s", cookie);
        session = sessions;
        while (session != NULL) {
            if (strcmp(cookie, session->id) == 0) break;
            session = session->next;
        }
        if (session != NULL) {
            tp_log_write(TPL_INFO, "an active session exists for ASKSESSION %s", cookie);
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

    unsigned int v1 = (unsigned int)random();
    unsigned int v2 = (unsigned int)random();
    unsigned int v3 = (unsigned int)random();
    unsigned int v4 = (unsigned int)random();
    char* sessionUUID = generateSessionUUID();
    snprintf(session->id, sizeof(session->id), "%s", sessionUUID);
    free(sessionUUID);
    session->rc++;
    session->start = time(NULL);

    /* put the new session at the head (lifo) of the sessions list */
    session->next = sessions;
    sessions = session;

    tp_log_write(TPL_INFO, "session started for connection (session UUID: %s)", session->id);

    return session;
}

void addSessionCookie(Session* session, Response* response)
{
    time_t rawtime;
    struct tm expirationTime;
    char expirationTimeBuffer[80];

    time(&rawtime);
    expirationTime = *localtime(&rawtime);
    expirationTime.tm_sec += 3600;
    mktime(&expirationTime);

    // DAY, DD-MMM-YYYY HH:MM:SS GMT
    // Sat, 01-Jan-2000 00:00:00 GMT
    strftime(expirationTimeBuffer, 80, "%a, %d-%b-%Y %X GMT", &expirationTime);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s=%s;expires=%s;path=/", ASK_COOKIE_NAME, session->id, expirationTimeBuffer);
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