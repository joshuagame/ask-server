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
//    const char* proto = globalConfig.http_auth_ssl ? "https://" : "http://";
    char *url = malloc(strlen(globalConfig.http_auth_url) +  strlen(username)- 1);
    sprintf(url, globalConfig.http_auth_url, username);

    return url;
}

int httpBasicAuthentication(const char* username, const char* basicAuth)
{
    char* url = authUrl(username);
    CURL *curl;
    CURLcode res;
    int result = 1;

    tp_log_write(TPL_DEBUG, "---------------------------------------------------------------");
    tp_log_write(TPL_DEBUG, "performing Zimbra authentication http request:");
    tp_log_write(TPL_DEBUG, "url: %s\n", url);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *chunk = NULL;
        int len = strlen("Authorization: ") + strlen(basicAuth) + 1;

        char* authorizationHeader;
        authorizationHeader = malloc(len);
        snprintf(authorizationHeader, len, "Authorization: %s", basicAuth);
        tp_log_write(TPL_DEBUG, "%s\n", authorizationHeader);

        chunk = curl_slist_append(chunk, authorizationHeader);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            tp_log_write(TPL_ERR, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        /* and get HTTP response code */
        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        tp_log_write(TPL_DEBUG, "Zimbra authentication result code: %d\n", httpCode);
        result = httpCode == 200 && res != CURLE_ABORTED_BY_CALLBACK ? AUTHENTICATED : NOT_AUTHENTICATED;

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    tp_log_write(TPL_DEBUG, "---------------------------------------------------------------");

    return result;
}