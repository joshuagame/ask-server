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

#define BASE64_LOOKUP_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

const char* testUsername = TEST_USER;
const char* testPassword = TEST_PASSWORD;


static char* base64Encode(const char* str)
{
    int i;
    unsigned long l;
    char* encoded;
    size_t strLength = strlen(str);

    if ((encoded = malloc(strLength * 2)) == NULL) {
        return encoded;
    }

    encoded[0] = 0;
    for (i = 0; i < strLength; i += 3) {
        l = (((unsigned long)str[i]) << 16)
            | (((i + 1) < strLength) ? (((unsigned long) str[i + 1]) << 8) :0)
            | (((i + 2) < strLength) ? ((unsigned long) str[i + 2]) :0);

        strncat(encoded, &BASE64_LOOKUP_CHARS[(l >> 18) & 0x3F], 1);
        strncat(encoded, &BASE64_LOOKUP_CHARS[(l >> 12) & 0x3F], 1);
        if (i + 1 < strLength) {
            strncat(encoded, &BASE64_LOOKUP_CHARS[(l >> 6) & 0x3F], 1);
        }
        if (i + 2 < strLength) {
            strncat(encoded, &BASE64_LOOKUP_CHARS[l & 0x3F], 1);
        }
    }

    if (strLength % 3) {
        strncat(encoded, "===", 3 - strLength % 3);
    }

    return encoded;
}

static int basicAuthentication(Connection* connection)
{
    const char* authorizationHeaderValue;
    char* expectedB64;
    char* expected;
    const char* basicPrefix = "Basic ";
    int authenticated;
    Session* session;

    authorizationHeaderValue = getHeaderValue(connection, MHD_HTTP_HEADER_AUTHORIZATION);

    /* no Basic info at all */
    if (authorizationHeaderValue == NULL) {
        return 0;
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

    expectedB64 = base64Encode(expected);
    free(expected);

    if (expectedB64 == NULL) {
        return 0;
    }

    authenticated = (strcmp(authorizationHeaderValue + strlen(basicPrefix), expectedB64) == 0);
    free(expectedB64);

    return authenticated ? AUTHENTICATED : NOT_AUTHENTICATED;
}

int authenticate(Connection* connection)
{
    /* if we found the ASKSESSION cookie, the user is authenticated */
    if (getSessionCookie(connection) != NULL) {
        return AUTHENTICATED;
    }

    /* if no cookie has been found, check for authentication credentials */
    return basicAuthentication(connection);
}

