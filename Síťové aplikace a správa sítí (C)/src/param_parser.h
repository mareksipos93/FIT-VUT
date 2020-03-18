/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

param_parser.h - modul pro zpracování argumentů příkazové řádky

**/

#ifndef PARAM_PARSER_H_
#define PARAM_PARSER_H_

#include <stdbool.h>
#include "util.h"

enum par_error {
    PAR_ERR_OK = 0,                   // param check ok
    PAR_ERR_SOURCE_MISSING = 100,     // no source specified
    PAR_ERR_SOURCE_MULTIPLE = 101,    // multiple sources specified
    PAR_ERR_CERTFILE_TOOMANY = 200,   // too many files with certs
    PAR_ERR_CERTADDR_TOOMANY = 300,   // too many cert addresses
    PAR_ERR_ARGUMENT_NOVALUE = 400,   // argument value is missing
    PAR_ERR_ARGUMENT_UNKNOWN = 401    // unknown argument
};

struct par_content {
    bool            is_help;                        // show help
    bool            is_url;                         // source URL is standalone
    char*           url;
    bool            is_feedfile;                    // -f = file with feed sources
    char*           feedfile;
    int             num_certfile;                   // -c = file with certs (multiple allowed)
    char*           certfile[MAX_CERTFILES];
    int             num_certaddr;                   // -C = address with certs (multiple allowed)
    char*           certaddr[MAX_CERTADDRESSES];
    bool            is_t;                           // -T = include creation or change time
    bool            is_a;                           // -a = include author name or email
    bool            is_u;                           // -u = include URL
    enum par_error  err_code;
    char*           err_msg;
};

struct par_content parse_parameters(int argc, char *argv[]);

struct par_content get_par_content();

void print_par_content(struct par_content *content);

void free_par_content(struct par_content *content);

#endif // PARAM_PARSER_H_
