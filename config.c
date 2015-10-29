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

struct config global_config = {.port = DEFAULT_PORT, .ssl = false};
const char *config_file_name = "ask.ini";
unsigned int command_line_configured_params = NONE;

static void usage();

static int loadConfiguration();

static int loadConfigurationHandler(void *user, const char *section, const char *name, const char *value);

static void printConfiguration();

void configure(int argc, char *const *argv)
{
        const char *shortOptions = "hp:sC:";
        static const struct option longOptions[] = {
                {"help",        no_argument,       NULL, 'h'},
                {"port",        required_argument, NULL, 'p'},
                {"ssl",         no_argument,       NULL, 's'},
                {"config-file", required_argument, NULL, 'C'},
                {NULL, 0,                          NULL, 0}
        };
        int longIndex = 0;

        global_config.name = argv[0];

        log(TPL_INFO, "ASK server (%s) initialization", global_config.name);
        log(TPL_INFO, "loading configuration from %s", config_file_name);
//    printf("\n>> ASK server (%s) initialization\n", globalConfig.name);
//    printf(">> Loading configuration fr    om %s\n", configFileName);

        int opt = getopt_long(argc, argv, shortOptions, longOptions, &longIndex);
        while (opt != -1) {
                switch (opt) {
                        case 'h':
                                usage();
                                exit(0);
                                //break;
                        case 'p':
                                global_config.port = atoi(optarg);
                                command_line_configured_params |= PORT;
                                break;
                        case 's':
                                global_config.ssl = true;
                                command_line_configured_params |= _SSL;

                                /* if no port value as command line param, then set default ssl port value */
                                if ((command_line_configured_params & PORT) == 0) {
                                        global_config.port = DEFAULT_SSL_PORT;
                                }
                                break;
                        case 'C':
                                config_file_name = optarg;
                                break;
                        default:
                                break;
                }
                opt = getopt_long(argc, argv, shortOptions, longOptions, &longIndex);
        }

        /* load configuration from file */
        loadConfiguration();
        log(TPL_INFO, "version: %s", ASK_VERSION);
        log(TPL_INFO, "listening port: %d", global_config.port);
        log(TPL_INFO, "SSL: %s", global_config.ssl ? "enabled" : "disabled");
        log(TPL_INFO, "Http Endpoint: http%s://localhost:%d/ask/auth", global_config.ssl ? "s" : "",
            global_config.port);
        printConfiguration();
}

static void usage()
{
        printf("Usage: %s [OPTIONS]\n", global_config.name);
        printf("  -p, --port port           ASK server port number\n");
        printf("  -s, --ssl                 enable SSL\n");
        printf("  -C, --config-file         configuration file\n");
        printf("  -h, --help                print this help and exit\n");
        printf("\n");
}

static int loadConfiguration()
{
        if (ini_parse(config_file_name, loadConfigurationHandler, &global_config)) {
                perror("Unable to load configuration");
                exit(-1);
        }
}

static int loadConfigurationHandler(void *user, const char *section, const char *name, const char *value)
{
        struct Config *pconfig = (struct Config *) user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
        if (MATCH("server", "port")) {
                if ((command_line_configured_params & PORT) == 0) {
                        /* if no port value as command line param now we have the default value,
                         * so here we can get the port specified in the INI file */
                        pconfig->port = atoi(value);
                }
        } else if (MATCH("server", "ssl")) {
                if ((command_line_configured_params & _SSL) == 0) {
                        /* if no SSL value as command line param here we get INI configuration */
                        pconfig->ssl = strcmp(strdup(value), "true") == 0 ? true : false;
                }
        } else if (MATCH("server", "log_level")) {

        } else if (MATCH("http_auth_client", "url")) {
                pconfig->http_auth_url = malloc(strlen(value));
                strcpy(pconfig->http_auth_url, value);
        } else if (MATCH("http_auth_client", "ssl")) {
                pconfig->http_auth_ssl = strcmp(strdup(value), "true") == 0 ? true : false;
        } else {
                return 0;  /* unknown section/name, error */
        }
        return 1;
}

static void printConfiguration()
{
        printf("=====================================================================\n");
        printf("%s configuration\n", global_config.name);
        printf("---------------------------------------------------------------------\n");
        printf("  Version         :     %s\n", ASK_VERSION);
        printf("  Listening port  :     %d\n", global_config.port);
        printf("  SSL             :     %s\n", global_config.ssl ? "enabled" : "disabled");
        printf("  Http Endpoint   :     http%s://localhost:%d/ask/auth\n", global_config.ssl ? "s" : "",
               global_config.port);
        printf("=====================================================================\n");
}
