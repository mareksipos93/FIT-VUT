/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

test_feedfile_parser.c - testování modulu pro zpracování vstupního souboru se zdroji pro feedy

**/

#include "test_suite.h"

#include "../feedfile_parser.h"

void test301() {
	struct ff_content c = get_ff_content();

	assert_is_equal("zero feeds", c.num_feeds, 0);
	assert_is_null("feeds not allocated", c.feeds);
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test302() {
	char *str = "";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("zero feeds", c.num_feeds, 0);
	assert_is_null("feeds not allocated", c.feeds);
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test303() {
	char *str = "\n";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("zero feeds", c.num_feeds, 0);
	assert_is_null("feeds not allocated", c.feeds);
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test304() {
	char *str = "http://www.url.com/";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("one feed", c.num_feeds, 1);
	if (assert_not_null("feeds are allocated", c.feeds)) {
		if (assert_not_null("feeds[0] allocated", c.feeds[0]))
			assert_string_equal("feeds[0] properly saved", c.feeds[0], "http://www.url.com/");
	}
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test305() {
	char *str = "    http://www.url.com/    ";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("one feed", c.num_feeds, 1);
	if (assert_not_null("feeds are allocated", c.feeds)) {
		if (assert_not_null("feeds[0] allocated", c.feeds[0]))
			assert_string_equal("feeds[0] properly saved", c.feeds[0], "http://www.url.com/");
	}
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test306() {
	char *str = "#";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("zero feeds", c.num_feeds, 0);
	assert_is_null("feeds not allocated", c.feeds);
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test307() {
	char *str = "#simple comment";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("zero feeds", c.num_feeds, 0);
	assert_is_null("feeds not allocated", c.feeds);
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test308() {
	char *str = "    #simple comment    ";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("zero feeds", c.num_feeds, 0);
	assert_is_null("feeds not allocated", c.feeds);
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test309() {
	char *str = "  http://www.someurl.com/text?var=val&var2=val2  #comment http://www.donotparseme.com";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("one feed", c.num_feeds, 1);
	if (assert_not_null("feeds are allocated", c.feeds)) {
		if (assert_not_null("feeds[0] allocated", c.feeds[0]))
			assert_string_equal("feeds[0] properly saved", c.feeds[0], "http://www.someurl.com/text?var=val&var2=val2");
	}
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test310() {
	char *str = "  http://www.someurl.com/text?var=val&var2=val2#notacomment http://www.anotherurl.com";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("two feeds", c.num_feeds, 2);
	if (assert_not_null("feeds are allocated", c.feeds)) {
		if (assert_not_null("feeds[0] allocated", c.feeds[0]))
			assert_string_equal("feeds[0] properly saved", c.feeds[0], "http://www.someurl.com/text?var=val&var2=val2#notacomment");
		if (assert_not_null("feeds[1] allocated", c.feeds[1]))
			assert_string_equal("feeds[1] properly saved", c.feeds[1], "http://www.anotherurl.com");
	}
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test311() {
	char *str = "\t\t     something #comment\n"
				" SOMETHING \telse #comment something \n"
				" #only comment and nothing else\n"
				"  //unsupported_comment\n\n"
				"#\n"
				"w";
	struct ff_content c = parse_feedstring(str);

	assert_is_equal("five feeds", c.num_feeds, 5);
	if (assert_not_null("feeds are allocated", c.feeds)) {
		if (assert_not_null("feeds[0] allocated", c.feeds[0]))
			assert_string_equal("feeds[0] properly saved", c.feeds[0], "something");
		if (assert_not_null("feeds[1] allocated", c.feeds[1]))
			assert_string_equal("feeds[1] properly saved", c.feeds[1], "SOMETHING");
		if (assert_not_null("feeds[2] allocated", c.feeds[2]))
			assert_string_equal("feeds[2] properly saved", c.feeds[2], "else");
		if (assert_not_null("feeds[3] allocated", c.feeds[3]))
			assert_string_equal("feeds[3] properly saved", c.feeds[3], "//unsupported_comment");
		if (assert_not_null("feeds[4] allocated", c.feeds[4]))
			assert_string_equal("feeds[4] properly saved", c.feeds[4], "w");
	}
	assert_is_equal("error code is zero (OK)", c.err_code, 0);

	free_ff_content(&c);
}

void test_feedfile_parser() {

	run_test("get_ff_content()", &test301);
	run_test("feedstring - empty", &test302);
	run_test("feedstring - '\\n'", &test303);
	run_test("feedstring - simple URL", &test304);
	run_test("feedstring - simple URL (spaces)", &test305);
	run_test("feedstring - '#'", &test306);
	run_test("feedstring - simple comment", &test307);
	run_test("feedstring - simple comment (spaces)", &test308);
	run_test("feedstring - URL with comment (separated)", &test309);
	run_test("feedstring - URL with comment (not separated)", &test310);
	run_test("feedstring - complex", &test311);

}
