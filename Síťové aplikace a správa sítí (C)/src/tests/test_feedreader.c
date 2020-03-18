/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

test_feedreader.c - testování aplikace Feedreader

**/

#include "test_suite.h"

#include <stdio.h>

/* TO ADD NEW MODULE
	1. Create its file with proper function
	2. Add file properly into Makefile
	3. Add function declaration there
	4. Call it in main() with test_module()
*/

void test_util();
void test_param_parser();
void test_feedfile_parser();
void test_url_parser();
void test_http_downloader();
void test_xml_parser();

int main(int argc, char *argv[]) {
	if (!begin_test_session()) {
		printf("Testy se nepodarilo spustit.\n");
		return 1;
	}

	test_module("Utilities", &test_util);
	test_module("Parameter parser", &test_param_parser);
	test_module("Feedfile parser", &test_feedfile_parser);
	test_module("URL parser", &test_url_parser);
	test_module("HTTP downloader", &test_http_downloader);
	test_module("XML parser", &test_xml_parser);

	if (!end_test_session()) {
		printf("Vysledky testovani nejsou k dispozici.\n");
		return 1;
	}

	return 0;
}
