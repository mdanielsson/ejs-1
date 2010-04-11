/*
 *  Form tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.form(HTTP + "/form.ejs", {name: "Julie", address: "700 Park Ave"})
    assert(http.response.contains('"name": "Julie"'))
    assert(http.code == 200)

} else {
    test.skip("Http not enabled")
}
