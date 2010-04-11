/*
 *  Http class tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.get(URL)
    assert(http.code == 200)
    lines = http.readLines()
    assert(lines.length == 4)
    for (l in lines) {
        assert(lines[l] != "")
    }

} else {
    test.skip("Http not enabled")
}
