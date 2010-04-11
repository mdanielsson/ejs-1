/*
 *  Redirect tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.get(HTTP + "/dir")
    http.followRedirects = false
    assert(http.code == 301)
    http.followRedirects = true
    http.get(HTTP + "/dir")
    assert(http.code == 200)

} else {
    test.skip("Http not enabled")
}
