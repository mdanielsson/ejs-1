/*
 *  Http class tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    http.upload(HTTP + "/upload.ejs", { myfile: "lib/io/file.dat"} )
    assert(http.response.contains('"clientFilename": "file.dat"'))

} else {
    test.skip("Http not enabled")
}
