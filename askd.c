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
#include <fcntl.h>

static void startServer();

static struct MHD_Daemon* mhdd;

static void int_signal_handler(int s)
{
        asklog(TPL_INFO, "Shutting down the server");
        MHD_stop_daemon(mhdd);
        asklog(TPL_INFO, "ASK Server is down...");
        asklog(TPL_INFO, "Bye!");
        log_dispose();
        exit(0);
}

int main(int argc, char* const* argv)
{
        signal(SIGINT, int_signal_handler);

        int log_fd = open("server.log", O_RDWR | O_APPEND | O_CREAT, S_IWRITE | S_IREAD);

        log_init(TPLM_FILE, TPL_DEBUG, log_fd);
        configure(argc, argv);
        asklog(TPL_INFO, "ASK server configured!");

        startServer();

        return 0;
}

void startServer()
{
//    struct MHD_Daemon* mhdd;
        struct timeval tv;
        struct timeval* tvp;
        fd_set read_fd_set;
        fd_set write_fd_set;
        fd_set exception_fd_set;
        int max_fd;
        unsigned MHD_LONG_LONG mhdTimeout;

        /** TODO: set connection timeout in config INI */
        srandom((unsigned int) time(NULL));
        mhdd = MHD_start_daemon(MHD_USE_DEBUG, global_config.port, NULL, NULL, &request_handler, NULL,
                                MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 15,
                                MHD_OPTION_NOTIFY_COMPLETED, &request_completed_callback, NULL,
                                MHD_OPTION_END);

        asklog(TPL_INFO, "ASK server started");
        asklog(TPL_INFO, "server is listening on port %d", global_config.port);
        if (mhdd == NULL) {
                perror("Unable to start server: error initializing internal MHD server Daemon\n");
                exit(-1);
        }

        bool running = true;
        while (running) {
                //handleExpiredSessions();
                max_fd = 0;
                FD_ZERO(&read_fd_set);
                FD_ZERO(&write_fd_set);
                FD_ZERO(&exception_fd_set);

                if (MHD_get_fdset(mhdd, &read_fd_set, &write_fd_set, &exception_fd_set, &max_fd) != MHD_YES) {
                        perror("Unable to start server: fatal internal error occurred\n");
                        exit(-1);
                }

                tvp = NULL;
                if (MHD_get_timeout(mhdd, &mhdTimeout) == MHD_YES) {
                        tv.tv_sec = mhdTimeout / 1000;
                        tv.tv_usec = (mhdTimeout - (tv.tv_sec * 1000)) * 1000;
                        tvp = &tv;
                }

                select(max_fd + 1, &read_fd_set, &write_fd_set, &exception_fd_set, tvp);

                /* finally, fire up the server and make it running! */
                MHD_run(mhdd);
        }

//    MHD_stop_daemon(mhdd);
}
