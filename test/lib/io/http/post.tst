/*
 *  Post tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.post(URL, "Some data")
    assert(http.code == 200)
//BUG
	http.close()

} else {
    test.skip("Http not enabled")
}
