/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

http_downloader.h - modul pro stažení HTTP či HTTPS obsahu s podporou TLS
**/

#ifndef HTTP_DOWNLOADER_H_
#define HTTP_DOWNLOADER_H_

enum dw_error {
    DW_ERR_OK = 0,          // download ok
    DW_ERR_URL = 100,       // url is invalid
    DW_ERR_CONN = 101,      // connection error
    DW_ERR_CERT = 102,      // certificates error
    DW_ERR_WRITE = 103,     // failed to create request
    DW_ERR_READ = 104,      // failed to read response
    DW_ERR_HTTP = 105       // HTTP response code was other than 200
};

struct dw_content {
    char*           content;
    enum dw_error   err_code;
    char*           err_msg;
};

void provide_certfile(char *path);

void provide_certaddr(char *path);

void free_certs();

struct dw_content try_download(char *url);

struct dw_content get_dw_content();

void print_dw_content(struct dw_content *content);

void free_dw_content(struct dw_content *content);

#endif // HTTP_DOWNLOADER_H_
