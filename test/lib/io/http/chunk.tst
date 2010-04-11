/*
 *  Http class tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.chunked = true
    http.post(URL)
    assert(http.code == 200)
//BUG
	http.close()

    http.chunked = false
    http.post(URL)
    assert(http.code == 200)

} else {
    test.skip("Http not enabled")
}
