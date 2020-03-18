/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

url_parser.h - modul pro zpracování URL zdroje

**/

#ifndef URL_PARSER_H_
#define URL_PARSER_H_

#include <stdbool.h>

enum url_error {
    URL_ERR_OK = 0,                 // URL ok
    URL_ERR_PROTOCOL = 100,         // URL does not start with http:// nor https://
    URL_ERR_HOST = 101,             // host is invalid
    URL_ERR_PORT = 102              // port is invalid
};

struct url_content {
    bool            is_https;
    char*           host;
    char*           port;
    char*           resource;
    enum url_error  err_code;
    char*           err_msg;
};

struct url_content parse_url(char *url);

struct url_content get_url_content();

void print_url_content(struct url_content *content);

void free_url_content(struct url_content *content);

#endif // URL_PARSER_H_
