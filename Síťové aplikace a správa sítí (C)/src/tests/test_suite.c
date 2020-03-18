/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

test_suite.c - modul pro testování v jazyce C

**/

#include "test_suite.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// Lock, only one test at the time
bool testing = false;

// Number of modules tested
int module_total = 0;

// Stats for all modules
int tested_total = 0;
int failed_total = 0;
int asserts_total = 0;

// Stats for one module
int tested_current_module = 0;
int failed_current_module = 0;

// Whether current test in module failed
bool current_test_failed = false;

// Storage for assert messages
int assert_messages_num = 0;
char **assert_messages = NULL;

bool begin_test_session() {
	if (testing) {
		return false;
	}
	testing = true;

	printf("\nLaunching tests...\n\n");
	
	module_total = 0;
	tested_total = 0;
	failed_total = 0;

	return true;
}

bool end_test_session() {
	if (!testing) {
		return false;
	}
	testing = false;

	printf("Test results:\n"
		   "  Modules: " WHI "%d" NOR "\n"
		   "  Tests total: " WHI "%d" NOR "\n"
		   "  Tests passed: %s%d" NOR "\n"
		   "  Asserts total: " WHI "%d" NOR "\n\n", module_total, tested_total, failed_total != 0 ? RED : GRE, tested_total-failed_total, asserts_total
	);

	return true;
}

bool test_module(char *name, funcPtr f) {
	if (!testing) {
		return false;
	}

	printf("--- TESTING MODULE '" YEL "%s" NOR "' ---\n", name == NULL ? "" : name);

	module_total++;
	tested_current_module = 0;
	failed_current_module = 0;

	(*f)();

	tested_total += tested_current_module;
	failed_total += failed_current_module;

	int tested = tested_current_module;
	int passed = tested_current_module-failed_current_module;
	printf("--- MODULE '" WHI "%s" NOR "' ENDED, PASSED " WHI "%d" NOR "/%d (%s%d%%" NOR ") ---\n\n", 
		name == NULL ? "" : name, passed, tested, passed != tested ? RED : (tested == 0 ? NOR : GRE) , tested != 0 ? (100*passed/tested) : 0);

	return (failed_current_module == 0);
}

bool run_test(char *name, funcPtr f) {
	if (!testing) {
		return false;
	}

	printf("%6s%-46s", "- ", name == NULL ? "unnamed test" : name);
	fflush(stdout);

	tested_current_module++;
	current_test_failed = false;

	(*f)();

	if (current_test_failed) {
		failed_current_module++;
		printf(RED "FAILED" NOR "\n");
	} else {
		printf(GRE "PASSED" NOR "\n");
	}

	for (int i = 0; i < assert_messages_num; i++) {
		printf(assert_messages[i]);
		free(assert_messages[i]);
		assert_messages[i] = NULL;
	}
	assert_messages_num = 0;
	free(assert_messages);
	assert_messages = NULL;

	return !current_test_failed;
}

void assert_printf(const char *format, ...) {
	va_list arg;

	va_start(arg, format);
	assert_messages_num++;
	assert_messages = (char **) realloc(assert_messages, assert_messages_num * sizeof(char *));
	ssize_t buffer_size = vsnprintf(NULL, 0, format, arg);
	va_end(arg);

	va_start(arg, format);
	assert_messages[assert_messages_num-1] = (char *) malloc((buffer_size + 1) * sizeof(char));
	vsnprintf(assert_messages[assert_messages_num-1], buffer_size + 1, format, arg);
	va_end(arg);
}

bool assert_false(char *desc, bool b) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (!b) {
		return true;
	} else {
		current_test_failed = true;
		assert_printf("%8sAssert '%s' failed, expected false but got true\n", "", desc == NULL ? "" : desc);
		return false;
	}
}

bool assert_true(char *desc, bool b) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (b) {
		return true;
	} else {
		current_test_failed = true;
		assert_printf("%8sAssert '%s' failed, expected true but got false\n", "", desc == NULL ? "" : desc);
		return false;
	}
}

bool assert_is_null(char *desc, void *ptr) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (ptr == NULL) {
		return true;
	} else {
		current_test_failed = true;
		assert_printf("%8sAssert '%s' failed, expected NULL but got NOT NULL\n", "", desc == NULL ? "" : desc);
		return false;
	}
}

bool assert_not_null(char *desc, void *ptr) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (ptr != NULL) {
		return true;
	} else {
		current_test_failed = true;
		assert_printf("%8sAssert '%s' failed, expected NOT NULL but got NULL\n", "", desc == NULL ? "" : desc);
		return false;
	}
}

bool assert_is_equal(char *desc, int n1, int n2) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (n1 == n2) {
		return true;
	} else {
		current_test_failed = true;
		assert_printf("%8sAssert '%s' failed, %d does not equal %d\n", "", desc == NULL ? "" : desc, n1, n2);
		return false;
	}
}

bool assert_not_equal(char *desc, int n1, int n2) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (n1 != n2) {
		return true;
	} else {
		current_test_failed = true;
		assert_printf("%8sAssert '%s' failed, %d equals %d\n", "", desc == NULL ? "" : desc, n1, n2);
		return false;
	}
}

bool assert_string_equal(char *desc, char *s1, char *s2) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (s1 != NULL && s2 != NULL && strcmp(s1, s2) == 0) {
		return true;
	} else {
		current_test_failed = true;
		if (s1 == NULL && s2 == NULL) {
			assert_printf("%8sAssert '%s' failed, NULL does not equal NULL\n", "", desc == NULL ? "" : desc);
		} else if (s1 == NULL && s2 != NULL) {
			assert_printf("%8sAssert '%s' failed, NULL does not equal '%s'\n", "", desc == NULL ? "" : desc, s2);
		} else if (s1 != NULL && s2 == NULL) {
			assert_printf("%8sAssert '%s' failed, '%s' does not equal NULL\n", "", desc == NULL ? "" : desc, s1);
		} else {
			assert_printf("%8sAssert '%s' failed, '%s' does not equal '%s'\n", "", desc == NULL ? "" : desc, s1, s2);
		}
		return false;
	}
}

bool assert_string_length(char *desc, char *s, int l) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (s != NULL && strlen(s) == l) {
		return true;
	} else {
		current_test_failed = true;
		if (s == NULL) {
			assert_printf("%8sAssert '%s' failed, length of NULL is not %d\n", "", desc == NULL ? "" : desc, l);
		} else {
			assert_printf("%8sAssert '%s' failed, length of '%s' is not %d\n", "", desc == NULL ? "" : desc, s, l);
		}
		return false;
	}
}

bool assert_is_smaller(char *desc, int n1, int n2) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (n1 < n2) {
		return true;
	} else {
		current_test_failed = true;
		assert_printf("%8sAssert '%s' failed, %d is not smaller than %d\n", "", desc == NULL ? "" : desc, n1, n2);
		return false;
	}
}

bool assert_is_bigger(char *desc, int n1, int n2) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (n1 > n2) {
		return true;
	} else {
		current_test_failed = true;
		assert_printf("%8sAssert '%s' failed, %d is not bigger than %d\n", "", desc == NULL ? "" : desc, n1, n2);
		return false;
	}
}


bool assert_custom(char *desc, bool cond, char *failmsg) {
	if (!testing) {
		return false;
	}

	asserts_total++;
	if (cond) {
		return true;
	} else {
		current_test_failed = true;
		if (failmsg == NULL) {
			assert_printf("%8sAssert '%s' failed, custom condition is false\n", "", desc == NULL ? "" : desc);
		} else {
			assert_printf("%8sAssert '%s' failed, %s\n", "", desc == NULL ? "" : desc, failmsg);
		}
		
		return false;
	}
}
