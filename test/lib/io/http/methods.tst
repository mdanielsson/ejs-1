/*
    Method tests
 */

if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.options(URL)
    assert(http.header("Allow") == "OPTIONS,GET,HEAD,POST,PUT,DELETE")

    http.trace(URL)
    assert(http.code == 406)

} else {
    test.skip("Http not enabled")
}
