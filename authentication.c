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
**
** ===========================================================================
*/

#include "ask.h"
#include "base64.h"
#include "httpAuthClient.h"

const char* testUsername = TEST_USER;
const char* testPassword = TEST_PASSWORD;

static unsigned int extractUsername(const char* basicAuth, char** username)
{
    unsigned int len = strlen(basicAuth);
    unsigned int i = 0;
    for (i = 0; i < len; i++) {
        if (basicAuth[i] == ':') {
            break;
        }
    }

    if (i < len) {
        *username = (char*)malloc(i);
        (*username)[i] = '\0';
        strncpy(*username, basicAuth, i);
    }

    return i;
}

static unsigned int extractAuthenticationData(const char* authenticationHeaderValue, char** authenticationData)
{
    unsigned int i = 0;
    char *line = strdup(authenticationHeaderValue);
    char *basic = strtok(line, " ");
    log(TPL_DEBUG, "basic: %s\n", basic);
    char *data = strtok(NULL, " ");
    log(TPL_DEBUG, "data: %s\n", data);

    *authenticationData = data;

    return i;
}

static int basicAuthentication(Connection* connection)
{
    const char* authorizationHeaderValue;
    char* expectedB64;
    unsigned char* expected;
    const char* basicPrefix = "Basic ";
    int authenticated;
    Session* session;

    authorizationHeaderValue = getHeaderValue(connection, MHD_HTTP_HEADER_AUTHORIZATION);
    log(TPL_DEBUG,"Authorization header: %s", authorizationHeaderValue);


    /* no Basic info at all */
    if (authorizationHeaderValue == NULL) {
        return NO_BASIC_AUTH_INFO;
    }

    /* malformed "Authorization" header value */
    if (strncmp(authorizationHeaderValue, basicPrefix, strlen(basicPrefix)) != 0) {
        return 0;
    }

    /* extract the authentication data from Authorization header */
    char* authenticationData;
    extractAuthenticationData(authorizationHeaderValue, &authenticationData);

    /* decode the authentication data */
    char* base64DecodeOutput;
    size_t decodedSize = 0;
    Base64Decode(authenticationData, &base64DecodeOutput, &decodedSize);

    /* extract the username */
    char* username;
    size_t ulen = extractUsername(base64DecodeOutput, &username);
    log(TPL_DEBUG, "username: %s", username);

    if (username == NULL) {
        return 0;
    }

    /* performs Zimbra authentication */
    authenticated =  httpBasicAuthentication(username, authorizationHeaderValue);
    return authenticated;
}

static int formBasedAuthentication(Connection* connection, Session* session)
{
    const char* username = getSessionUsername(session);
    const char* password = getSessionPassword(session);

    if (strlen(username) == 0 || strlen(password) == 0) {
        return NOT_AUTHENTICATED;
    }

    if (strcmp(username, testUsername) == 0 && strcmp(password, testPassword) == 0) {
        return AUTHENTICATED;
    }

    return NOT_AUTHENTICATED;
}

int authenticate(Connection* connection, Session* session)
{
//    char* sessionCookieValue;

//    /* if we found the ASKSESSION cookie, the user is authenticated */
//    if ((sessionCookieValue = getSessionCookie(connection)) != NULL) {
//        log(TPL_DEBUG, "session [%s] found", sessionCookieValue);
//        return AUTHENTICATED;
//    }

    /*
     * if no cookie has been found or the session has expired, so check for authentication credentials:
     * here we first try for Basic Authentication and if there are no Basic Auth info, then we check for
     * FORM-Based username and password (here we have username and password in session because of the post iterator)
     */
    log(TPL_INFO, "Authenticating");
    int auth = basicAuthentication(connection);
    if (auth == NO_BASIC_AUTH_INFO) {
        auth = formBasedAuthentication(connection, session);
    }

    return auth;
}

