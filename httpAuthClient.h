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
** httpAuthClient.h
** header for HTTP authentication client (actually authenticate against Zimbra server)
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

#ifndef ASK_SERVER_HTTP_AUTH_CLIENT_H
#define ASK_SERVER_HTTP_AUTH_CLIENT_H

#include "ask.h"

static char* authUrl(const char* username);

//char* sessionId;
//unsigned int v1 = (unsigned int)random();
//unsigned int v2 = (unsigned int)random();
//unsigned int v3 = (unsigned int)random();
//unsigned int v4 = (unsigned int)random();
//snprintf(sessionId, sizeof(sessionId), "%X%X%X%X", v1, v2, v3, v4);
//
//printf("\nMD5: %s\n", str2md5(sessionId, sizeof sessionId));
//
//
//CURL *curl;
//CURLcode res;
//
//curl_global_init(CURL_GLOBAL_DEFAULT);
//
//curl = curl_easy_init();
//if(curl) {
//struct curl_slist *chunk = NULL;
//chunk = curl_slist_append(chunk, "Authorization: Basic bHVjYS5zdGFzaW86ZGV2ZWxvcDIwMDA=");
//
//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
//curl_easy_setopt(curl, CURLOPT_URL, "https://www.itresources.it/home/luca.stasio/drafts.rss?auth=ba");
//
////#ifdef SKIP_PEER_VERIFICATION
//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//
////#ifdef SKIP_HOSTNAME_VERIFICATION
//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//
///* Perform the request, res will get the return code */
//res = curl_easy_perform(curl);
///* Check for errors */
//if(res != CURLE_OK)
//fprintf(stderr, "curl_easy_perform() failed: %s\n",
//curl_easy_strerror(res));
//
///* always cleanup */
//curl_easy_cleanup(curl);
//}
//
//curl_global_cleanup();
//
//return 0;

#endif