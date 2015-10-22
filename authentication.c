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

const char* testUsername = TEST_USER;
const char* testPassword = TEST_PASSWORD;

//static char* extractUsername(const char* basicAuth)
//{
//    char* username;
//
//    int i = 0;
//    for (i = 0; i < sizeof(basicAuth); i++) {
//        if (basicAuth[i] == ':') {
//            break;
//        }
//    }
//
//    if (i < sizeof(basicAuth)) {
//        username = malloc(sizeof(char)*i + 1);
//        int k = 0;
//        for (k = 0; k < i; k++) {
//            username[k] = basicAuth[i];
//        }
//        printf("username: %s", username);
//    }
//
//    return username;
//}

static int basicAuthentication(Connection* connection)
{
    const char* authorizationHeaderValue;
    unsigned char* expectedB64 = NULL;
    unsigned char* expected;
    const char* basicPrefix = "Basic ";
    int authenticated;
    Session* session;

    authorizationHeaderValue = getHeaderValue(connection, MHD_HTTP_HEADER_AUTHORIZATION);
    printf("authorizationHeaderValue: %s\n", authorizationHeaderValue);

    /* no Basic info at all */
    if (authorizationHeaderValue == NULL) {
        return NO_BASIC_AUTH_INFO;
    }

    /* malformed "Authorization" header value */
    if (strncmp(authorizationHeaderValue, basicPrefix, strlen(basicPrefix)) != 0) {
        return 0;
    }

    /* allocate the string for the expected Basic Auth Header value */
    if ((expected = malloc(strlen(testUsername) + 1 + strlen(testPassword) + 1)) == NULL) {
        return 0;
    }

    /* create the expected value and encode it into its Base64 representation */
    strcpy(expected, testUsername);
    strcat(expected, ":");
    strcat(expected, testPassword);

    //Base64encode(expectedB64, expected, sizeof(expected));
    //Base64Encode(expected, strlen(expected), &expectedB64);
    size_t* dlen = 0;
    base64_decode(expectedB64, dlen, expected, sizeof(expected));
    free(expected);

    printf("expected: %s\n", expectedB64);

    if (expectedB64 == NULL) {
        return 0;
    }

    printf("decoding auth info:\n");
    unsigned char* base64DecodeOutput;
    size_t* decodedSize = 0;
    //Base64decode(base64DecodeOutput, expectedB64);
//    Base64Decode(expectedB64, &base64DecodeOutput, &decodedSize);
    base64_decode(base64DecodeOutput, decodedSize, expectedB64, sizeof(expectedB64));
    printf("Output: %s %d\n", base64DecodeOutput, decodedSize);

//    extractUsername(expectedB64);

//    httpBasicAuthentication(expectedB64);


    authenticated = (strcmp(authorizationHeaderValue + strlen(basicPrefix), expectedB64) == 0);
    free(expectedB64);

    return authenticated ? AUTHENTICATED : NOT_AUTHENTICATED;
}

static int formBasedAuthentication(Connection* connection, Session* session)
{
    printf("\n\nformBasedAuthentication()");
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
    printf("--------Authenticate()\n");
    /* if we found the ASKSESSION cookie, the user is authenticated */
    if (getSessionCookie(connection) != NULL) {
        printf("--------session ok\n");
        return AUTHENTICATED;
    }

    /*
     * if no cookie has been found, check for authentication credentials:
     * here we first try for Basic Authentication and if there are no Basic Auth info, then we check for
     * FORM-Based username and password (here we had username and password in session because of the post iterator)
     */
    int auth = basicAuthentication(connection);
    if (auth == NO_BASIC_AUTH_INFO) {
        auth = formBasedAuthentication(connection, session);
    }

    return auth;
}

