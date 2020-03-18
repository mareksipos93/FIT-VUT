/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

test_suite.h - modul pro testování v jazyce C

**/

#ifndef TEST_SUITE_H_
#define TEST_SUITE_H_

#include <stdbool.h>

#define BLA   "\x1B[30m"
#define RED   "\x1B[31m"
#define GRE   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define PUR   "\x1B[35m"
#define CYA   "\x1B[36m"
#define WHI   "\x1B[37m"
#define NOR   "\x1B[0m"

typedef void (*funcPtr)(void);

// Init tests and print test message, returns 'false' if test couldnt be launched
bool begin_test_session();
// End tests and show results, returns 'false' if no results were available
bool end_test_session();

// Test one module within session, returns whether module was successful
bool test_module(char *name, funcPtr f);

// Run one test within module, returns whether test was successful
bool run_test(char *name, funcPtr f);

bool assert_false(char *desc, bool b);
bool assert_true(char *desc, bool b);
bool assert_is_null(char *desc, void *ptr);
bool assert_not_null(char *desc, void *ptr);
bool assert_is_equal(char *desc, int n1, int n2);
bool assert_not_equal(char *desc, int n1, int n2);
bool assert_string_equal(char *desc, char *s1, char *s2);
bool assert_string_length(char *desc, char *s, int l);
bool assert_is_smaller(char *desc, int n1, int n2);
bool assert_is_bigger(char *desc, int n1, int n2);
bool assert_custom(char *desc, bool cond, char *failmsg);

#endif // TEST_SUITE_H_
