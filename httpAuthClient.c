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
#include <curl/curl.h>

static char* authUrl(const char* username)
{
    printf("authUrl()\n");
    if (globalConfig.http_auth_ssl) {
        printf("\nSSL enabled\n");
    } else {
        printf("\nSSL not enabled\n");
    }
    const char* proto = globalConfig.http_auth_ssl ? "https://" : "http://";
    printf("PROTO: %s\n", proto);
    printf("Base auth url: %s\n", globalConfig.http_auth_url);

    char *url = malloc(strlen(globalConfig.http_auth_url) +  strlen(username)- 1);

    printf("cerca di creare baseUrl\n");
    sprintf(url, globalConfig.http_auth_url, username);
    printf("baseUrl creata\n");
    printf("baseUrl: %s\n", url);

    return url;
}

int httpBasicAuthentication(const char* username, const char* basicAuth)
{
    char* url = authUrl(username);
    CURL *curl;
    CURLcode res;
    int result = 1;
    char* authorizationHeader;

    printf("\n------------------------------------------------\n");
    printf("performing Zimbra authentication http request:\n");
    printf("\turl:             %s\n", url);
    printf("\tusername:        %s\n", username);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *chunk = NULL;
        int headerLen = strlen("Authorization: ") + strlen(basicAuth) + 1;

        snprintf(authorizationHeader, headerLen, "Authorization: %s", basicAuth);
        printf("\tAuthorization Header -->%s\n", authorizationHeader);

        chunk = curl_slist_append(chunk, authorizationHeader);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_URL, url);

        //#ifdef SKIP_PEER_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        //#ifdef SKIP_HOSTNAME_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        printf("\n==> libCURL code: %d\n", res);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

//        long httpCode = 0;
//        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
//        printf("==> HTTP code: %d\n", httpCode);
//        if (httpCode == 200 /*&& res != CURLE_ABORTED_BY_CALLBACK*/) {
//            result = AUTHENTICATED;
//        } else{
//            result = NOT_AUTHENTICATED;
//        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    printf("------------------------------------------------\n");

    return result;
}