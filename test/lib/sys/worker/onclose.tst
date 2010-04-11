/*
 *  onclose Tests
 */
var w: Worker

w = new Worker("lib/sys/worker/onclose.es")
let exitCount = 0
w.onclose = function (e) {
    exitCount++
}
Worker.join(w)
assert(exitCount == 1)

exitCount = 0
workers = []
for (i in 4) {
    w = new Worker("lib/sys/worker/onclose.es")
    workers.append(w)
    assert(w.name)
    // print("Start " + w.name)
    w.onclose = function (e) {
        exitCount++
    }
}
Worker.join(workers)
assert(exitCount == 4)
