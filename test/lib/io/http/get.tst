/*
 *  Get tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.get(URL)
    assert(http.code == 200)
    assert(http.response.contains("Hello /index.html"))
    assert(!http.isSecure)

    http.get(URL)
    assert(http.readString(6) == "<html>")
    assert(http.readString(6) == "<head>")

} else {
    test.skip("Http not enabled")
}
