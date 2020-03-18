/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

param_parser.c - modul pro zpracování argumentů příkazové řádky

**/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "param_parser.h"
#include "util.h"

enum arg_type {
    ARG_TYPE_TEXT = 111,         // "text"
    ARG_TYPE_PARAM_SHORT = 222,  // "-X"
    ARG_TYPE_PARAM_LONG = 333,   // "-Xvalue"
    ARG_TYPE_INVALID = 444       // NULL, "", '-'
};

enum arg_type get_arg_type(char *arg) {
    if (arg == NULL || arg[0] == '\0' || (arg[0] == '-' && arg[1] == '\0')) {
        return ARG_TYPE_INVALID;
    } else if (arg[0] != '-') {
        return ARG_TYPE_TEXT;
    } else if (arg[2] == '\0') {
        return ARG_TYPE_PARAM_SHORT;
    } else {
        return ARG_TYPE_PARAM_LONG;
    }
}

void load_param_value(struct par_content *result, char loading_param_value, char *value) {
    switch (loading_param_value) {
        case 'U':
            if (!result->is_url && !result->is_feedfile) {
                result->is_url = true;
                result->url = malloc_string(1, value);
            } else {
                result->err_code = PAR_ERR_SOURCE_MULTIPLE;
                result->err_msg = malloc_string(1, "Zadano vice zdroju feedu!\nMuze byt zadana pouze jedna adresa URL nebo jeden soubor s feedy.\n");
            }
            break;
        case 'f':
            result->is_feedfile = true;
            result->feedfile = malloc_string(1, value);
            break;
        case 'c':
            result->certfile[result->num_certfile] = malloc_string(1, value);
            result->num_certfile++;
            break;
        case 'C':
            result->certaddr[result->num_certaddr] = malloc_string(1, value);
            result->num_certaddr++;
            break;
    }
}

char process_param(struct par_content *result, char loading_param_value, char param) {
    if (loading_param_value != 'U') {
        result->err_code = PAR_ERR_ARGUMENT_NOVALUE;
        char c1[2];
        sprintf(c1, "%c", loading_param_value);
        result->err_msg = malloc_string(3, "Chybi hodnota u zadaneho argumentu!\nU argumentu -", c1, " je hodnota povinna.\n");
        return 'U';
    }
    switch (param) {
        case 'f':
            if (!result->is_url && !result->is_feedfile) {
                return 'f';
            } else {
                result->err_code = PAR_ERR_SOURCE_MULTIPLE;
                result->err_msg = malloc_string(1, "Zadano vice zdroju feedu!\nMuze byt zadana pouze jedna adresa URL nebo jeden soubor s feedy.\n");
                return 'U';
            }
        case 'c':
            if (result->num_certfile < MAX_CERTFILES) {
                return 'c';
            } else {
                result->err_code = PAR_ERR_CERTFILE_TOOMANY;
                char num[12];
                sprintf(num, "%d", MAX_CERTFILES);
                result->err_msg = malloc_string(3, "Bylo zadano prilis mnoho souboru s certifikaty!\nJe povoleno maximalne ", num, " souboru s certifikaty.\n");
                return 'U';
            }
        case 'C':
            if (result->num_certaddr < MAX_CERTADDRESSES) {
                return 'C';
            } else {
                result->err_code = PAR_ERR_CERTADDR_TOOMANY;
                char num[12];
                sprintf(num, "%d", MAX_CERTADDRESSES);
                result->err_msg = malloc_string(3, "Bylo zadano prilis mnoho adres s certifikaty!\nJe povoleno maximalne ", num, " adres s certifikaty.\n");
                return 'U';
            }
        case 'T':  // include creation or change time
            result->is_t = true;
            return 'U';
        case 'a':  // include author name or email
            result->is_a = true;
            return 'U';
        case 'u':  // include URL
            result->is_u = true;
            return 'U';
        case 'h':  // show help
            result->is_help = true;
            return 'U';
        default:
            result->err_code = PAR_ERR_ARGUMENT_UNKNOWN;
            char c2[2];
            sprintf(c2, "%c", param);
            result->err_msg = malloc_string(3, "Neznamy argument!\nArgument -", c2, " neni znam.\n");
            return 'U';
    }
}

struct par_content parse_parameters(int argc, char *argv[]) {

    struct par_content result = get_par_content();
    char loading_param_value = 'U'; // U = URL, f = feedfile, c = certfile, C = certaddr

    for (int i = 1; i < argc; i++) {
        switch (get_arg_type(argv[i])) {
            case ARG_TYPE_TEXT:
                load_param_value(&result, loading_param_value, argv[i]);
                loading_param_value = 'U';
                if (result.err_code != PAR_ERR_OK) {
                    return result;
                }
                break;
            case ARG_TYPE_PARAM_SHORT:
                loading_param_value = process_param(&result, loading_param_value, argv[i][1]);
                if (result.err_code != PAR_ERR_OK) {
                    return result;
                }
                break;
            case ARG_TYPE_PARAM_LONG:
                loading_param_value = process_param(&result, loading_param_value, argv[i][1]);
                if (result.err_code != PAR_ERR_OK) {
                    return result;
                }
                load_param_value(&result, loading_param_value, &argv[i][2]);
                loading_param_value = 'U';
                if (result.err_code != PAR_ERR_OK) {
                    return result;
                }
                break;
            case ARG_TYPE_INVALID:
                break;
        }
    }

    if (loading_param_value != 'U') {
        result.err_code = PAR_ERR_ARGUMENT_NOVALUE;
        char c1[2];
        sprintf(c1, "%c", loading_param_value);
        result.err_msg = malloc_string(3, "Chybi hodnota u zadaneho argumentu!\nU argumentu -", c1, " je hodnota povinna.\n");
        return result;
    }

    if (!result.is_url && !result.is_feedfile) {
        result.err_code = PAR_ERR_SOURCE_MISSING;
        result.err_msg = malloc_string(1, "Nebyl zadan zadny zdroj!\nJe nutne zadat jednu adresu URL nebo soubor se zdroji parametrem -f.\n");
        return result;
    }

    return result;
}

struct par_content get_par_content() {
    struct par_content result;
    result.is_help = false;
    result.is_url = false;
    result.url = NULL;
    result.is_feedfile = false;
    result.feedfile = NULL;
    result.num_certfile = 0;
    for (int i = 0; i < MAX_CERTFILES; i++) {
        result.certfile[i] = NULL;
    }
    result.num_certaddr = 0;
    for (int i = 0; i < MAX_CERTADDRESSES; i++) {
        result.certaddr[i] = NULL;
    }
    result.is_t = false;
    result.is_a = false;
    result.is_u = false;
    result.err_code = PAR_ERR_OK;
    result.err_msg = NULL;
    return result;
}

void print_par_content(struct par_content *content) {
    printf("=== PAR_CONTENT BEGIN ===\n");
    printf("is_help: %s\n", content->is_help ? "true" : "false");
    printf("is_url: %s\n", content->is_url ? "true" : "false");
    printf("url: %s\n", content->url == NULL ? "NULL" : content->url);
    printf("is_feedfile: %s\n", content->is_feedfile ? "true" : "false");
    printf("feedfile: %s\n", content->feedfile == NULL ? "NULL" : content->feedfile);
    printf("num_certfile: %d\n", content->num_certfile);
    for (int i = 0; i < content->num_certfile; i++) {
        printf("certfile[%d]: %s\n", i, content->certfile[i] == NULL ? "NULL" : content->certfile[i]);
    }
    printf("num_certaddr: %d\n", content->num_certaddr);
    for (int i = 0; i < content->num_certaddr; i++) {
        printf("certaddr[%d]: %s\n", i, content->certaddr[i] == NULL ? "NULL" : content->certaddr[i]);
    }
    printf("is_t: %s\n", content->is_t ? "true" : "false");
    printf("is_a: %s\n", content->is_a ? "true" : "false");
    printf("is_u: %s\n", content->is_u ? "true" : "false");
    printf("err_code: %d\n", content->err_code);
    printf("err_msg: %s\n", content->err_msg == NULL ? "NULL" : content->err_msg);
    printf("=== PAR_CONTENT END ===\n");
}

void free_par_content(struct par_content *content) {
    if (content->is_url) {
        free(content->url);
        content->url = NULL;
    }
    if (content->is_feedfile) {
        free(content->feedfile);
        content->feedfile = NULL;
    }
    for (int i = 0; i < content->num_certfile; i++) {
        free(content->certfile[i]);
        content->certfile[i] = NULL;
    }
    for (int i = 0; i < content->num_certaddr; i++) {
        free(content->certaddr[i]);
        content->certaddr[i] = NULL;
    }
    if (content->err_code != PAR_ERR_OK) {
        free(content->err_msg);
        content->err_msg = NULL;
    }
}
