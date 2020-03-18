/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

xml_parser.c - modul pro zpracování XML těla a extrakci potřebných dat

**/

#include <libxml/parser.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "xml_parser.h"
#include "util.h"


xmlNodePtr getNotNullXmlNode(int num, ... ) {
    xmlNodePtr result = NULL;

    va_list valist;
    va_start(valist, num);

    for (int i = 0; i < num; i++) {
        result = va_arg(valist, xmlNodePtr);

        if (result != NULL) {
            break;
        }
    }

    va_end(valist);

    return result;
}

xmlNodePtr findXmlNodeByPath(xmlNodePtr root, int num, ... ) {
    //printf("findXmlNodeByPath (root = %s, num = %d)\n", (char *)root->name, num);
    xmlNodePtr result = NULL;

    va_list valist;
    va_start(valist, num);

    result = root;
    for (int i = 0; i < num; i++) {
        char *param = va_arg(valist, char *);
        result = result->xmlChildrenNode;
        //printf("vyhledavam v urovni %d, hledany uzel: <%s>\n", i, param);

        while (result != NULL) {
            if (!xmlStrcmp(result->name, (const xmlChar *) param)) {
                //printf("spravny uzel <%s>, jdu o uroven niz\n", param);
                break;
            } else {
                //printf("spatny uzel <%s>, hledam dal...\n", (char *) result->name);
                result = xmlNextElementSibling(result);
                //result = result->next;
            }
        }
        if (result == NULL) {
            //printf("Nepodarilo se najit uzel <%s> :(\n", param);
            break;
        } else {
            continue;
        }
    }

    if (result != NULL) {
        //printf("Nalezen vysledny uzel <%s> :)\n", result->name);
    }

    va_end(valist);

    return result;
}

xmlNodePtr findNextXmlNodeOnSameLevel(xmlNodePtr root, char *name) {
    //xmlNodePtr result = root->next;
    xmlNodePtr result = xmlNextElementSibling(root);

    while (result != NULL) {
        if (!xmlStrcmp(result->name, (const xmlChar *) name)) {
            break;
        } else {
            result = xmlNextElementSibling(result);
            //result = result->next;
        }
    }

    return result;
}

struct xml_content parse_xml(char *xml) {

    struct xml_content result = get_xml_content();

    // Init
    xmlDocPtr xmlDoc;
    xmlNodePtr xmlRoot;
    xmlNodePtr xmlNode;
    xmlDoc = xmlReadMemory(xml, strlen(xml), NULL, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);

    // Check for empty XML
    if (xml == NULL || strlen(xml) == 0) {
        result.err_code = XMLPARSE_ERR_EMPTY;
        result.err_msg = malloc_string(1, "Soubor byl prazdny.");
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        return result;
    }

    // Convert to XML structure
    if (xmlDoc == NULL) {
        result.err_code = XMLPARSE_ERR_OTHER;
        result.err_msg = malloc_string(1, "Nepodarilo se zpracovat XML strukturu.");
        xmlCleanupParser();
        return result;
    }

    // Get root
    xmlRoot = xmlDocGetRootElement(xmlDoc);
    if (xmlRoot == NULL) {
        result.err_code = XMLPARSE_ERR_EMPTY;
        result.err_msg = malloc_string(1, "Soubor byl prazdny.");
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        return result;
    }

    // Get feed name
    xmlNode = getNotNullXmlNode( 2 ,
        findXmlNodeByPath(xmlRoot, 2, "channel", "title") , // RSS 0.91 & RSS 1.0 & RSS 2.0
        findXmlNodeByPath(xmlRoot, 1, "title") // ATOM 1.0
    );
    if (xmlNode != NULL) {
        xmlChar *name = xmlNodeListGetString(xmlDoc, xmlNode->xmlChildrenNode, 1);
        result.name = malloc_string(1, (char *) name);
        xmlFree(name);
    }


    // Get items
    xmlNode = getNotNullXmlNode( 3 ,
        findXmlNodeByPath(xmlRoot, 2, "channel", "item") , // RSS 0.91 & RSS 2.0
        findXmlNodeByPath(xmlRoot, 1, "item") , // RSS 1.0
        findXmlNodeByPath(xmlRoot, 1, "entry") // ATOM 1.0
    );
    while (xmlNode != NULL) {
        // Alloc item
        result.num_items++;
        result.items = (struct xml_item*) realloc(result.items, sizeof(struct xml_item) * result.num_items);
        result.items[result.num_items-1] = get_xml_item();

        xmlNodePtr tempItem;

        // Get item name
        tempItem = findXmlNodeByPath(xmlNode, 1, "title"); // RSS 0.91 & RSS 1.0 & RSS 2.0 & ATOM 1.0
        if (tempItem != NULL) {
            if (xmlIsBlankNode(tempItem->xmlChildrenNode) == 0) {
                xmlChar *name = xmlNodeListGetString(xmlDoc, tempItem->xmlChildrenNode, 1);
                result.items[result.num_items-1].name = malloc_string(1, (char *) name);
                xmlFree(name);
            }
        }
        
        // Get item time
        tempItem = getNotNullXmlNode( 4 ,
            findXmlNodeByPath(xmlNode, 1, "dc:date") , // RSS 1.0
            findXmlNodeByPath(xmlNode, 1, "date") , // RSS 1.0
            findXmlNodeByPath(xmlNode, 1, "pubDate") , // RSS 2.0
            findXmlNodeByPath(xmlNode, 1, "updated") // ATOM 1.0
        );
        if (tempItem != NULL) {
            if (xmlIsBlankNode(tempItem->xmlChildrenNode) == 0) {
                xmlChar *time = xmlNodeListGetString(xmlDoc, tempItem->xmlChildrenNode, 1);
                if (time != NULL) {
                    result.items[result.num_items-1].time = malloc_string(1, (char *) time);
                    result.items[result.num_items-1].has_time = true;
                }
                xmlFree(time);
            }
        }

        // Get item author
        tempItem = getNotNullXmlNode( 3 ,
            findXmlNodeByPath(xmlNode, 1, "dc:creator") , // RSS 1.0
            findXmlNodeByPath(xmlNode, 1, "creator") , // RSS 1.0
            findXmlNodeByPath(xmlNode, 1, "author") // RSS 2.0 & ATOM 1.0
        );
        if (tempItem != NULL) {
            xmlNodePtr tempItemItem = getNotNullXmlNode( 2 ,
                findXmlNodeByPath(tempItem, 1, "name") , // ATOM 1.0
                findXmlNodeByPath(tempItem, 1, "email") // ATOM 1.0
            );
            if (tempItemItem != NULL) {
                tempItem = tempItemItem;
            }
            if (xmlIsBlankNode(tempItem->xmlChildrenNode) == 0) {
                xmlChar *author = xmlNodeListGetString(xmlDoc, tempItem->xmlChildrenNode, 1);
                if (author != NULL) {
                    result.items[result.num_items-1].author = malloc_string(1, (char *) author);
                    result.items[result.num_items-1].has_author = true;
                }
                xmlFree(author);
            }
        }

        // Get item URL
        tempItem = getNotNullXmlNode( 2 ,
            findXmlNodeByPath(xmlNode, 1, "link") , // RSS 0.91 & RSS 1.0 & RSS 2.0 & ATOM 1.0
            findXmlNodeByPath(xmlNode, 1, "atom:link") // ATOM 1.0
        );
        if (tempItem != NULL) {
            xmlChar *url = xmlGetProp(tempItem, (const xmlChar *) "href"); // ATOM 1.0
            if (url == NULL) {
                if (xmlIsBlankNode(tempItem->xmlChildrenNode) == 0) {
                    url = xmlNodeListGetString(xmlDoc, tempItem->xmlChildrenNode, 1);
                }
            }
            if (url != NULL) {
                result.items[result.num_items-1].url = malloc_string(1, (char *) url);
                result.items[result.num_items-1].has_url = true;
                xmlFree(url);
            }
        } else {
            tempItem = findXmlNodeByPath(xmlNode, 1, "guid"); // RSS 2.0
            if (tempItem != NULL) {
                xmlChar *url = xmlGetProp(tempItem, (const xmlChar *) "isPermaLink");
                if (url != NULL && !xmlStrcmp(url, (const xmlChar *) "true")) {
                    xmlFree(url);
                    if (xmlIsBlankNode(tempItem->xmlChildrenNode) == 0) {
                        url = xmlNodeListGetString(xmlDoc, tempItem->xmlChildrenNode, 1);
                        if (url != NULL) {
                            result.items[result.num_items-1].url = malloc_string(1, (char *) url);
                            result.items[result.num_items-1].has_url = true;
                        }
                        xmlFree(url);
                    }
                }
            }
        }

        // Get next item
        xmlNode = getNotNullXmlNode( 2 ,
            findNextXmlNodeOnSameLevel(xmlNode, "item") , // RSS 0.91 & RSS 1.0 & RSS 2.0
            findNextXmlNodeOnSameLevel(xmlNode, "entry") // ATOM 1.0
        );
    }

    // Clean
    xmlFreeDoc(xmlDoc);
    xmlCleanupParser();
    return result;
}

struct xml_item get_xml_item() {
    struct xml_item result;
    result.name = NULL;
    result.has_time = false;
    result.time = NULL;
    result.has_author = false;
    result.author = NULL;
    result.has_url = false;
    result.url = NULL;
    return result;
}

void print_xml_item(struct xml_item *item) {
    printf("=== XML_ITEM BEGIN ===\n");
    printf("name: %s\n", item->name == NULL ? "NULL" : item->name);
    printf("has_time: %s\n", item->has_time ? "true" : "false");
    printf("time: %s\n", item->time == NULL ? "NULL" : item->time);
    printf("has_author: %s\n", item->has_author ? "true" : "false");
    printf("author: %s\n", item->author == NULL ? "NULL" : item->author);
    printf("has_url: %s\n", item->has_url ? "true" : "false");
    printf("url: %s\n", item->url == NULL ? "NULL" : item->url);
    printf("=== XML_ITEM END ===\n");
}

void free_xml_item(struct xml_item *item) {
    if (item->name != NULL) {
        free(item->name);
        item->name = NULL;
    }
    if (item->has_time) {
        free(item->time);
        item->time = NULL;
        item->has_time = false;
    }
    if (item->has_author) {
        free(item->author);
        item->author = NULL;
        item->has_author = false;
    }
    if (item->has_url) {
        free(item->url);
        item->url = NULL;
        item->has_url = false;
    }
}

struct xml_content get_xml_content() {
    struct xml_content result;
    result.name = NULL;
    result.num_items = 0;
    result.items = NULL;
    result.err_code = XMLPARSE_ERR_OK;
    result.err_msg = NULL;
    return result;
}

void print_xml_content(struct xml_content *content) {
    printf("=== XML_CONTENT BEGIN ===\n");
    printf("name: %s\n", content->name == NULL ? "NULL" : content->name);
    printf("num_items: %d\n", content->num_items);
    printf("items: %s\n", content->items == NULL ? "NULL" : "");
    for (int i = 0; i < content->num_items; i++) {
        printf("  item #%d\n", i);
        print_xml_item(&content->items[i]);
    }
    printf("err_code: %d\n", content->err_code);
    printf("err_msg: %s\n", content->err_msg == NULL ? "NULL" : content->err_msg);
    printf("=== XML_CONTENT END ===\n");
}

void free_xml_content(struct xml_content *content) {
    if (content->name != NULL) {
        free(content->name);
        content->name = NULL;
    }
    if (content->num_items > 0) {
        for (int i = 0; i < content->num_items; i++) {
            free_xml_item(&content->items[i]);
        }
        free(content->items);
        content->items = NULL;
        content->num_items = 0;
    }
    if (content->err_code != XMLPARSE_ERR_OK) {
        free(content->err_msg);
        content->err_msg = NULL;
    }
}
