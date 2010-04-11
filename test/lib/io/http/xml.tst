/*
 *  XML tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    var http: Http = new Http

    http.get(HTTP + "/test.xml")
    assert(http.readXml().customer.name == "Joe Green")

} else {
    test.skip("Http not enabled")
}
