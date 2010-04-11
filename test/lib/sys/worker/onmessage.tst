/*
 *  onmessage Tests
 */
var w: Worker

w = new Worker("lib/sys/worker/onmessage.es")
w.onmessage = function (e) {
    assert(e.data.contains("Short Message"))
}
Worker.join()
