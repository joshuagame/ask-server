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
** httpAuthClient.c
** HTTP authentication client (actually authenticate against Zimbra server)
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

#include "httpAuthClient.h"

static char* authUrl(const char* username)
{
    printf("authUrl()\n");
    const char* proto = globalConfig.http_auth_ssl ? "https://" : "http://";

    char *url = malloc(strlen(globalConfig.http_auth_url) + strlen(username) - 1);
    printf("cerca di creare baseUrl\n");
    snprintf(url, sizeof(url), globalConfig.http_auth_url, username);
    printf("baseUrl creata\n");
    printf("baseUrl: %s\n", url);

    return url;
}

int httpBasicAuthentication(const char* username, const char* basicAuth)
{
    printf("httpBsicAuthentication()\n");
    char* url = authUrl(username);
    printf("BaseURL: %s\n", url);

    return 0;
}