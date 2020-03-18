/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

feedfile_parser.c - modul pro zpracování vstupního souboru se zdroji pro feedy

**/

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feedfile_parser.h"
#include "util.h"

void parse(struct ff_content *result, char *str, FILE *file) {
    bool comment = false;   // FSM - inside comment
    bool source = false;    // FSM - inside string with source
    char buffer[FEEDFILE_BUFFERSIZE];
    int  buffer_realloc = 0;
    int  buffer_used = 0;
    char c;
    int i = 0;
    while(42) {

        if (str != NULL) {
            c = str[i];
            i++;
        } else if (file != NULL) {
            c = getc(file);
        } else {
            return;
        }

        if ((c == EOF || (str != NULL && c == '\0') || c == '\n' || isspace(c)) && source) {
            strncpy(result->feeds[result->num_feeds-1] + ((buffer_realloc*FEEDFILE_BUFFERSIZE) * sizeof(char)), buffer, buffer_used);
            result->feeds[result->num_feeds-1][(buffer_realloc*FEEDFILE_BUFFERSIZE) + buffer_used] = '\0';
            buffer_realloc = 0;
            buffer_used = 0;
            source = false; 
        }
        
        if (c == EOF || (str != NULL && c == '\0')) {
            break;
        }
        else if (c == '\n') {
            comment = false;
        }
        else if (isspace(c)) {
            continue;
        }
        else if (c == '#' && !source && !comment) {
            comment = true;
        }
        else {
            if (comment) {
                continue;
            }
            else if (source) {
                buffer[buffer_used] = c;
                buffer_used++;
                if (buffer_used == FEEDFILE_BUFFERSIZE) {
                    buffer_realloc++;
                    result->feeds[result->num_feeds-1] = (char *) realloc(result->feeds[result->num_feeds-1], (FEEDFILE_BUFFERSIZE*(1+buffer_realloc)+1) * sizeof(char));
                    strncpy(result->feeds[result->num_feeds-1] + (((buffer_realloc-1)*FEEDFILE_BUFFERSIZE) * sizeof(char)), buffer, FEEDFILE_BUFFERSIZE);
                    buffer_used = 0;
                }
            }
            else {
                source = true;
                buffer[buffer_used] = c;
                buffer_used++;

                result->feeds = (char**) realloc(result->feeds, (result->num_feeds+1) * sizeof(char *));
                result->feeds[result->num_feeds] = (char *) malloc((FEEDFILE_BUFFERSIZE+1) * sizeof(char));
                result->num_feeds++;
            }
        }

    }
}

struct ff_content parse_feedfile(char *file) {

    struct ff_content result = get_ff_content();

    FILE *f = fopen(file, "r");
    if (f == NULL) {
        result.err_code = errno;
        return result;
    }
    
    parse(&result, NULL, f);

    fclose(f);

    return result;
}

struct ff_content parse_feedstring(char *str) {

    struct ff_content result = get_ff_content();
    
    parse(&result, str, NULL);

    return result;
}

struct ff_content get_ff_content() {
    struct ff_content result;
    result.num_feeds = 0;
    result.feeds = NULL;
    result.err_code = 0;
    return result;
}

void print_ff_content(struct ff_content *content) {
    printf("=== FF_CONTENT BEGIN ===\n");
    printf("num_feeds: %d\n", content->num_feeds);
    for (int i = 0; i < content->num_feeds; i++) {
        printf("feeds[%d]: %s\n", i, content->feeds[i]);
    }
    printf("err_code: %d\n", content->err_code);
    printf("=== FF_CONTENT END ===\n");
}

void free_ff_content(struct ff_content *content) {
    if (content->num_feeds > 0) {
        for (int i = 0; i < content->num_feeds; i++) {
            free(content->feeds[i]);
            content->feeds[i] = NULL;
        }
        free(content->feeds);
        content->feeds = NULL;
    }
}
