/*
 *  Misc Http class tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    assert(Http.mimeType("a.txt") == "text/plain")
    assert(Http.mimeType("a.html") == "text/html")

} else {
    test.skip("Http not enabled")
}
