/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

test_url_parser.c - testování modulu pro zpracování URL zdroje

**/

#include "test_suite.h"

#include "../url_parser.h"

void test401() {
	struct url_content c = get_url_content();

    assert_false("is not HTTPS", c.is_https);
    assert_is_null("host is NULL", c.host);
    assert_is_null("port is NULL", c.port);
    assert_is_null("resource is NULL", c.resource);
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test402() {
	char *str = "";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    assert_is_null("host is NULL", c.host);
    assert_is_null("port is NULL", c.port);
    assert_is_null("resource is NULL", c.resource);
    assert_is_equal("error code is 'no protocol'", c.err_code, URL_ERR_PROTOCOL);
	assert_not_null("error msg allocated", c.err_msg);

	free_url_content(&c);
}

void test403() {
	char *str = "\n";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    assert_is_null("host is NULL", c.host);
    assert_is_null("port is NULL", c.port);
    assert_is_null("resource is NULL", c.resource);
    assert_is_equal("error code is 'no protocol'", c.err_code, URL_ERR_PROTOCOL);
	assert_not_null("error msg allocated", c.err_msg);

	free_url_content(&c);
}

void test404() {
	char *str = "host.com";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    assert_is_null("host is NULL", c.host);
    assert_is_null("port is NULL", c.port);
    assert_is_null("resource is NULL", c.resource);
    assert_is_equal("error code is 'no protocol'", c.err_code, URL_ERR_PROTOCOL);
	assert_not_null("error msg allocated", c.err_msg);

	free_url_content(&c);
}

void test405() {
	char *str = "http://";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    assert_is_null("host is NULL", c.host);
    assert_is_null("port is NULL", c.port);
    assert_is_null("resource is NULL", c.resource);
    assert_is_equal("error code is 'invalid host'", c.err_code, URL_ERR_HOST);
	assert_not_null("error msg allocated", c.err_msg);

	free_url_content(&c);
}

void test406() {
	char *str = "HTTPS://";
	struct url_content c = parse_url(str);

    assert_true("is HTTPS", c.is_https);
    assert_is_null("host is NULL", c.host);
    assert_is_null("port is NULL", c.port);
    assert_is_null("resource is NULL", c.resource);
    assert_is_equal("error code is 'invalid host'", c.err_code, URL_ERR_HOST);
	assert_not_null("error msg allocated", c.err_msg);

	free_url_content(&c);
}

void test407() {
	char *str = "http://aa";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    assert_is_null("host is NULL", c.host);
    assert_is_null("port is NULL", c.port);
    assert_is_null("resource is NULL", c.resource);
    assert_is_equal("error code is 'invalid host'", c.err_code, URL_ERR_HOST);
	assert_not_null("error msg allocated", c.err_msg);

	free_url_content(&c);
}

void test408() {
	char *str = "http://a.a";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "a.a");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "80");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test409() {
	char *str = "http://a.a:";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "a.a");
    assert_is_null("port is NULL", c.port);
    assert_is_null("resource is NULL", c.resource);
    assert_is_equal("error code is 'invalid port'", c.err_code, URL_ERR_PORT);
	assert_not_null("error msg allocated", c.err_msg);

	free_url_content(&c);
}

void test410() {
	char *str = "http://a.a:https";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "a.a");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "https");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test411() {
	char *str = "https://a.a:80";
	struct url_content c = parse_url(str);

    assert_true("is HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "a.a");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "80");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test412() {
	char *str = "hTTPs://a.a:-5";
	struct url_content c = parse_url(str);

    assert_true("is HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "a.a");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "-5");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test413() {
	char *str = "https://bb&ccc";
	struct url_content c = parse_url(str);

    assert_true("is HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "bb&ccc");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "443");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test414() {
	char *str = "http://:555//d";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    assert_is_null("host is NULL", c.host);
    assert_is_null("port is NULL", c.port);
    assert_is_null("resource is NULL", c.resource);
    assert_is_equal("error code is 'invalid host'", c.err_code, URL_ERR_HOST);
	assert_not_null("error msg allocated", c.err_msg);

	free_url_content(&c);
}

void test415() {
	char *str = "http://a.a:555//d";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "a.a");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "555");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "//d");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test416() {
	char *str = "HtTpS://a.a/res?res=4&val=5#anchor";
	struct url_content c = parse_url(str);

    assert_true("is HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "a.a");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "443");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/res?res=4&val=5#anchor");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test417() {
	char *str = "HTTP://www.host.com:555/res.htm?var=val";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "www.host.com");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "555");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/res.htm?var=val");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test418() {
	char *str = "http://host\n\t:port/res.res?res=res&res=res";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "host\n\t");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "port");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/res.res?res=res&res=res");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test419() {
	char *str = "http://host:po\n\trt/res.res?res=res&res=res";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "host");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "po\n\trt");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/res.res?res=res&res=res");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test420() {
	char *str = "http://host:port/res.res?r\n\tes=res&res=res";
	struct url_content c = parse_url(str);

    assert_false("is not HTTPS", c.is_https);
    if (assert_not_null("host is not NULL", c.host))
    	assert_string_equal("host properly saved", c.host, "host");
    if (assert_not_null("port is not NULL", c.port))
    	assert_string_equal("port properly saved", c.port, "port");
    if (assert_not_null("resource is not NULL", c.resource))
    	assert_string_equal("resource properly saved", c.resource, "/res.res?r\n\tes=res&res=res");
    assert_is_equal("no error code", c.err_code, URL_ERR_OK);
	assert_is_null("no error msg", c.err_msg);

	free_url_content(&c);
}

void test_url_parser() {

	run_test("get_url_content()", &test401);
	run_test("empty string", &test402);
	run_test("'\\n'", &test403);
	run_test("no protocol", &test404);
	run_test("no host", &test405);
	run_test("protocol case insensitive", &test406);
	run_test("too short host", &test407);
	run_test("minimalistic working", &test408);
	run_test("no port after ':'", &test409);
	run_test("port is \"https\"", &test410);
	run_test("https with port 80", &test411);
	run_test("port is -5", &test412);
	run_test("host with '&'", &test413);
	run_test("no host with doubleslash", &test414);
	run_test("doubleslash in resource", &test415);
	run_test("complex URL 1", &test416);
	run_test("complex URL 2", &test417);
	run_test("host with newline and tab", &test418);
	run_test("port with newline and tab", &test419);
	run_test("resource with newline and tab", &test420);

}
