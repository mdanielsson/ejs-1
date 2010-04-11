/*
 *  Authentication tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.get(HTTP + "/basic/basic.html")
    assert(http.code == 401)
    http.setCredentials("joshua", "pass1")
    http.get(HTTP + "/basic/basic.html")
    assert(http.code == 200)

} else {
    test.skip("Http not enabled")
}
