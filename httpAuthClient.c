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
*/

#include "httpAuthClient.h"
#include <curl/curl.h>

static char* auth_url(const char* username)
{
//    const char* proto = globalConfig.http_auth_ssl ? "https://" : "http://";
        char* url = malloc(strlen(global_config.http_auth_url) + strlen(username) - 1);
        sprintf(url, global_config.http_auth_url, username);

        return url;
}

int http_basic_authentication(const char* username, const char* basicAuth)
{
        char* url = auth_url(username);
        CURL* curl;
        CURLcode res;
        int result = 1;

        asklog(TPL_DEBUG, "---------------------------------------------------------------");
        asklog(TPL_DEBUG, "performing Zimbra authentication http request:");
        asklog(TPL_DEBUG, "url: %s", url);

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl) {
                struct curl_slist* chunk = NULL;
                int len = strlen("Authorization: ") + strlen(basicAuth) + 1;

                char* authorizationHeader;
                authorizationHeader = malloc(len);
                snprintf(authorizationHeader, len, "Authorization: %s", basicAuth);
                asklog(TPL_DEBUG, "%s", authorizationHeader);

                chunk = curl_slist_append(chunk, authorizationHeader);

                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

                /* Perform the request, res will get the return code */
                res = curl_easy_perform(curl);
                /* Check for errors */
                if (res != CURLE_OK)
                        asklog(TPL_ERR, "curl_easy_perform() failed: %s", curl_easy_strerror(res));

                /* and get HTTP response code */
                long httpCode = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
                asklog(TPL_DEBUG, "Zimbra authentication result code: %d", httpCode);
                result = httpCode == 200 && res != CURLE_ABORTED_BY_CALLBACK ? AUTHENTICATED : NOT_AUTHENTICATED;

                /* always cleanup */
                curl_easy_cleanup(curl);
        }

        curl_global_cleanup();
        asklog(TPL_DEBUG, "---------------------------------------------------------------");

        return result;
}