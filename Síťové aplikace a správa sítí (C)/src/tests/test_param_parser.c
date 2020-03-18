/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

test_param_parser.c - testování modulu pro zpracování argumentů příkazové řádky

**/

#include "test_suite.h"

#include "../param_parser.h"
#include <stdlib.h>

void test201() {
	struct par_content c = get_par_content();

	assert_false("is not help", c.is_help);
	assert_false("is not URL", c.is_url);
	assert_is_null("URL is NULL", c.url);
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("no error code", c.err_code, PAR_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_par_content(&c);
}

void test202() {
	int argc = 1;
	char *argv[] = {"test_param_parser.c", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_false("is not URL", c.is_url);
	assert_is_null("URL is NULL", c.url);
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("error code is 'No URL provided'", c.err_code, PAR_ERR_SOURCE_MISSING);
	assert_not_null("error msg is allocated", c.err_msg);

	free_par_content(&c);
}

void test203() {
	int argc = 2;
	char *argv[] = {"test_param_parser.c", "-h", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_true("is help", c.is_help);
	assert_false("is not URL", c.is_url);
	assert_is_null("URL is NULL", c.url);
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);

	free_par_content(&c);
}

void test204() {
	int argc = 2;
	char *argv[] = {"test_param_parser.c", "http://www.url.com", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_true("is URL", c.is_url);
	assert_string_equal("URL properly saved", c.url, "http://www.url.com");
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("no error code", c.err_code, PAR_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_par_content(&c);
}

void test205() {
	int argc = 3;
	char *argv[] = {"test_param_parser.c", "-f", "feedfile.txt", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_false("is not URL", c.is_url);
	assert_is_null("URL is NULL", c.url);
	assert_true("is feedfile", c.is_feedfile);
	assert_string_equal("feedfile properly saved", c.feedfile, "feedfile.txt");
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("no error code", c.err_code, PAR_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_par_content(&c);
}

void test206() {
	int argc = 6;
	char *argv[] = {"test_param_parser.c", "http://www.url.com", "-c", "certfile1", "-c", "certfile2", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_true("is URL", c.is_url);
	assert_string_equal("URL properly saved", c.url, "http://www.url.com");
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("two certfiles", c.num_certfile, 2);
	assert_string_equal("certfile[0] properly saved", c.certfile[0], "certfile1");
	assert_string_equal("certfile[1] properly saved", c.certfile[1], "certfile2");
	assert_is_null("certfile[2] is NULL", c.certfile[2]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("no error code", c.err_code, PAR_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_par_content(&c);
}

void test207() {
	int argc = 6;
	char *argv[] = {"test_param_parser.c", "http://www.url.com", "-C", "certaddr1", "-C", "certaddr2", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_true("is URL", c.is_url);
	assert_string_equal("URL properly saved", c.url, "http://www.url.com");
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("two certaddr", c.num_certaddr, 2);
	assert_string_equal("certaddr[0] properly saved", c.certaddr[0], "certaddr1");
	assert_string_equal("certaddr[1] properly saved", c.certaddr[1], "certaddr2");
	assert_is_null("certaddr[2] is NULL", c.certaddr[2]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("no error code", c.err_code, PAR_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_par_content(&c);
}

void test208() {
	int argc = 6;
	char *argv[] = {"test_param_parser.c", "http://www.url.com", "-c", "certfile", "-C", "certaddr", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_true("is URL", c.is_url);
	assert_string_equal("URL properly saved", c.url, "http://www.url.com");
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("one certfile", c.num_certfile, 1);
	assert_string_equal("certfile[0] properly saved", c.certfile[0], "certfile");
	assert_is_null("certfile[1] is NULL", c.certfile[1]);
	assert_is_equal("one certaddr", c.num_certaddr, 1);
	assert_string_equal("certaddr[0] properly saved", c.certaddr[0], "certaddr");
	assert_is_null("certaddr[1] is NULL", c.certaddr[1]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("no error code", c.err_code, PAR_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_par_content(&c);
}

void test209() {
	int argc = 4;
	char *argv[] = {"test_param_parser.c", "-f", "feedfile.txt", "http://www.url.com", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("error code is 'Multiple sources'", c.err_code, PAR_ERR_SOURCE_MULTIPLE);
	assert_not_null("error msg is allocated", c.err_msg);

	free_par_content(&c);
}

void test210() {
	int argc = 3;
	char *argv[] = {"test_param_parser.c", "http://www.url1.com", "http://www.url2.com", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("error code is 'Multiple sources'", c.err_code, PAR_ERR_SOURCE_MULTIPLE);
	assert_not_null("error msg is allocated", c.err_msg);

	free_par_content(&c);
}

void test211() {
	int argc = 5;
	char *argv[] = {"test_param_parser.c", "-f", "feedfile1.txt", "-f", "feedfile2.txt", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_false("is not URL", c.is_url);
	assert_is_null("URL is NULL", c.url);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("error code is 'Multiple sources'", c.err_code, PAR_ERR_SOURCE_MULTIPLE);
	assert_not_null("error msg is allocated", c.err_msg);

	free_par_content(&c);
}

void test212() {
	int argc = 6;
	char *argv[] = {"test_param_parser.c", "-T", "-f", "feedfile.txt", "-u", "-a", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_false("is not URL", c.is_url);
	assert_is_null("URL is NULL", c.url);
	assert_true("is feedfile", c.is_feedfile);
	assert_string_equal("feedfile properly saved", c.feedfile, "feedfile.txt");
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_true("is '-T'", c.is_t);
	assert_true("is '-a'", c.is_a);
	assert_true("is '-u'", c.is_u);
	assert_is_equal("no error code", c.err_code, PAR_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_par_content(&c);
}

void test213() {
	int argc = 2;
	char *argv[] = {"test_param_parser.c", "-H", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("error code is 'Unknown parameter'", c.err_code, PAR_ERR_ARGUMENT_UNKNOWN);
	assert_not_null("error msg is allocated", c.err_msg);

	free_par_content(&c);
}

void test214() {
	int argc = 2;
	char *argv[] = {"test_param_parser.c", "-f", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_false("is not URL", c.is_url);
	assert_is_null("URL is NULL", c.url);
	assert_is_equal("zero certfiles", c.num_certfile, 0);
	assert_is_null("certfile[0] is NULL", c.certfile[0]);
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("error code is 'Missing param value'", c.err_code, PAR_ERR_ARGUMENT_NOVALUE);
	assert_not_null("error msg is allocated", c.err_msg);

	free_par_content(&c);
}

void test215() {
	int argc = 3;
	char *argv[] = {"test_param_parser.c", "-cfile", "http://www.url.com", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_true("is URL", c.is_url);
	assert_string_equal("URL properly saved", c.url, "http://www.url.com");
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_is_equal("one certfile", c.num_certfile, 1);
	assert_string_equal("certfile[0] properly saved", c.certfile[0], "file");
	assert_is_equal("zero certaddr", c.num_certaddr, 0);
	assert_is_null("certaddr[0] is NULL", c.certaddr[0]);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("no error code", c.err_code, PAR_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_par_content(&c);
}

void test216() {
	int argc = 4;
	char *argv[] = {"test_param_parser.c", "-c", "-C", "http://www.url.com", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_false("is not feedfile", c.is_feedfile);
	assert_is_null("feedfile is NULL", c.feedfile);
	assert_false("is not '-T'", c.is_t);
	assert_false("is not '-a'", c.is_a);
	assert_false("is not '-u'", c.is_u);
	assert_is_equal("error code is 'Missing param value'", c.err_code, PAR_ERR_ARGUMENT_NOVALUE);
	assert_not_null("error msg is allocated", c.err_msg);

	free_par_content(&c);
}

void test217() {
	int argc = 10;
	char *argv[] = {"test_param_parser.c", "-T", "-a", "-u", "-f", "feedfile.txt", "-c", "certfile", "-C", "certaddr", (char *)NULL};
	struct par_content c = parse_parameters(argc, argv);

	assert_false("is not help", c.is_help);
	assert_false("is not URL", c.is_url);
	assert_is_null("URL is NULL", c.url);
	assert_true("is feedfile", c.is_feedfile);
	assert_string_equal("feedfile properly saved", c.feedfile, "feedfile.txt");
	assert_is_equal("one certfile", c.num_certfile, 1);
	assert_string_equal("certfile[0] properly saved", c.certfile[0], "certfile");
	assert_is_null("certfile[1] is NULL", c.certfile[1]);
	assert_is_equal("one certaddr", c.num_certaddr, 1);
	assert_string_equal("certaddr[0] properly saved", c.certaddr[0], "certaddr");
	assert_is_null("certaddr[1] is NULL", c.certaddr[1]);
	assert_true("is '-T'", c.is_t);
	assert_true("is '-a'", c.is_a);
	assert_true("is '-u'", c.is_u);
	assert_is_equal("no error code", c.err_code, PAR_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_par_content(&c);
}

void test_param_parser() {
	run_test("get_par_content()", &test201);
	run_test("No parameters (no source)", &test202);
	run_test("Just help", &test203);
	run_test("Basic URL", &test204);
	run_test("Feedfile", &test205);
	run_test("URL with two certfiles", &test206);
	run_test("URL with two certaddr", &test207);
	run_test("URL with certfile and certaddr", &test208);
	run_test("URL and feedfile", &test209);
	run_test("Two URLs", &test210);
	run_test("Two feedfiles", &test211);
	run_test("Feedfile with -T -a -u", &test212);
	run_test("URL with unknown parameter -H", &test213);
	run_test("Missing feedfile value", &test214);
	run_test("URL with certfile like \"-cfile\"", &test215);
	run_test("URL with -c followed by -C", &test216);
	run_test("Feedfile with certfile, certaddr and -T -a -u", &test217);
}
