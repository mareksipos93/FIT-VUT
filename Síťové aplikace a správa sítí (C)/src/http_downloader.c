/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

http_downloader.c - modul pro stažení HTTP či HTTPS obsahu s podporou TLS

**/

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "http_downloader.h"
#include "url_parser.h"
#include "util.h"

char num_certfile = 0;
char **certfile = NULL;
char num_certaddr = 0;
char **certaddr = NULL;
BIO *bio;
SSL_CTX *ctx;
SSL *ssl;

char *malloc_http_message(int code) {
    char *msg;
    char scode[12];
    sprintf(scode, "%d", code);

    // ??? - pri presmerovani vypsat na JAKOU adresu to bylo presunuto ???

    if (code == 200) {
        msg = "Vse probehlo v poradku";
    } else if (code == 300) {
        msg = "Pozadovany soubor neni jednoznacny";
    } else if (code == 301) {
        msg = "Pozadovany soubor byl trvale premisten na jinou adresu";
    } else if (code == 302) {
        msg = "Pozadovany soubor byl docasne premisten na jinou adresu";
    } else if (code == 303) {
        msg = "Pozadovany soubor byl premisten na jinou adresu";
    } else if (code == 304) {
        msg = "Server neodeslal soubor, protoze se nijak nelisi od predchoziho pozadavku";
    } else if (code == 307) {
        msg = "Pozadovany soubor byl docasne premisten na jinou adresu";
    } else if (code == 400) {
        msg = "Zadany pozadavek je neplatny";
    } else if (code == 401) {
        msg = "Server vyzaduje autorizaci";
    } else if (code == 403) {
        msg = "K pozadovanemu souboru neni povolen pristup";
    } else if (code == 404) {
        msg = "Pozadovany soubor nebyl na serveru nalezen";
    } else if (code == 410) {
        msg = "Pozadovany soubor se na serveru jiz nenachazi";
    } else if (code == 500) {
        msg = "Doslo k interni chybe serveru";
    } else if (code == 501) {
        msg = "Server neni schopen pozadavek zpracovat";
    } else if (code == 502) {
        msg = "Backend serveru nedokazal zpracovat pozadavek";
    } else if (code == 503) {
        msg = "Server nemuze pozadavek v tuto chvili zpracovat";
    } else if (code == 504) {
        msg = "Backend serveru nezpracoval pozadavek v casovem limitu";
    } else if (code / 100 == 1) {
        msg = "Jina zprava s informacemi";
    } else if (code / 100 == 2) {
        msg = "Jina zprava s uspechem";
    } else if (code / 100 == 3) {
        msg = "Jina zprava s presmerovanim";
    } else if (code / 100 == 4) {
        msg = "Jina zprava s chybou klienta";
    } else if (code / 100 == 5) {
        msg = "Jina zprava s chybou serveru";
    } else {
        msg = "Neznama odpoved serveru (chyba)";
    }

    return malloc_string(4, "Server vratil kod odpovedi #", scode, ": ", msg);
}

void provide_certfile(char *path) {
    num_certfile++;
    certfile = (char **) realloc(certfile, num_certfile * sizeof(char *));
    certfile[num_certfile-1] = path;
}

void provide_certaddr(char *path) {
    num_certaddr++;
    certaddr = (char **) realloc(certaddr, num_certaddr * sizeof(char *));
    certaddr[num_certaddr-1] = path;
}

void free_certs() {
    if (num_certfile > 0) {
        free(certfile);
        num_certfile = 0;
    }
    if (num_certaddr > 0) {
        free(certaddr);
        num_certaddr = 0;
    }
}

void free_openssl(bool is_https) {

    // MERLIN ONLY (SIGSEGV on Eva)
    #ifdef __linux__
    BIO_free_all(bio);
    #endif

    if (is_https) {
        SSL_free(ssl);
    }
    FIPS_mode_set(0);
    EVP_cleanup();
    SSL_COMP_free_compression_methods();
    CRYPTO_cleanup_all_ex_data();
    ERR_remove_state(0);
    ERR_free_strings();
}

struct dw_content try_download(char *url) {

    print_debug("HTTP_DOWNLOADER - Inicializace\n");
    struct dw_content result = get_dw_content();
    bool is_https = false;
    char *host_port = NULL;

    // PARSE URL
    print_debug("HTTP_DOWNLOADER - Zpracovani URL '%s'\n", url);
    struct url_content parsed_url = parse_url(url);
    /* DEBUG */ //print_url_content(&parsed_url); /* DEBUG */

    if (parsed_url.err_code != URL_ERR_OK) {
        result.err_code = DW_ERR_URL;
        result.err_msg = malloc_string(1, parsed_url.err_msg);
        free_url_content(&parsed_url);
        return result;
    }
    is_https = parsed_url.is_https;
    host_port = malloc_string(3, parsed_url.host, ":", parsed_url.port);

    // OPENSSL INIT
    print_debug("HTTP_DOWNLOADER - Inicializace OpenSSL\n");
    SSL_load_error_strings();
    SSL_library_init();

    if (!is_https) {
        print_debug("HTTP_DOWNLOADER - Obycejne spojeni\n");
        // CONNECT NON-SECURE
        bio = BIO_new_connect(host_port);
        if (bio == NULL) {
            result.err_code = DW_ERR_CONN;
            result.err_msg = malloc_string(2, "Nepodarilo se navazat nezabezpecene spojeni. Duvod: ", ERR_reason_error_string(ERR_get_error()));
            free_openssl(is_https);
            free(host_port);
            free_url_content(&parsed_url);
            return result;
        }
        while (BIO_do_connect(bio) <= 0) {
            if (!BIO_should_retry(bio)) {
                result.err_code = DW_ERR_CONN;
                result.err_msg = malloc_string(2, "Nepodarilo se navazat nezabezpecene spojeni. Duvod: ", ERR_reason_error_string(ERR_get_error()));
                free_openssl(is_https);
                free(host_port);
                free_url_content(&parsed_url);
                return result;
            } else {
                continue;
            }
        }
    } else {
        print_debug("HTTP_DOWNLOADER - SSL spojeni\n");
        ctx = SSL_CTX_new(SSLv23_client_method());

        // LOAD CERTIFICATES
        if (num_certfile == 0 && num_certaddr == 0) {
            SSL_CTX_set_default_verify_paths(ctx);
        } else {
            for (int i = 0; i < num_certfile; i++) {
                SSL_CTX_load_verify_locations(ctx, certfile[i], NULL);
            }
            for (int i = 0; i < num_certaddr; i++) {
                SSL_CTX_load_verify_locations(ctx, NULL, certaddr[i]);
            }
        }

        // CONNECT SECURE
        bio = BIO_new_ssl_connect(ctx);
        BIO_get_ssl(bio, &ssl);
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
        BIO_set_conn_hostname(bio, host_port);

        while (BIO_do_connect(bio) <= 0) {
            if (!BIO_should_retry(bio)) {
                result.err_code = DW_ERR_CONN;
                result.err_msg = malloc_string(2, "Nepodarilo se navazat zabezpecene spojeni. Duvod: ", ERR_reason_error_string(ERR_get_error()));
                free_openssl(is_https);
                free(host_port);
                free_url_content(&parsed_url);
                return result;
            } else {
                continue;
            }
        }
        print_debug("SSL_get_verify_result(ssl) vrátilo %ld\n", SSL_get_verify_result(ssl));
        if (SSL_get_verify_result(ssl) != X509_V_OK) {
            result.err_code = DW_ERR_CERT;
            result.err_msg = malloc_string(1, "Overeni certifikatu serveru selhalo.");
            free_openssl(is_https);
            free(host_port);
            free_url_content(&parsed_url);
            return result;
        }

    }

    free(host_port);

    // WRITE TO SOCKET
    print_debug("HTTP_DOWNLOADER - Zapis pozadavku do socketu\n");
    char *request = malloc_string(5, 
        "GET ", parsed_url.resource, " HTTP/1.0\r\n"
        "Host: ", parsed_url.host, "\r\n"
        "User-Agent: Feedreader-xsipos03\r\n"
        "Accept: application/xml\r\n"
        "Accept-Charset: UTF-8,*\r\n"
        "Cache-Control: private, no-store, max-age=0\r\n"
        "Connection: close\r\n"
        "\r\n");
    while (BIO_write(bio, request, (strlen(request)+1) * sizeof(char)) <= 0) {
        if (!BIO_should_retry(bio)) {
            result.err_code = DW_ERR_WRITE;
            result.err_msg = malloc_string(1, "Nepodarilo se zaslat pozadavek na stazeni.");
            free_openssl(is_https);
            free_url_content(&parsed_url);
            free(request);
            return result;
        } else {
            continue;
        }
    }
    /* DEBUG */ //printf("--- HEADER ---\n%s\n", request); /* DEBUG */
    free(request);

    // READ FROM SOCKET
    print_debug("HTTP_DOWNLOADER - Cteni odpovedi ze socketu\n");
    int buffer_size = 1024*128;
    int buffer_increment = 1024*128;
    int buffer_used = 0;
    char *response = (char *) malloc(buffer_size+1);
    while(42) {
        int len = BIO_read(bio, response+buffer_used, buffer_size-buffer_used);

        if (len < 0) {
            if (!BIO_should_retry(bio)) {
                result.err_code = DW_ERR_READ;
                result.err_msg = malloc_string(1, "Nepodarilo se stahnout data.");
                free_openssl(is_https);
                free_url_content(&parsed_url);
                free(response);
                return result;
            } else {
                continue;
            }
        } else if (len == 0) {
            break;
        } else {
            buffer_used += len;
            if (buffer_size-buffer_used <= buffer_increment/2) {
                buffer_size += buffer_increment;
                response = (char *) realloc(response, buffer_size+1);
            }
            continue;
        }
    }
    response[buffer_used] = '\0';

    /* DEBUG */ //printf("--- RESPONSE ---\n%s\n", response); /* DEBUG */

    // ??? - mozna kontrola jestli content-length odpovida telu odpovedi ???

    // CHECK HTTP RESPONSE
    print_debug("HTTP_DOWNLOADER - Kontrola HTTP kodu odpovedi\n");
    int http_code = 0;      // Final code is saved here
    char http_scode[4];     // Buffer for reading code
    int reading_digit = -1; // Number of digit read (-1 0 1 2)
    for (int i = 0; i < strlen(response)+1; i++) {
        if (response[i] == '\0') {
            break;
        }
        if (response[i] == ' ') {
            reading_digit = 0;
            continue;
        }
        if (reading_digit >= 0) {
            http_scode[reading_digit] = response[i];
            if (reading_digit < 2) {
                reading_digit++;
                continue;
            } else {
                http_scode[reading_digit+1] = '\0';
                http_code = atoi(http_scode);
                break;
            }
        }
    }
    if (http_code != 200) {
        print_debug("HTTP_DOWNLOADER - Vysledny HTTP kod neni 200 (OK) - misto toho je %d\n", http_code);
        result.err_code = DW_ERR_HTTP;
        result.err_msg = malloc_http_message(http_code);
        free_openssl(is_https);
        free_url_content(&parsed_url);
        free(response);
        return result;
    }

    print_debug("HTTP_DOWNLOADER - Predani vysledneho dokumentu\n");
    result.content = response;

    // CLEANUP
    print_debug("HTTP_DOWNLOADER - Uvolnovani\n");
    free_openssl(is_https);
    free_url_content(&parsed_url);

    return result;
}

struct dw_content get_dw_content() {
    struct dw_content result;
    result.content = NULL;
    result.err_code = DW_ERR_OK;
    result.err_msg = NULL;
    return result;
}

void print_dw_content(struct dw_content *content) {
    printf("=== DW_CONTENT BEGIN ===\n");
    printf("content: %s\n", content->content == NULL ? "NULL" : content->content);
    printf("err_code: %d\n", content->err_code);
    printf("err_msg: %s\n", content->err_msg == NULL ? "NULL" : content->err_msg);
    printf("=== DW_CONTENT END ===\n");
}

void free_dw_content(struct dw_content *content) {
    if (content->content != NULL) {
        free(content->content);
        content->content = NULL;
    }

    if (content->err_code != DW_ERR_OK) {
        free(content->err_msg);
        content->err_msg = NULL;
    }
}
