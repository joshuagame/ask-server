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
** config.c
** ASK Server configuration functionalities
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

#define DEFAULT_PORT 5080
#define DEFAULT_SSL_PORT 5443

struct Config globalConfig = {.port = DEFAULT_PORT, .ssl = false };
const char* configFileName = "ask.ini";
unsigned int commandLineConfiguredParams = NONE;

static void usage();
static int loadConfiguration();
static int loadConfigurationHandler(void* user, const char* section, const char* name, const char* value);
static void printConfiguration();

void configure(int argc, char *const *argv)
{
    const char *shortOptions = "hp:sC:";
    static const struct option longOptions[] = {
            {"help", no_argument, NULL, 'h'},
            {"port", required_argument, NULL, 'p'},
            {"ssl", no_argument, NULL, 's'},
            {"config-file", required_argument, NULL, 'C'},
            {NULL, 0, NULL, 0}
    };
    int longIndex = 0;

    globalConfig.name = argv[0];

    printf("\n>> ASK server (%s) initialization\n", globalConfig.name);
    printf(">> Loading configuration from %s\n", configFileName);

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
                commandLineConfiguredParams |= _SSL;

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

    /* load configuration from file */
    loadConfiguration();
    printConfiguration();
}

static void usage()
{
    printf("Usage: %s [OPTIONS]\n", globalConfig.name);
    printf("  -p, --port port           ASK server port number\n");
    printf("  -s, --ssl                 enable SSL\n");
    printf("  -C, --config-file         configuration file\n");
    printf("  -h, --help                print this help and exit\n");
    printf("\n");
}

static int loadConfiguration()
{
    if (ini_parse(configFileName, loadConfigurationHandler, &globalConfig)) {
        perror("Unable to load configuration");
        exit(-1);
    }
}

static int loadConfigurationHandler(void* user, const char* section, const char* name, const char* value)
{
    struct Config* pconfig = (struct Config*)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("server", "port")) {
        if ((commandLineConfiguredParams & PORT) == 0) {
            /* if no port value as command line param now we have the default value,
             * so here we can get the port specified in the INI file */
            pconfig->port = atoi(value);
        }
    } else if (MATCH("server", "ssl")) {
        if ((commandLineConfiguredParams & _SSL) == 0) {
            /* if no SSL value as command line param here we get INI configuration */
            pconfig->ssl = strcmp(strdup(value), "true") == 0 ? true : false;
        }
    } else if (MATCH("http_auth_client", "url")) {
//        snprintf(pconfig->http_auth_url, sizeof(pconfig->http_auth_url), "%s", value);
        pconfig->http_auth_url = value;
    } else if (MATCH("http_auth_client", "ssl")) {
        pconfig->http_auth_ssl = atoi(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

static void printConfiguration()
{
    printf("=====================================================================\n");
    printf("%s configuration\n", globalConfig.name);
    printf("---------------------------------------------------------------------\n");
    printf("  Version         :     %s\n", ASK_VERSION);
    printf("  Listening port  :     %d\n", globalConfig.port);
    printf("  SSL             :     %s\n", globalConfig.ssl ? "enabled" : "disabled");
    printf("  Http Endpoint   :     http%s://localhost:%d//ask-api/auth\n", globalConfig.ssl ? "s" : "", globalConfig.port);
    printf("=====================================================================\n");
}
