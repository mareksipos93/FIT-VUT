/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

feedfile_parser.h - modul pro zpracování vstupního souboru se zdroji pro feedy

**/

#ifndef FEEDFILE_PARSER_H_
#define FEEDFILE_PARSER_H_

struct ff_content {
    int             num_feeds;
    char**          feeds;
    int             err_code;
};

struct ff_content parse_feedfile(char *file);	// EOF = end
struct ff_content parse_feedstring(char *str);	// '\0' = end

struct ff_content get_ff_content();

void print_ff_content(struct ff_content *content);

void free_ff_content(struct ff_content *content);

#endif // FEEDFILE_PARSER_H_
