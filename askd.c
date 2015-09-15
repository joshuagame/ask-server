/*
**     _    ____  _  __
**    / \  / ___|| |/ /  ASK
**   / _ \ \___ \| ' /   Authentication Sessions Keeper Server
**  / ___ \ ___) | . \   -------------------------------------
** /_/   \_\____/|_|\_\  www.codegazoline.it/ask
**                 v0.1
** ===========================================================================
**
** Copyright (C) 2015, The CodeGazoline Team - gargantua@codegazoline.it
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
** along with Nome - Programma.If not, see <http:**www.gnu.org/licenses/>.
**
** ===========================================================================
*/

/**/
#define _GNU_SOURCE         /*!< this is for asprintf() */

#include "version.h"
#include "ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>



#define REALM "\"ask\""
#define TEST_USER "test"
#define TEST_PASSWORD "password"
#define DEFAULT_PORT 5080
#define DEFAULT_SSL_PORT 5443

static char* appName;

/** configuration --------------------------------------------------------------------------------------------------- */

struct askConfig {
    int port;
    bool ssl;
} globalConfig = {
        .port = DEFAULT_PORT,
        .ssl = false
};

const char* configFileName = "ask.ini";

enum CL_CONF {
    NONE = 0,
    PORT = 1,
    SSL = 2
};

unsigned int commandLineConfiguredParams = NONE;

/** request and session --------------------------------------------------------------------------------------------- */

struct formCredentials {
    char username[64];
    char password[64];
};

typedef struct askSession {
    struct askSession* next;
    char id[33];
    unsigned int rc;
    time_t start;
    struct formCredentials fcred;
} askSession;

typedef struct {
    askSession* session;
    struct MHD_PostProcessor* postProcessor;
    const char* postUrl;
} askRequest;


/** forwards -------------------------------------------------------------------------------------------------------- */
void usage();
static int loadConfigurationHandler(void* user, const char* section, const char* name, const char* value);
int loadConfiguration();
void printConfiguration();
void startServer();
static int requestHandler(void* cls, struct MHD_Connection* connection, const char* url, const char* method,
                          const char* version, const char* uploadData, size_t* uploadDataSize, void** ptr);
static void requestCompletedCallback(void* cls, struct MHD_Connection* connection,
                                     void** conCls, enum MHD_RequestTerminationCode toe);

/**
 * Main
 */
int main(int argc, char *const *argv) {
    int c;
    const char *shortOptions = "hp:sC:";
    static const struct option longOptions[] = {
            {"help", no_argument, NULL, 'h'},
            {"port", required_argument, NULL, 'p'},
            {"ssl", no_argument, NULL, 's'},
            {"config-file", required_argument, NULL, 'C'},
            {NULL, 0, NULL, 0}
    };
    int longIndex = 0;

    appName = argv[0];

    int opt = getopt_long( argc, argv, shortOptions, longOptions, &longIndex );
    while (opt != -1) {
        switch (opt) {
            case 'h':
                usage();
                exit(0);
                //break;
            case 'p':
                globalConfig.port = atoi(optarg);
                commandLineConfiguredParams |= PORT;
                break;
            case 's':
                globalConfig.ssl = true;
                commandLineConfiguredParams |= SSL;

                /* if no port value as command line param, then set default ssl port value */
                if ((commandLineConfiguredParams & PORT) == 0) {
                    globalConfig.port = DEFAULT_SSL_PORT;
                }
                break;
            case 'C':
                configFileName = optarg;
                break;
            default:
                break;
        }
        opt = getopt_long( argc, argv, shortOptions, longOptions, &longIndex );
    }

    printf("\n>> ASK server (%s) initialization\n", appName);
    printf(">> Loading configuration from %s\n", configFileName);

    /* load configuration from file */
    loadConfiguration();
    printConfiguration();

    /* start the server */
    startServer();
    printf(">> ASK server up & running...\n\n");

    return 0;
}


void usage()
{
    printf("Usage: %s [OPTIONS]\n", appName);
    printf("  -p, --port port           ASK server port number\n");
    printf("  -s, --ssl                 enable SSL\n");
    printf("  -C, --config-file         configuration file\n");
    printf("  -h, --help                print this help and exit\n");
    printf("\n");
}


static int loadConfigurationHandler(void* user, const char* section, const char* name,
                   const char* value)
{
    struct askConfig* pconfig = (struct askConfig*)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("server", "port")) {
        if ((commandLineConfiguredParams & PORT) == 0) {
            /* if no port value as command line param now we have the default value,
             * so here we can get the port specified in the INI file */
            pconfig->port = atoi(value);
        }
    } else if (MATCH("server", "ssl")) {
        if ((commandLineConfiguredParams & SSL) == 0) {
            /* if no SSL value as command line param here we get INI configuration */
            pconfig->ssl = strcmp(strdup(value), "true") == 0 ? true : false;
        }
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}


int loadConfiguration()
{
    if (ini_parse(configFileName, loadConfigurationHandler, &globalConfig)) {
        perror("Unable to load configuration");
        exit(-1);
    }
}

void printConfiguration()
{
    printf("=====================================================================\n");
    printf("%s configuration\n", appName);
    printf("---------------------------------------------------------------------\n");
    printf("  Version         :     %s\n", ASK_VERSION);
    printf("  Listening port  :     %d\n", globalConfig.port);
    printf("  SSL             :     %s\n", globalConfig.ssl ? "enabled" : "disabled");
    printf("  Http Endpoint   :     http%s://localhost:%d//ask-api/auth\n", globalConfig.ssl ? "s" : "", globalConfig.port);
    printf("=====================================================================\n");
}

void startServer()
{
    struct MHD_Daemon* server;
    struct timeval tv;
    struct timeval* tvp;
    fd_set readFdSet;
    fd_set writeFdSet;
    fd_set exceptionFdSet;
    int maxFd;
    unsigned MHD_LONG_LONG mhdTimeout;

    /** TODO: set connection timeout in config INI */
    srandom((unsigned int)time(NULL));
    server = MHD_start_daemon(MHD_USE_DEBUG, globalConfig.port, NULL, NULL, &requestHandler, NULL,
                              MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int)15,
                              MHD_OPTION_NOTIFY_COMPLETED, &requestCompletedCallback, NULL,
                              MHD_OPTION_END);

    if (server == NULL) {
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

        if (MHD_get_fdset(server, &readFdSet, &writeFdSet, &exceptionFdSet, &maxFd) != MHD_YES) {
            perror("Unable to start server: fatal internal error occurred\n");
            exit(-1);
        }

        tvp = NULL;
        if (MHD_get_timeout(server, &mhdTimeout) == MHD_YES) {
            tv.tv_sec = mhdTimeout / 1000;
            tv.tv_usec = (mhdTimeout - (tv.tv_sec * 1000)) * 1000;
            tvp = &tv;
        }

        select(maxFd + 1, &readFdSet, &writeFdSet, &exceptionFdSet, tvp);

        /* finally, fire up the server and make it running! */
        MHD_run(server);
    }

    MHD_stop_daemon(server);
}

static int requestHandler(void* cls, struct MHD_Connection* connection, const char* url, const char* method,
                          const char* version, const char* uploadData, size_t* uploadDataSize, void** ptr)
{
    return 0;
}

static void requestCompletedCallback(void* cls, struct MHD_Connection* connection,
                                     void** conCls, enum MHD_RequestTerminationCode toe)
{

}




