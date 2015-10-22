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
** askd.c
** main entry point for the ASK Server
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

/**/
#define _GNU_SOURCE         /*!< this is for asprintf() */

#include "ask.h"
#include <curl/curl.h>

static void startServer();

int main(int argc, char *const *argv) {
    configure(argc, argv);
    /* start the server */
    startServer();
    return 0;
}

void startServer()
{
    struct MHD_Daemon* mhdd;
    struct timeval tv;
    struct timeval* tvp;
    fd_set readFdSet;
    fd_set writeFdSet;
    fd_set exceptionFdSet;
    int maxFd;
    unsigned MHD_LONG_LONG mhdTimeout;

    /** TODO: set connection timeout in config INI */
    srandom((unsigned int)time(NULL));
    mhdd = MHD_start_daemon(MHD_USE_DEBUG, globalConfig.port, NULL, NULL, &requestHandler, NULL,
                            MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int)15,
                            MHD_OPTION_NOTIFY_COMPLETED, &requestCompletedCallback, NULL,
                            MHD_OPTION_END);

    if (mhdd == NULL) {
        perror("Unable to start server: error initializing internal MHD server Daemon\n");
        exit(-1);
    }

    bool running = true;
    while(running) {
        //handleExpiredSessions();
        maxFd = 0;
        FD_ZERO(&readFdSet);
        FD_ZERO(&writeFdSet);
        FD_ZERO(&exceptionFdSet);

        if (MHD_get_fdset(mhdd, &readFdSet, &writeFdSet, &exceptionFdSet, &maxFd) != MHD_YES) {
            perror("Unable to start server: fatal internal error occurred\n");
            exit(-1);
        }

        tvp = NULL;
        if (MHD_get_timeout(mhdd, &mhdTimeout) == MHD_YES) {
            tv.tv_sec = mhdTimeout / 1000;
            tv.tv_usec = (mhdTimeout - (tv.tv_sec * 1000)) * 1000;
            tvp = &tv;
        }

        select(maxFd + 1, &readFdSet, &writeFdSet, &exceptionFdSet, tvp);

        /* finally, fire up the server and make it running! */
        MHD_run(mhdd);
    }

    MHD_stop_daemon(mhdd);
}
