/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

xml_parser.h - modul pro zpracování XML těla a extrakci potřebných dat

**/


#ifndef XML_PARSER_H_
#define XML_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum xml_error {
    XMLPARSE_ERR_OK = 0,               // XML parsed ok
    XMLPARSE_ERR_EMPTY = 100,          // resulting XML was empty
    XMLPARSE_ERR_OTHER = 300,          // unknown error
};

struct xml_item {
    char*       name;
    bool        has_time;
    char*       time;
    bool        has_author;
    char*       author;
    bool        has_url;
    char*       url;
};

struct xml_content {
    char*               name;
    int                 num_items;
    struct xml_item*    items;
    enum xml_error      err_code;
    char*               err_msg;
};

struct xml_content parse_xml(char *xml);

struct xml_item get_xml_item();

void print_xml_item(struct xml_item *item);

void free_xml_item(struct xml_item *item);

struct xml_content get_xml_content();

void print_xml_content(struct xml_content *content);

void free_xml_content(struct xml_content *content);

#endif // XML_PARSER_H_
