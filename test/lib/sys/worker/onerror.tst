/*
 *  onerror Tests
 */
var w: Worker

w = new Worker("lib/sys/worker/onerror.es")
w.onerror = function (e) {
    assert(e.message.contains("Can't continue"))
    assert(e.stack)
    assert(e.lineno > 0)
    // print("Error: " + e.message + " in " + e.filename + "@" + e.lineno)
    // print("Stack " + e.stack)
}
Worker.join()
