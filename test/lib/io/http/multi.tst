/*
 *  Http class tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.get(URL)
    assert(http.code == 200)
    http.get(URL)
    assert(http.code == 200)
    http.get(URL)
    assert(http.code == 200)
    http.get(URL)
    assert(http.code == 200)

} else {
    test.skip("Http not enabled")
}
