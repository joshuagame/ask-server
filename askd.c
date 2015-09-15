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

#define _GNU_SOURCE         /*!< this is for asprintf() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <libconfig.h>


#define REALM "\"ask\""
#define TEST_USER "test"
#define TEST_PASSWORD "password"
#define DEFAULT_PORT 5080
#define DEFAULT_SSL_PORT 5443

enum ASK_COMMAND_LINE_OPTIONS {
    NONE = 0,
    PORT = 1,
    SSL = 2,
    CONFIG_FILE = 3
};

unsigned int globalCommandLineSet;

struct askConfig {
    int port;
    bool ssl;
    char *configFileName;
} globalConfig = {
        .port = DEFAULT_PORT,
        .ssl = false,
        .configFileName = "/home/joshuagame/Sviluppo/ClionProjects/ask-server/dist/ask.conf"
};

void usage(char *appName)
{
    printf("Usage: %s [OPTIONS]\n", appName);
    printf("  -p, --port port           ASK server port number\n");
    printf("  -s, --ssl                 enable SSL\n");
    printf("  -f, --config-file         configuration file\n");
    printf("  -h, --help                print this help and exit\n");
    printf("\n");
}

int loadConfiguration()
{
    config_t cfg;
    config_setting_t *setting;
    const char *str;

    config_init(&cfg);
    if (!config_read_file(&cfg, globalConfig.configFileName)) {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return -1;
    }

    setting = config_lookup(&cfg, "server");
    unsigned int count = config_setting_length(setting);
    printf("server config len: %u\n", count);
    if (setting != NULL) {
        config_setting_t *server = config_setting_get_elem(setting, 0);
        int port = config_setting_get_int_elem(setting, 0);
        //if(!(config_setting_lookup_int(server, "port", &port))) {
        //    printf("No value for port in configuration file\n");
        //}
        //config_setting_lookup_int(server, "port", &port);
        config_setting_get
        printf("server.port = %d\n", port);
    }
}

int main(int argc, char *const *argv) {
    int c;
    const char *shortOptions = "hp:sf:";
    static const struct option longOptions[] = {
            {"help", no_argument, NULL, 'h'},
            {"port", required_argument, NULL, 'p'},
            {"ssl", no_argument, NULL, 's'},
            {"config-file", required_argument, NULL, 'f'},
            {NULL, 0, NULL, 0}
    };
    int longIndex = 0;



    int opt = getopt_long( argc, argv, shortOptions, longOptions, &longIndex );
    while (opt != -1) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
                break;
            case 'p':
                globalCommandLineSet |= PORT;
                globalConfig.port = atoi(optarg);
                break;
            case 's':
                globalCommandLineSet |= SSL;
                globalConfig.ssl = true;
                if ((globalCommandLineSet & PORT) == 0) {
                    globalConfig.port = DEFAULT_SSL_PORT;
                }
                break;
            case 'f':
                globalCommandLineSet |= CONFIG_FILE;
                globalConfig.configFileName = optarg;
                break;
            default:
                break;
        }
        opt = getopt_long( argc, argv, shortOptions, longOptions, &longIndex );
    }

    printf("ASK server initialization\n");
    printf("Getting configuration from %s\n", globalConfig.configFileName);

    /* load configuration from file */
    loadConfiguration();

    printf("ASK server starting on port %d (ssl:%s)\n", globalConfig.port, globalConfig.ssl ? "enabled" : "disabled");

    return 0;
}
//
//case 'm':
//{
//char* endp = NULL;
//long l = -1;
//if (!optarg ||  ((l=strtol(optarg, 0, &endp)),(endp && *endp)))
//{ fprintf(stderr, "invalid m option %s - expecting a number\n",
//optarg?optarg:"");
//exit(EXIT_FAILURE);
//};
//// you could add more checks on l here...
//n = (int) l;
//break;
//}
//n = optarg;
//break;