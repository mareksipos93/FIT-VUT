/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

url_parser.c - modul pro zpracování URL zdroje

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url_parser.h"
#include "util.h"

struct url_content parse_url(char *url) {

    struct url_content result = get_url_content();

    int scan_phase = 0; // 0 = protocol, 1 = host, 2 = port, 3 = resource
    int scan_num = 0;
    for (int i = 0; i < strlen(url)+1; i++) {

        char c = url[i];

        if (scan_phase == 0) {
            if ((scan_num == 0 && (c != 'h' && c != 'H')) || 
                (scan_num == 1 && (c != 't' && c != 'T')) ||
                (scan_num == 2 && (c != 't' && c != 'T')) ||
                (scan_num == 3 && (c != 'p' && c != 'P')) ||
                (scan_num == 5 && (c != ':')) ||
                (scan_num == 6 && (c != '/')) ||
                (scan_num == 7 && (c != '/'))) {
                    result.err_code = URL_ERR_PROTOCOL;
                    result.err_msg = malloc_string(3, "Adresa '", url, "' nezacina HTTP:// ani HTTPS://.");
                    return result;
            } else if (scan_num == 4) {
                if (c == ':') {
                    result.is_https = false;
                    scan_num = 6;
                    continue;
                } else if (c == 's' || c == 'S') {
                    result.is_https = true;
                    scan_num++;
                    continue;
                } else {
                    result.err_code = URL_ERR_PROTOCOL;
                    result.err_msg = malloc_string(3, "Adresa '", url, "' nezacina HTTP:// ani HTTPS://.");
                    return result;
                }
            } else {
                scan_num++;
                if (scan_num == 8) {
                    scan_phase = 1;
                    scan_num = 0;
                }
                continue;
            }
            
        }
        if (scan_phase == 1) {
            if (c == ':' || c == '/' || c == '\0') {
                if (scan_num < 3) {
                    result.err_code = URL_ERR_HOST;
                    result.err_msg = malloc_string(3, "Hostitel adresy '", url, "' je prilis kratky.");
                    return result;
                } else {
                    result.host = (char *) malloc((scan_num+1) * sizeof(char));
                    strncpy(result.host, url+i-scan_num, scan_num);
                    result.host[scan_num] = '\0';

                    if (c == ':') {
                        scan_phase = 2;
                        scan_num = 0;
                    } else {
                        if (result.is_https) {
                            result.port = malloc_string(1, "443");
                        } else {
                            result.port = malloc_string(1, "80");
                        }

                        if (c == '/') {
                            scan_phase = 3;
                            scan_num = 0;
                        } else {
                            result.resource = malloc_string(1, "/");
                            break;
                        }
                    }
                    continue;
                }
            } else {
                scan_num++;
                continue;
            }
        }
        if (scan_phase == 2) {
            if (c == '/' || c == '\0') {
                if (scan_num < 1) {
                    result.err_code = URL_ERR_PORT;
                    result.err_msg = malloc_string(3, "V adrese '", url, "' nebyl zadan port za dvojteckou.");
                    return result;
                } else {
                    result.port = (char *) malloc((scan_num+1) * sizeof(char));
                    strncpy(result.port, url+i-scan_num, scan_num);
                    result.port[scan_num] = '\0';

                    if (c == '/') {
                        scan_phase = 3;
                        scan_num = 0;
                    } else if (c == '\0') {
                        result.resource = malloc_string(1, "/");
                        break;
                    }
                    continue;
                }
            } else {
                scan_num++;
                continue;
            }
        }
        if (scan_phase >= 3) {
            if (c == '\0') {
                result.resource = (char *) malloc((scan_num+2) * sizeof(char));
                result.resource[0] = '/';
                strncpy(result.resource+1, url+i-scan_num, scan_num);
                result.resource[scan_num+1] = '\0';
                break;
            } else {
                scan_num++;
                continue;
            }
        }
    }

    return result;
}

struct url_content get_url_content() {
    struct url_content result;
    result.is_https = false;
    result.host = NULL;
    result.port = NULL;
    result.resource = NULL;
    result.err_code = URL_ERR_OK;
    result.err_msg = NULL;
    return result;
}

void print_url_content(struct url_content *content) {
    printf("=== URL_CONTENT BEGIN ===\n");
    printf("is_https: %s\n", content->is_https ? "true" : "false");
    printf("host: %s\n", content->host == NULL ? "NULL" : content->host);
    printf("port: %s\n", content->port == NULL ? "NULL" : content->port);
    printf("resource: %s\n", content->resource == NULL ? "NULL" : content->resource);
    printf("err_code: %d\n", content->err_code);
    printf("err_msg: %s\n", content->err_msg == NULL ? "NULL" : content->err_msg);
    printf("=== URL_CONTENT END ===\n");
}

void free_url_content(struct url_content *content) {
    if (content->host != NULL) {
        free(content->host);
        content->host = NULL;
    }
    if (content->port != NULL) {
        free(content->port);
        content->port = NULL;
    }
    if (content->resource != NULL) {
        free(content->resource);
        content->resource = NULL;
    }
    if (content->err_code != URL_ERR_OK) {
        free(content->err_msg);
        content->err_msg = NULL;
    }
}
