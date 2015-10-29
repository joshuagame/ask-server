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
** authentication.c
** ASK Server Authentication logic
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

#include "ask.h"
#include "base64.h"

static unsigned int extract_username(const char* basic_auth, char** username)
{
        unsigned int len = strlen(basic_auth);
        unsigned int i = 0;
        for (i = 0; i < len; i++) {
                if (basic_auth[i] == ':') {
                        break;
                }
        }

        if (i < len) {
                *username = (char*) malloc(i);
                (*username)[i] = '\0';
                strncpy(*username, basic_auth, i);
        }

        return i;
}

static unsigned int extract_authentication_data(const char* authentication_header_value, char** authentication_data)
{
        unsigned int i = 0;
        char* line = strdup(authentication_header_value);
        char* basic = strtok(line, " ");
        char* data = strtok(NULL, " ");
        *authentication_data = data;
        return i;
}

static int basic_authentication(connection_t* connection)
{
        const char* authentication_header_value;
        const char* basic_prefix = "Basic ";
        int authenticated;

        authentication_header_value = get_header_value(connection, MHD_HTTP_HEADER_AUTHORIZATION);
        asklog(TPL_DEBUG, "Authorization header: %s", authentication_header_value);

        /* no Basic info at all */
        if (authentication_header_value == NULL) {
                return NO_BASIC_AUTH_INFO;
        }

        /* malformed "Authorization" header value */
        if (strncmp(authentication_header_value, basic_prefix, strlen(basic_prefix)) != 0) {
                return 0;
        }

        /* extract the authentication data from Authorization header */
        char* authenticationData;
        extract_authentication_data(authentication_header_value, &authenticationData);

        /* decode the authentication data */
        char* base64_decoded_output;
        size_t decodedSize = 0;
        Base64Decode(authenticationData, &base64_decoded_output, &decodedSize);

        /* extract the username */
        char* username;
        size_t ulen = extract_username(base64_decoded_output, &username);
        asklog(TPL_DEBUG, "username: %s", username);

        if (username == NULL) {
                return 0;
        }

        /* perform Zimbra authentication */
        authenticated = http_basic_authentication(username, authentication_header_value);
        return authenticated;
}

/* TODO: implement this!!! */
static int form_based_authentication(connection_t* connection, session_t* session)
{
//    const char* username = getSessionUsername(session);
//    const char* password = getSessionPassword(session);
//
//    if (strlen(username) == 0 || strlen(password) == 0) {
//        return NOT_AUTHENTICATED;
//    }
//
////    if (strcmp(username, testUsername) == 0 && strcmp(password, testPassword) == 0) {
////        return AUTHENTICATED;
////    }

        return NOT_AUTHENTICATED;
}

int authenticate(connection_t* connection, session_t* session)
{
        /*
         * no cookie has been found or the session has expired, so check for authentication credentials:
         * here we first try for Basic Authentication and if there are no Basic Auth info, then we check for
         * FORM-Based username and password (here we have username and password in session because of the post iterator)
         */
        asklog(TPL_INFO, "Authenticating");
        int auth = basic_authentication(connection);
        if (auth == NO_BASIC_AUTH_INFO) {
                auth = form_based_authentication(connection, session);
        }

        return auth;
}

