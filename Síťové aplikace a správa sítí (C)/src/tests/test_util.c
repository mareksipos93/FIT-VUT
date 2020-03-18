/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

test_util.c - testování modulu obsahujícího pomocné funkce

**/

#include "test_suite.h"

#include <stdlib.h>
#include "../util.h"

void test101() {
	#define A 1
	#define B 2

	assert_string_equal("\"1 + 2\" using values", STR(1) " + " STR(2), "1 + 2");
	assert_string_equal("\"1 + 2\" using macros", STR(A) " + " STR(B), "1 + 2");
	assert_string_equal("STR(text) should be \"text\"", STR(text), "text");
}

void test102() {
	assert_is_bigger("MAX_CERTFILES > 0", MAX_CERTFILES, 0);
	assert_is_bigger("MAX_CERTADDRESSES > 0", MAX_CERTADDRESSES, 0);
	assert_is_bigger("FEEDFILE_BUFFERSIZE > 1", FEEDFILE_BUFFERSIZE, 1);
}

void test103() {
	char *text1;

	text1 = malloc_string(1, "simple text");

	if (assert_not_null("Direct string - not NULL", text1))
		if (assert_is_equal("Direct string - zero terminated", text1[11], '\0'))
			if (assert_string_length("Direct string - length", text1, 11))
				assert_string_equal("Direct string - content", text1, "simple text");


	char *text2;
	text2 = malloc_string(1, text1);
	free(text1);

	if (assert_not_null("Indirect string - not NULL", text2))
		if (assert_is_equal("Indirect string - zero terminated", text2[11], '\0'))
			if (assert_string_length("Indirect string - length", text2, 11))
				assert_string_equal("Indirect string - content", text2, "simple text");

	free(text2);
}

void test104() {
	char *text;

	text = malloc_string(1, NULL);

	assert_is_null("One NULL param", text);
	free(text);

	text = malloc_string(2, NULL, NULL);

	assert_is_null("Two NULL params", text);
	free(text);

	text = malloc_string(3, NULL, "text", NULL);

	if (assert_not_null("NULL, \"text\", NULL - not NULL", text))
		if (assert_is_equal("NULL, \"text\", NULL - zero terminated", text[4], '\0'))
			if (assert_string_length("NULL, \"text\", NULL - length", text, 4))
				assert_string_equal("NULL, \"text\", NULL - content", text, "text");
	free(text);
}

void test105() {
	char *text1;
	char *text2;

	text1 = malloc_string(2, "aaa", "bbb");

	if (assert_not_null("simple concat - not NULL", text1))
		if (assert_is_equal("simple concat - zero terminated", text1[6], '\0'))
			if (assert_string_length("simple concat - length", text1, 6))
				assert_string_equal("simple concat - content", text1, "aaabbb");
	free(text1);

	text1 = malloc_string(4, "aaa", NULL, "bb", "b");

	if (assert_not_null("three concats with NULL - not NULL", text1))
		if (assert_is_equal("three concats with NULL - zero terminated", text1[6], '\0'))
			if (assert_string_length("three concats with NULL - length", text1, 6))
				assert_string_equal("three concats with NULL - content", text1, "aaabbb");

	text2 = malloc_string(14, "a", "b", "c", "d", "e", NULL, "\n", "abcde", NULL, "\t\n", NULL, NULL, text1, text1);
	if (assert_not_null("crazy text - not NULL", text2))
		if (assert_is_equal("crazy text - zero terminated", text2[25], '\0'))
			if (assert_string_length("crazy text - length", text2, 25))
				assert_string_equal("crazy text - content", text2, "abcde\nabcde\t\naaabbbaaabbb");
	free(text1);
	free(text2);
}

void test_util() {

	run_test("Macro STR(x) (val to str)", &test101);
	run_test("Options min values", &test102);
	run_test("malloc_string() simple", &test103);
	run_test("malloc_string() with NULLs", &test104);
	run_test("malloc_string() concatenations", &test105);

}
