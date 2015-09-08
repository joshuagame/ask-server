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

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define REALM     "\"ask\""
#define USER      "test"
#define PASSWORD  "password"


/*
#define PORT 8443
#define SERVERKEYFILE "certs/server.key"
#define SERVERCERTFILE "certs/server.crt"





static long
get_file_size(const char *filename) {
    FILE *fp;

    fp = fopen(filename, "rb");
    if (fp) {
        long size;

        if ((0 != fseek(fp, 0, SEEK_END)) || (-1 == (size = ftell(fp))))
            size = 0;

        fclose(fp);

        return size;
    }
    else
        return 0;
}

static char *
load_file(const char *filename) {
    FILE *fp;
    char *buffer;
    long size;

    size = get_file_size(filename);
    if (size == 0)
        return NULL;

    fp = fopen(filename, "rb");
    if (!fp)
        return NULL;

    buffer = malloc(size);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }

    if (size != fread(buffer, 1, size, fp)) {
        free(buffer);
        buffer = NULL;
    }

    fclose(fp);
    return buffer;
}




static int
secret_page(struct MHD_Connection *connection) {
    int ret;
    struct MHD_Response *response;
    const char *page = "<html><body>A secret.</body></html>";

    response =
            MHD_create_response_from_buffer(strlen(page), (void *) page,
                                            MHD_RESPMEM_PERSISTENT);
    if (!response)
        return MHD_NO;

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}


static int
answer_to_connection(void *cls, struct MHD_Connection *connection,
                     const char *url, const char *method,
                     const char *version, const char *upload_data,
                     size_t *upload_data_size, void **con_cls) {
    if (0 != strcmp(method, "GET"))
        return MHD_NO;
    if (NULL == *con_cls) {
        *con_cls = connection;
        return MHD_YES;
    }

    if (!is_authenticated(connection, USER, PASSWORD))
        return ask_for_authentication(connection, REALM);

    return secret_page(connection);
}


int
main() {
    struct MHD_Daemon *daemon;
    char *key_pem;
    char *cert_pem;

    key_pem = load_file(SERVERKEYFILE);
    cert_pem = load_file(SERVERCERTFILE);

    if ((key_pem == NULL) || (cert_pem == NULL)) {
       //printf("The key/certificate files could not be read.\n");
        return 1;
    }

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_SSL, PORT, NULL,
                             NULL, &answer_to_connection, NULL,
                             MHD_OPTION_HTTPS_MEM_KEY, key_pem,
                             MHD_OPTION_HTTPS_MEM_CERT, cert_pem, MHD_OPTION_END);
    if (NULL == daemon) {
       //printf("%s\n", cert_pem);

        free(key_pem);
        free(cert_pem);

        return 1;
    }

    getchar ();

    MHD_stop_daemon(daemon);
    free(key_pem);
    free(cert_pem);

    return 0;
}

 */

/* needed for asprintf */
#define _GNU_SOURCE


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <microhttpd.h>

/**
 * Invalid method page.
 */
#define METHOD_ERROR "<html><head><title>Illegal request</title></head><body>Go away.</body></html>"

/**
 * Invalid URL page.
 */
#define NOT_FOUND_ERROR "<html><head><title>Not found</title></head><body>Go away.</body></html>"



#define LOGIN_PAGE "<html><head><title>Welcome</title></head><body><form action=\"/auth_form\" method=\"post\">Username: <input type=\"text\" name=\"j_username\"/><br>Password: <input type=\"password\" name=\"j_password\"/><br><input type=\"submit\" value=\"Next\" /></body></html>"

/**
 * Front page. (/)
 */
#define MAIN_PAGE "<html><head><title>Welcome</title></head><body><form action=\"/2\" method=\"post\">Whatis your name? <input type=\"text\" name=\"v1\" value=\"%s\" /><input type=\"submit\" value=\"Next\" /></body></html>"

/**
 * Second page. (/2)
 */
#define SECOND_PAGE "<html><head><title>Tell me more</title></head><body><a href=\"/\">previous</a> <form action=\"/S\" method=\"post\">%s, what is your job? <input type=\"text\" name=\"v2\" value=\"%s\" /><input type=\"submit\" value=\"Next\" /></body></html>"

/**
 * Second page (/S)
 */
#define SUBMIT_PAGE "<html><head><title>Ready to submit?</title></head><body><form action=\"/F\" method=\"post\"><a href=\"/2\">previous </a> <input type=\"hidden\" name=\"DONE\" value=\"yes\" /><input type=\"submit\" value=\"Submit\" /></body></html>"

/**
 * Last page.
 */
#define LAST_PAGE "<html><head><title>Thank you</title></head><body>Thank you.</body></html>"

/**
 * Name of our cookie.
 */
#define COOKIE_NAME "ASK_SESSION"


/*!*
 * State we keep for each user/session/browser.
 */
struct Session {
    /**
     * We keep all sessions in a linked list.
     */
    struct Session *next;

    /**
     * Unique ID for this session.
     */
    char sid[33];

    /**
     * Reference counter giving the number of connections
     * currently using this session.
     */
    unsigned int rc;

    /**
     * Time when this session was last active.
     */
    time_t start;

    // ADD an hashmap for params

    char j_username[64];
    char j_password[64];
};


/**
 * Data kept per request.
 */
struct Request {

    /**
     * Associated session.
     */
    struct Session *session;

    /**
     * Post processor handling form data (IF this is
     * a POST request).
     */
    struct MHD_PostProcessor *pp;

    /**
     * URL to serve in response to this POST (if this request
     * was a 'POST')
     */
    const char *post_url;



};


/**
 * Linked list of all active sessions.  Yes, O(n) but a
 * hash table would be overkill for a simple example...
 */
static struct Session *sessions;


static char *
string_to_base64(const char *message) {
    const char *lookup =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned long l;
    int i;
    char *tmp;
    size_t length = strlen(message);

    tmp = malloc(length * 2);
    if (NULL == tmp)
        return tmp;

    tmp[0] = 0;

    for (i = 0; i < length; i += 3) {
        l = (((unsigned long) message[i]) << 16)
            | (((i + 1) < length) ? (((unsigned long) message[i + 1]) << 8) : 0)
            | (((i + 2) < length) ? ((unsigned long) message[i + 2]) : 0);


        strncat(tmp, &lookup[(l >> 18) & 0x3F], 1);
        strncat(tmp, &lookup[(l >> 12) & 0x3F], 1);

        if (i + 1 < length)
            strncat(tmp, &lookup[(l >> 6) & 0x3F], 1);
        if (i + 2 < length)
            strncat(tmp, &lookup[l & 0x3F], 1);
    }

    if (length % 3)
        strncat(tmp, "===", 3 - length % 3);

    return tmp;
}

static int
ask_for_authentication(struct MHD_Connection *connection, const char *realm) {
    int ret;
    struct MHD_Response *response;
    char *headervalue;
    const char *strbase = "Basic realm=";

    response = MHD_create_response_from_buffer(0, NULL,
                                               MHD_RESPMEM_PERSISTENT);
    if (!response)
        return MHD_NO;

    headervalue = malloc(strlen(strbase) + strlen(realm) + 1);
    if (!headervalue)
        return MHD_NO;

    strcpy(headervalue, strbase);
    strcat(headervalue, realm);

    ret = MHD_add_response_header(response, "WWW-Authenticate", headervalue);
    free(headervalue);
    if (!ret) {
        MHD_destroy_response(response);
        return MHD_NO;
    }

    ret = MHD_queue_response(connection, MHD_HTTP_UNAUTHORIZED, response);

    MHD_destroy_response(response);

    return ret;
}

/**
 * Return the session handle for this connection, or
 * create one if this is a new user.
 */
static struct Session *
get_session(struct MHD_Connection *connection) {
    struct Session *ret;
    const char *cookie;

    cookie = MHD_lookup_connection_value(connection,
                                         MHD_COOKIE_KIND,
                                         COOKIE_NAME);
    if (cookie != NULL) {
        /* find existing session */
        ret = sessions;
        while (NULL != ret) {
            if (0 == strcmp(cookie, ret->sid))
                break;
            ret = ret->next;
        }
        if (NULL != ret) {
            ret->rc++;
            return ret;
        }
    }
    /* create fresh session */
    ret = calloc(1, sizeof(struct Session));
    if (NULL == ret) {
        fprintf(stderr, "calloc error: %s\n", strerror(errno));
        return NULL;
    }
    /* not a super-secure way to generate a random session ID,
       but should do for a simple example... */
    snprintf(ret->sid,
             sizeof(ret->sid),
             "%X%X%X%X",
             (unsigned int) random(),
             (unsigned int) random(),
             (unsigned int) random(),
             (unsigned int) random());
    ret->rc++;
    ret->start = time(NULL);
    ret->next = sessions;
    sessions = ret;
    return ret;
}


static int
is_authenticated(struct MHD_Connection *connection,
                 const char *username, const char *password) {
    const char *headervalue;
    char *expected_b64, *expected;
    const char *strbase = "Basic ";
    int authenticated;
    struct Session* session;

    /* check if there is a session for this request */
    //session = get_session(connection);
    const char* cookie = MHD_lookup_connection_value(connection,
                                         MHD_COOKIE_KIND,
                                         COOKIE_NAME);
    if (NULL == cookie) {
       fprintf(stdout, "No session for request. Try and check credentials.\n");
        //return MHD_NO; /* internal error */

        headervalue =
                MHD_lookup_connection_value(connection, MHD_HEADER_KIND,
                                            "Authorization");
       fprintf(stdout, "HEADER Authorization: %s\n", headervalue == NULL ? "null" : headervalue);
        if (NULL == headervalue)
            return 0;
        if (0 != strncmp(headervalue, strbase, strlen(strbase)))
            return 0;

        expected = malloc(strlen(username) + 1 + strlen(password) + 1);
        if (NULL == expected)
            return 0;

        strcpy(expected, username);
        strcat(expected, ":");
        strcat(expected, password);

        expected_b64 = string_to_base64(expected);
        free(expected);
        if (NULL == expected_b64)
            return 0;

        authenticated = (strcmp(headervalue + strlen(strbase), expected_b64) == 0);

        free(expected_b64);
    } else {
       //printf("%s authenticated.\n", cookie);
        authenticated = (1 == 1);
    }



    return authenticated;
}




/**
 * Type of handler that generates a reply.
 *
 * @param cls content for the page (handler-specific)
 * @param mime mime type to use
 * @param session session information
 * @param connection connection to process
 * @param MHD_YES on success, MHD_NO on failure
 */
typedef int (*PageHandler)(const void *cls,
                           const char *mime,
                           struct Session *session,
                           struct MHD_Connection *connection);


/**
 * Entry we generate for each page served.
 */
struct Page {
    /**
     * Acceptable URL for this page.
     */
    const char *url;

    /**
     * Mime type to set for the page.
     */
    const char *mime;

    /**
     * Handler to call to generate response.
     */
    PageHandler handler;

    /**
     * Extra argument to handler.
     */
    const void *handler_cls;
};


/**
 * Add header to response to set a session cookie.
 *
 * @param session session to use
 * @param response response to modify
 */
static void
add_session_cookie(struct Session *session,
                   struct MHD_Response *response) {
    char cstr[256];
    snprintf(cstr,
             sizeof(cstr),
             "%s=%s",
             COOKIE_NAME,
             session->sid);
    if (MHD_NO ==
        MHD_add_response_header(response,
                                MHD_HTTP_HEADER_SET_COOKIE,
                                cstr)) {
        fprintf(stderr,
                "Failed to set session cookie header!\n");
    }
}


/**
 * Handler that returns a simple static HTTP page that
 * is passed in via 'cls'.
 *
 * @param cls a 'const char *' with the HTML webpage to return
 * @param mime mime type to use
 * @param session session handle
 * @param connection connection to use
 */
static int
serve_simple_form(const void *cls,
                  const char *mime,
                  struct Session *session,
                  struct MHD_Connection *connection) {
    int ret;
    const char *form = cls;
    struct MHD_Response *response;

    /* return static form */
    response = MHD_create_response_from_buffer(strlen(form),
                                               (void *) form,
                                               MHD_RESPMEM_PERSISTENT);
    add_session_cookie(session, response);
    MHD_add_response_header(response,
                            MHD_HTTP_HEADER_CONTENT_ENCODING,
                            mime);
    ret = MHD_queue_response(connection,
                             MHD_HTTP_OK,
                             response);
    MHD_destroy_response(response);
    return ret;
}


/**
 * Handler that adds the 'v1' value to the given HTML code.
 *
 * @param cls a 'const char *' with the HTML webpage to return
 * @param mime mime type to use
 * @param session session handle
 * @param connection connection to use
 */
//static int
//fill_v1_form(const void *cls,
//             const char *mime,
//             struct Session *session,
//             struct MHD_Connection *connection) {
//    int ret;
//    const char *form = cls;
//    char *reply;
//    struct MHD_Response *response;
//
//    if (-1 == asprintf(&reply,
//                       form,
//                       session->value_1)) {
//        /* oops */
//        return MHD_NO;
//    }
//    /* return static form */
//    response = MHD_create_response_from_buffer(strlen(reply),
//                                               (void *) reply,
//                                               MHD_RESPMEM_MUST_FREE);
//    add_session_cookie(session, response);
//    MHD_add_response_header(response,
//                            MHD_HTTP_HEADER_CONTENT_ENCODING,
//                            mime);
//    ret = MHD_queue_response(connection,
//                             MHD_HTTP_OK,
//                             response);
//    MHD_destroy_response(response);
//    return ret;
//}

static int
login(const void *cls,
             const char *mime,
             struct Session *session,
             struct MHD_Connection *connection) {
    int ret;
    const char *form = cls;
    char *reply;
    struct MHD_Response *response;

    if (-1 == asprintf(&reply, form)) {
        /* oops */
        return MHD_NO;
    }
    /* return static form */
    response = MHD_create_response_from_buffer(strlen(reply),
                                               (void *) reply,
                                               MHD_RESPMEM_MUST_FREE);
    add_session_cookie(session, response);
    MHD_add_response_header(response,
                            MHD_HTTP_HEADER_CONTENT_ENCODING,
                            mime);
    ret = MHD_queue_response(connection,
                             MHD_HTTP_OK,
                             response);
    MHD_destroy_response(response);
    return ret;
}

static int authenticate(const void *cls, const char* mime, struct Session* session, struct MHD_Connection* connection)
{
    int ret;
    char *reply;
    struct MHD_Response* response;

    // authenticate user
    if (is_authenticated(connection, USER, PASSWORD)) {
        // prepare reply OK or KO... here is OK
        if (-1 == asprintf(&reply, "{\"result\":\"OK\", \"description\":\"authenticated\"}")) {
            return MHD_NO;
        }
    } else {
//        if (-1 == asprintf(&reply, "{\"result\":\"KO\", \"description\":\"not authenticated\"}")) {
//            return MHD_NO;
//        }

       //printf("USER NOT AUTHENTICATED. Try and ask for authentication with Realm $s\n", REALM);
        return ask_for_authentication(connection, REALM);
    }



    // return the response
    response = MHD_create_response_from_buffer(strlen(reply), (void*)reply, MHD_RESPMEM_MUST_FREE);

    // ass session cookie
    add_session_cookie(session, response);

    MHD_add_response_header(response,
                            MHD_HTTP_HEADER_CONTENT_ENCODING,
                            mime);
    ret = MHD_queue_response(connection,
                             MHD_HTTP_OK,
                             response);
    MHD_destroy_response(response);
    return ret;
}

static int auth_form(const void *cls, const char* mime, struct Session* session, struct MHD_Connection* connection)
{
    fprintf(stdout, "AUTH_FORM\n");

    fprintf(stdout, "j_username: %s\n", session->j_username);
    fprintf(stdout, "j_password: %s\n", session->j_password);

    return MHD_YES;
}

/**
 * Handler that adds the 'v1' and 'v2' values to the given HTML code.
 *
 * @param cls a 'const char *' with the HTML webpage to return
 * @param mime mime type to use
 * @param session session handle
 * @param connection connection to use
 */
/*
static int
fill_v1_v2_form(const void *cls,
                const char *mime,
                struct Session *session,
                struct MHD_Connection *connection) {
    int ret;
    const char *form = cls;
    char *reply;
    struct MHD_Response *response;

    if (-1 == asprintf(&reply,
                       form,
                       session->value_1,
                       session->value_2)) {

        return MHD_NO;
    }
    response = MHD_create_response_from_buffer(strlen(reply),
                                               (void *) reply,
                                               MHD_RESPMEM_MUST_FREE);
    add_session_cookie(session, response);
    MHD_add_response_header(response,
                            MHD_HTTP_HEADER_CONTENT_ENCODING,
                            mime);
    ret = MHD_queue_response(connection,
                             MHD_HTTP_OK,
                             response);
    MHD_destroy_response(response);
    return ret;
}

*/
/**
 * Handler used to generate a 404 reply.
 *
 * @param cls a 'const char *' with the HTML webpage to return
 * @param mime mime type to use
 * @param session session handle
 * @param connection connection to use
 */
static int
not_found_page(const void *cls,
               const char *mime,
               struct Session *session,
               struct MHD_Connection *connection) {
    int ret;
    struct MHD_Response *response;

    /* unsupported HTTP method */
    response = MHD_create_response_from_buffer(strlen(NOT_FOUND_ERROR),
                                               (void *) NOT_FOUND_ERROR,
                                               MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection,
                             MHD_HTTP_NOT_FOUND,
                             response);
    MHD_add_response_header(response,
                            MHD_HTTP_HEADER_CONTENT_ENCODING,
                            mime);
    MHD_destroy_response(response);
    return ret;
}


/**
 * List of all pages served by this HTTP server.
 */
static struct Page pages[] =
        {
                /*{"/",  "text/html", &fill_v1_form,      MAIN_PAGE},
                {"/2", "text/html", &fill_v1_v2_form,   SECOND_PAGE},
                {"/S", "text/html", &serve_simple_form, SUBMIT_PAGE},
                {"/F", "text/html", &serve_simple_form, LAST_PAGE},*/
                {"/auth_basic", "application/json; charset=utf-8", &authenticate, NULL},
                {"/login", "text/html", &login, LOGIN_PAGE},
                {"/auth_form", "text/html", &auth_form, NULL},
                {NULL, NULL,        &not_found_page, NULL} /* 404 */
        };


/**
 * Iterator over key-value pairs where the value
 * maybe made available in increments and/or may
 * not be zero-terminated.  Used for processing
 * POST data.
 *
 * @param cls user-specified closure
 * @param kind type of the value
 * @param key 0-terminated key for the value
 * @param filename name of the uploaded file, NULL if not known
 * @param content_type mime-type of the data, NULL if not known
 * @param transfer_encoding encoding of the data, NULL if not known
 * @param data pointer to size bytes of data at the
 *              specified offset
 * @param off offset of data in the overall value
 * @param size number of bytes in data available
 * @return MHD_YES to continue iterating,
 *         MHD_NO to abort the iteration
 */


static int
post_iterator(void *cls,
              enum MHD_ValueKind kind,
              const char *key,
              const char *filename,
              const char *content_type,
              const char *transfer_encoding,
              const char *data, uint64_t off, size_t size)
{
    struct Request *request = cls;
    struct Session *session = request->session;

    if (0 == strcmp("j_username", key)) {
        if (size + off > sizeof(session->j_username)) {
            size = sizeof(session->j_username) - off;
        }
        memcpy(&session->j_username[off], data, size);
        if (size + off < sizeof(session->j_username)) {
            session->j_username[size + off] = '\0';
        }
        fprintf(stderr, "j_username: `%s'\n", session->j_username);
        return MHD_YES;
    }

    if (0 == strcmp("j_password", key)) {
        if (size + off > sizeof(session->j_password)) {
            size = sizeof(session->j_password) - off;
        }
        memcpy(&session->j_password[off], data, size);
        if (size + off < sizeof(session->j_password)) {
            session->j_password[size + off] = '\0';
        }
        fprintf(stderr, "j_password: `%s'\n", session->j_password);
        return MHD_YES;
    }

    fprintf(stderr, "Unsupported form value `%s'\n", key);
    return MHD_YES;
}


/**
 * Main MHD callback for handling requests.
 *
 *
 * @param cls argument given together with the function
 *        pointer when the handler was registered with MHD
 * @param url the requested url
 * @param method the HTTP method used ("GET", "PUT", etc.)
 * @param version the HTTP version string (i.e. "HTTP/1.1")
 * @param upload_data the data being uploaded (excluding HEADERS,
 *        for a POST that fits into memory and that is encoded
 *        with a supported encoding, the POST data will NOT be
 *        given in upload_data and is instead available as
 *        part of MHD_get_connection_values; very large POST
 *        data *will* be made available incrementally in
 *        upload_data)
 * @param upload_data_size set initially to the size of the
 *        upload_data provided; the method must update this
 *        value to the number of bytes NOT processed;
 * @param con_cls pointer that the callback can set to some
 *        address and that will be preserved by MHD for future
 *        calls for this request; since the access handler may
 *        be called many times (i.e., for a PUT/POST operation
 *        with plenty of upload data) this allows the application
 *        to easily associate some request-specific state.
 *        If necessary, this state can be cleaned up in the
 *        global "MHD_RequestCompleted" callback (which
 *        can be set with the MHD_OPTION_NOTIFY_COMPLETED).
 *        Initially, <tt>*con_cls</tt> will be NULL.
 * @return MHS_YES if the connection was handled successfully,
 *         MHS_NO if the socket must be closed due to a serios
 *         error while handling the request
 */
static int
create_response(void *cls,
                struct MHD_Connection *connection,
                const char *url,
                const char *method,
                const char *version,
                const char *upload_data,
                size_t *upload_data_size,
                void **ptr) {
    struct MHD_Response *response;
    struct Request *request;
    struct Session *session;
    int ret;
    unsigned int i;

    request = *ptr;
    if (NULL == request) {
        request = calloc(1, sizeof(struct Request));
        if (NULL == request) {
            fprintf(stderr, "calloc error: %s\n", strerror(errno));
            return MHD_NO;
        }
        *ptr = request;

        if (0 == strcmp(method, MHD_HTTP_METHOD_POST)) {
            request->pp = MHD_create_post_processor(connection, 1024,
                                                    &post_iterator, request);
            if (NULL == request->pp) {
                fprintf(stderr, "Failed to setup post processor for `%s'\n",
                        url);
                return MHD_NO; /* internal error */
            }
        }
        return MHD_YES;
    }
    if (NULL == request->session) {
        request->session = get_session(connection);
        if (NULL == request->session) {
            fprintf(stderr, "Failed to setup session for `%s'\n",
                    url);
            return MHD_NO; /* internal error */
        }
    }
    session = request->session;
    session->start = time(NULL);
    if (0 == strcmp(method, MHD_HTTP_METHOD_POST)) {
        fprintf(stdout, "POST method\n");
        /* evaluate POST data */
        MHD_post_process(request->pp,
                         upload_data,
                         *upload_data_size);
        fprintf(stdout, "POST data processed\n");
        if (0 != *upload_data_size) {
            fprintf(stdout, "upload_data_size = 0\n");
            *upload_data_size = 0;
            return MHD_YES;
        }
        /* done with POST data, serve response */
        fprintf(stdout, "serving response\n");
        MHD_destroy_post_processor(request->pp);
        request->pp = NULL;
        method = MHD_HTTP_METHOD_GET; /* fake 'GET' */
        if (NULL != request->post_url)
            url = request->post_url;
    }

    if ((0 == strcmp(method, MHD_HTTP_METHOD_GET)) ||
        (0 == strcmp(method, MHD_HTTP_METHOD_HEAD))) {
        /* find out which page to serve */
        i = 0;
        while ((pages[i].url != NULL) &&
               (0 != strcmp(pages[i].url, url)))
            i++;
        ret = pages[i].handler(pages[i].handler_cls,
                               pages[i].mime,
                               session, connection);
        if (ret != MHD_YES)
            fprintf(stderr, "Failed to create page for `%s'\n",
                    url);
        return ret;
    }
    /* unsupported HTTP method */
    response = MHD_create_response_from_buffer(strlen(METHOD_ERROR),
                                               (void *) METHOD_ERROR,
                                               MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection,
                             MHD_HTTP_METHOD_NOT_ACCEPTABLE,
                             response);
    MHD_destroy_response(response);
    return ret;
}


/**
 * Callback called upon completion of a request.
 * Decrements session reference counter.
 *
 * @param cls not used
 * @param connection connection that completed
 * @param con_cls session handle
 * @param toe status code
 */
static void
request_completed_callback(void *cls,
                           struct MHD_Connection *connection,
                           void **con_cls,
                           enum MHD_RequestTerminationCode toe) {
    struct Request *request = *con_cls;

    if (NULL == request)
        return;
    if (NULL != request->session)
        request->session->rc--;
    if (NULL != request->pp)
        MHD_destroy_post_processor(request->pp);
    free(request);
}


/**
 * Clean up handles of sessions that have been idle for
 * too long.
 */
static void
expire_sessions() {
    struct Session *pos;
    struct Session *prev;
    struct Session *next;
    time_t now;

    now = time(NULL);
    prev = NULL;
    pos = sessions;
    while (NULL != pos) {
        next = pos->next;
        if (now - pos->start > 60 * 60) {
            /* expire sessions after 1h */
            if (NULL == prev)
                sessions = pos->next;
            else
                prev->next = next;
            free(pos);
        }
        else
            prev = pos;
        pos = next;
    }
}


/**
 * Call with the port number as the only argument.
 * Never terminates (other than by signals, such as CTRL-C).
 */
int
main(int argc, char *const *argv) {
    struct MHD_Daemon *d;
    struct timeval tv;
    struct timeval *tvp;
    fd_set rs;
    fd_set ws;
    fd_set es;
    int max;
    unsigned MHD_LONG_LONG mhd_timeout;

    if (argc != 2) {
       //printf("%s PORT\n", argv[0]);
        return 1;
    }
    /* initialize PRNG */
    srandom((unsigned int) time(NULL));
    d = MHD_start_daemon(MHD_USE_DEBUG,
                         atoi(argv[1]),
                         NULL, NULL,
                         &create_response, NULL,
                         MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 15,
                         MHD_OPTION_NOTIFY_COMPLETED, &request_completed_callback, NULL,
                         MHD_OPTION_END);
    if (NULL == d)
        return 1;
    while (1) {
        expire_sessions();
        max = 0;
        FD_ZERO (&rs);
        FD_ZERO (&ws);
        FD_ZERO (&es);
        if (MHD_YES != MHD_get_fdset (d, &rs, &ws, &es, &max))
            break; /* fatal internal error */
        if (MHD_get_timeout(d, &mhd_timeout) == MHD_YES) {
            tv.tv_sec = mhd_timeout / 1000;
            tv.tv_usec = (mhd_timeout - (tv.tv_sec * 1000)) * 1000;
            tvp = &tv;
        }
        else
            tvp = NULL;
        select(max + 1, &rs, &ws, &es, tvp);
        MHD_run(d);
    }
    MHD_stop_daemon(d);
    return 0;
}