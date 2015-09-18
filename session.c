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

#include "ask.h"

Session* getSession(struct MHD_Connection* connection)
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

void addSessionCookie(Session* session, Response* response)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s=%s", ASK_COOKIE_NAME, session->id);
    if (MHD_add_response_header(response, MHD_HTTP_HEADER_SET_COOKIE, buffer) == MHD_NO) {
        perror("Unable to set session cookie header.\n");
    }
}

const char* getSessionCookie(Connection* connection)
{
    return MHD_lookup_connection_value(connection, MHD_COOKIE_KIND, ASK_COOKIE_NAME);
}