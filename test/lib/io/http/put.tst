/*
 *  Put tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    data = Path("lib/io/file.dat").readString()
    http.put(HTTP + "/tmp/file.dat", data)
    assert(http.code == 201 || http.code == 204)

    http.bodyLength = Path("lib/io/file.dat").size
    http.put(HTTP + "/tmp/file.dat")
    file = Path("lib/io/file.dat").open()
    buf = new ByteArray
    while (file.read(buf) > 0) {
        http.write(buf)
    }
    http.write()
    http.wait()
    assert(http.code == 201 || http.code == 204)

} else {
    test.skip("Http not enabled")
}
