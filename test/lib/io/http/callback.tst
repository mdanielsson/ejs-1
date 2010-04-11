/*
 *  Callback tests
 */


if (test.config["http_client"] == 1 && session["http"]) {

    const HTTP = session["http"]
    const URL = HTTP + "/index.html"
    var http: Http = new Http

    /* FUTURE
    http.setCallback(Http.Read, function (e) {
        if (e is HttpDataEvent) {
            if (e.eventMask == Http.Read) {
                data = http.readString()
            }
        } else if (e is HttpErrorEvent) {
            throw e
        } else {
            throw "Bad event in http callbac"
        }
    })
    http.get(BIG)
    http.wait()
    print(http.code)
    */

    /* FUTURE
    writeCount = 5
    http.chunked = true
    http.setCallback(Http.Write, function (e) {
        if (e is HttpDataEvent) {
            print("MASK " + e.eventMask)
            if (e.eventMask & Http.Write) {
                print("WRITE DATA " + writeCount)
                if (writeCount-- > 0) {
                    http.write("WRITE DATA " + writeCount + " \n")
                } else {
                    http.write()
                }
            }
            if (e.eventMask & Http.Read) {
                print("READ EVENT ")
            }
        } else if (e is HttpErrorEvent) {
            throw e
        } else {
            throw "Bad event in http callbac"
        }
    })
    http.post(HTTP + "/f.ejs")
    http.wait()
    print("CODE " + http.code)
    print("GOT  " + http.response.length + " bytes of response")
    */

} else {
    test.skip("Http not enabled")
}
