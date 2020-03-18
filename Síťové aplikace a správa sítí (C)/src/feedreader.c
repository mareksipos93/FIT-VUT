/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

feedreader.c - jádro aplikace

**/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "param_parser.h"
#include "feedfile_parser.h"
#include "url_parser.h"
#include "util.h"
#include "http_downloader.h"
#include "xml_parser.h"

/******** MAIN RETURN VALUES ********/

#define STATUS_OK       0 // Execution OK
#define STATUS_PARAMS   1 // Parameters error
#define STATUS_FEEDFILE 2 // Feedfile error

struct par_content params;
struct ff_content feeds;
struct url_content parsed_url;
struct dw_content download;
struct xml_content xml;

void end_program(int status) {

	print_debug("MAIN - Uvolnovani a konec programu\n");
    free_ff_content(&feeds);
    free_par_content(&params);
    free_url_content(&parsed_url);
    free_dw_content(&download);
    free_xml_content(&xml);
    free_certs();

    exit(status);
}

void print_help() {
    printf(
        "--- CTECKA FEEDU RSS A ATOM ---\n"
        "--- Autor: MAREK SIPOS (xsipos03) ---\n"
        "\n"
        "Pouziti: feedreader <URL | -f <feedfile>> [-c <certfile>] [-C <certaddr>] [-T] [-a] [-u] [-h]\n"
        "\n"
        "URL | -f <feedfile>\n"
        "Povinny parametr, slouzi k urceni adresy URL jednoho feedu nebo souboru s adresami URL pro vice feedu.\n"
        "Soubor feedfile umoznuje psani komentaru uvozenych znakem # az do konce daneho radku.\n"
        "Je mozne zadat najednou bud jednu adresu URL, nebo jeden soubor s feedy.\n"
        "\n"
        "-c <certfile>\n"
        "Volitelny parametr, slouzi k zadani souboru s duveryhodnymi certifikaty pro zabezpecenou komunikaci (HTTPS).\n"
        "Opakovanym pouzivanim parametru je mozne zadat az " STR(MAX_CERTFILES) " certifikatu.\n"
        "Pokud nejsou parametry -c a -C pouzity ani jednou, k vyhledani certifikatu se pouzije vychozi uloziste v systemu.\n"
        "\n"
        "-C <certaddr>\n"
        "Volitelny parametr, slouzi k zadani slozky s duveryhodnymi certifikaty pro zabezpecenou komunikaci (HTTPS).\n"
        "Opakovanym pouzivanim parametru je mozne zadat az " STR(MAX_CERTADDRESSES) " slozek.\n"
        "Pokud nejsou parametry -c a -C pouzity ani jednou, k vyhledani certifikatu se pouzije vychozi uloziste v systemu.\n"
        "\n"
        "-T\n"
        "Volitelny parametr, ve vysledcich dojde k zahrnuti casu zmeny ci vytvoreni zaznamu.\n"
        "\n"
        "-a\n"
        "Volitelny parametr, ve vysledcich dojde k zahrnuti identifikacnich udaju autora zaznamu.\n"
        "\n"
        "-u\n"
        "Volitelny parametr, ve vysledcich dojde k zahrnuti asociovane adresy URL zaznamu.\n"
        "\n"
        "-h\n"
        "Volitelny parametr, pokud je pri volani programu zadan samostatne, dojde k vyvolani teto napovedy.\n"
        "------------------------\n"
    );
}

int main(int argc, char *argv[])
{
	print_debug("MAIN - Inicializace\n");
    params = get_par_content();
    feeds = get_ff_content();
    parsed_url = get_url_content();
    download = get_dw_content();
    xml = get_xml_content();

    /******** PARSE PARAMS ********/

    print_debug("MAIN - Zpracovani parametru\n");
    params = parse_parameters(argc, argv);

    /* DEBUG */ //print_par_content(&params); /* DEBUG */

    if (argc == 2 && params.is_help) {
    	print_debug("MAIN - Tisk napovedy\n");
        print_help();
        end_program(STATUS_OK);
    }

    if (params.err_code != PAR_ERR_OK) {
        fprintf(stderr, params.err_msg);
        fprintf(stderr, "Pro napovedu spustte program znovu pouze s parametrem -h\n");
        end_program(STATUS_PARAMS);
    }

    /******** PARSE FEEDFILE ********/

    if (params.is_feedfile) {
    	print_debug("MAIN - Zpracovani feedfile\n");
        feeds = parse_feedfile(params.feedfile);

        /* DEBUG */ //print_ff_content(&feeds); /* DEBUG */

        if (feeds.err_code != 0) {
            fprintf(stderr, "Chyba pri zpracovani souboru se zdroji: %s\n", strerror(feeds.err_code));
            end_program(STATUS_FEEDFILE);
        }

        if (feeds.num_feeds == 0) {
            fprintf(stderr, "Soubor se zdroji neobsahoval zadny platny zdroj.\n");
            end_program(STATUS_FEEDFILE);
        }
    }

    
    /******** PASS CERTIFICATES ********/

    if (params.num_certfile > 0 || params.num_certaddr > 0) {
    	print_debug("MAIN - Predavani certifikatu\n");
    }

    for (int i = 0; i < params.num_certfile; i++) {
        provide_certfile(params.certfile[i]);
    }
        for (int i = 0; i < params.num_certaddr; i++) {
        provide_certaddr(params.certaddr[i]);
    }

    /******** PROCESS URLS ********/

    print_debug("MAIN - Zacina zpracovani vsech URL\n");
    for (int i = 0; i < (params.is_url ? 1 : feeds.num_feeds); i++) {

        // Parse URL (in Main it is used only for some outputs...)
        print_debug("MAIN - Zpracovani URL '%s'\n", params.is_url ? params.url : feeds.feeds[i]);
        parsed_url = parse_url(params.is_url ? params.url : feeds.feeds[i]);
        if (parsed_url.err_code != URL_ERR_OK) {
            fprintf(stderr, "Chyba pri zpracovani URL: %s\n", parsed_url.err_msg);
            free_url_content(&parsed_url);
            continue;
        }

        // Download
        print_debug("MAIN - Stahovani z URL '%s'\n", params.is_url ? params.url : feeds.feeds[i]);
        download = try_download(params.is_url ? params.url : feeds.feeds[i]);

        /* DEBUG */ //print_dw_content(&download); /* DEBUG */

        if (download.err_code != DW_ERR_OK) {
            fprintf(stderr, "Chyba pri stahovani ze serveru '%s': %s\n", parsed_url.host, download.err_msg);
            free_dw_content(&download);
            free_url_content(&parsed_url);
            continue;
        }

        // Skip HTTP header
        print_debug("MAIN - Skipping HTTP header\n");
        bool skip_failed = false;   // If error occurs, it is used to -continue- in outer loop (-for- with i)
        int skip = 0;               // How many chars to skip (aka size of header)
        for (int j = 0; j < strlen(download.content)+1; j++) {
            if (download.content[j] == '\0') {
                skip_failed = true;
            } else if (download.content[j] == '\n' && download.content[j+1] != '\0' && download.content[j+1] == '\n') {
                skip = j+2; // +1 is because j started with 0 and +1 is because we checked for \n\n (one char ahead)
                break;
            } else if (download.content[j] == '\r' && download.content[j+1] != '\0' && download.content[j+1] == '\n' &&
                download.content[j+2] != '\0' && download.content[j+2] == '\r' &&
                download.content[j+3] != '\0' && download.content[j+3] == '\n') {
                skip = j+4; // +1 is because j started with 0 and +3 is because we checked for \r\n\r\n (three chars ahead)
                break;
            }
        }
        if (skip_failed) {
            fprintf(stderr, "Chyba pri zpracovani souboru ze serveru '%s': Nepodarilo se preskocit HTTP hlavicku.\n", parsed_url.host);
            free_dw_content(&download);
            free_url_content(&parsed_url);
            continue;
        }

        // Parse XML
        print_debug("MAIN - Parsing XML\n");
        xml = parse_xml(download.content+skip);

        /* DEBUG */ //print_xml_content(&xml); /* DEBUG */

        if (xml.err_code != XMLPARSE_ERR_OK) {
            fprintf(stderr, "Chyba pri zpracovani souboru ze serveru '%s': %s\n", parsed_url.host, xml.err_msg);
            free_xml_content(&xml);
            free_dw_content(&download);
            free_url_content(&parsed_url);
            continue;
        }

        // Print results
        print_debug("MAIN - Tisk vysledku\n");
        printf("*** %s ***\n", xml.name == NULL ? "<Feed bez nazvu>" : xml.name);
        for (int j = 0; j < xml.num_items; j++) {
            printf("%s\n", xml.items[j].name == NULL ? "<Zaznam bez nazvu>" : xml.items[j].name);
            if (params.is_t && xml.items[j].has_time) {
                printf("Aktualizace: %s\n", xml.items[j].time == NULL ? "<Datum neurceno>" : xml.items[j].time);
            }
            if (params.is_a && xml.items[j].has_author) {
                printf("Autor: %s\n", xml.items[j].author == NULL ? "<Autor neurcen>" : xml.items[j].author);
            }
            if (params.is_u && xml.items[j].has_url) {
                printf("URL: %s\n", xml.items[j].url == NULL ? "<URL neurceno>" : xml.items[j].url);
            }
            if (params.is_t || params.is_a || params.is_u) {
                printf("\n");
            }
        }
        printf("\n");

        // ??? - poradi atributu kdyz pouziju -T -a -u ???

        // ??? - vlakna pro soubezne stahovani ???

        free_xml_content(&xml);
        free_dw_content(&download);
        free_url_content(&parsed_url);
    }
    
    /******** END ********/

    end_program(STATUS_OK);
}
