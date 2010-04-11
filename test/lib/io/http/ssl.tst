/*
 *  SSL tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTPS = session["https"]
    const URLS = HTTPS + "/index.html"

    var http: Http = new Http

    if (test.config["ssl"] == 1) {
        http.get(HTTPS)
        assert(http.code == 200)
        assert(http.isSecure)
    } else {
        test.skip("SSL not enabled")
    }

} else {
    test.skip("Http not enabled")
}
