/*
 *  Big file transfers
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const BIG = HTTP + "/big.ejs"
    var http: Http = new Http

    http.get(BIG)
    buf = new ByteArray
    while (http.read(buf) > 0) {
        assert(buf.available > 0)
    }

/*
    http.get(BIG)
    ts = new TextStream(http)
    lines = ts.readLines()
print(lines.length)
    assert(lines.length == 801)
*/
} else {
    test.skip("Http not enabled")
}
