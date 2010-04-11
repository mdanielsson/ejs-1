/*
 *  Header tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.addHeader("key", "value")
    http.get(URL)
    assert(http.code == 200)

    http.get(URL)
    connection = http.header("Connection")
    assert(connection == "keep-alive")

    http.get(URL)
    assert(http.codeString == "OK")
    assert(http.contentType == "text/html")
    assert(http.date.toString().contains("GMT"))
    assert(http.lastModified.toString().contains("GMT"))

} else {
    test.skip("Http not enabled")
}
