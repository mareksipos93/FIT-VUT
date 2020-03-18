# 
# ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
# Autor: Marek Šipoš (xsipos03), 26.9. 2018
# 
# Makefile - soubor s direktivami pro kompilaci, spuštění, archivaci a úklid
# 

FLAGS = -Wall -pedantic -std=c99
TESTS_DIR = tests/
# always put tests in child dir

all: xml_parser.o http_downloader.o feedfile_parser.o param_parser.o url_parser.o util.o feedreader

#############################

xml_parser.o: xml_parser.c
	gcc $(FLAGS) -c xml_parser.c -o xml_parser.o `xml2-config --cflags` `xml2-config --libs`

http_downloader.o: http_downloader.c
	gcc $(FLAGS) -c http_downloader.c -o http_downloader.o

feedfile_parser.o: feedfile_parser.c
	gcc $(FLAGS) -c feedfile_parser.c -o feedfile_parser.o

param_parser.o: param_parser.c
	gcc $(FLAGS) -c param_parser.c -o param_parser.o

url_parser.o: url_parser.c
	gcc $(FLAGS) -c url_parser.c -o url_parser.o

util.o: util.c
	gcc $(FLAGS) -c util.c -o util.o

feedreader: xml_parser.h xml_parser.o http_downloader.h http_downloader.o feedfile_parser.h feedfile_parser.o param_parser.h param_parser.o url_parser.h url_parser.o util.h util.o feedreader.c
	gcc $(FLAGS) xml_parser.o http_downloader.o feedfile_parser.o param_parser.o url_parser.o util.o feedreader.c -lssl -lcrypto -lxml2 -o feedreader

#############################

$(TESTS_DIR)test_suite.o: $(TESTS_DIR)test_suite.c
	gcc $(FLAGS) -c $(TESTS_DIR)test_suite.c -o $(TESTS_DIR)test_suite.o

$(TESTS_DIR)test_feedreader: feedreader $(TESTS_DIR)test_suite.h $(TESTS_DIR)test_suite.o $(TESTS_DIR)test_util.c $(TESTS_DIR)test_param_parser.c $(TESTS_DIR)test_feedfile_parser.c $(TESTS_DIR)test_url_parser.c $(TESTS_DIR)test_http_downloader.c $(TESTS_DIR)test_xml_parser.c $(TESTS_DIR)test_feedreader.c
	gcc $(FLAGS) xml_parser.o http_downloader.o feedfile_parser.o param_parser.o url_parser.o util.o $(TESTS_DIR)test_suite.o $(TESTS_DIR)test_util.c $(TESTS_DIR)test_param_parser.c $(TESTS_DIR)test_feedfile_parser.c $(TESTS_DIR)test_url_parser.c $(TESTS_DIR)test_http_downloader.c $(TESTS_DIR)test_xml_parser.c $(TESTS_DIR)test_feedreader.c -lssl -lcrypto -lxml2 -o $(TESTS_DIR)test_feedreader

test: $(TESTS_DIR)test_feedreader runtest

#############################

.PHONY: clean runtest cleantest tar

fullclean: clean distclean

clean: cleantest
	rm -f *.o
	rm -f *.core

runtest:
	./$(TESTS_DIR)test_feedreader

cleantest:
	rm -f $(TESTS_DIR)*.o
	rm -f $(TESTS_DIR)*.core

distclean:
	rm -f feedreader
	rm -f $(TESTS_DIR)*test_feedreader

tar: clean
	tar -cvf xsipos03.tar . --exclude=test.txt --exclude=feedreader --exclude=$(TESTS_DIR)test_feedreader --exclude=xsipos03.tar
