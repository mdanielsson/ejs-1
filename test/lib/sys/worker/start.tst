/*
 *  Start Worker Tests
 */

w = new Worker
w.load("lib/sys/worker/start.es")
let exitCount = 0
w.onclose = function (e) {
    exitCount++
}
w.join()
assert(exitCount == 1)

//  Test start with a name
w = new Worker(null, { name: "funny-worker" })
exitCount = 0
w.load("lib/sys/worker/start.es")
w.onclose = function (e) {
    exitCount++
    assert(this.name == "inside-funny-worker")
}
w.join()
assert(exitCount == 1)

//  Test receiving a message from the started script
w = new Worker("lib/sys/worker/start.es")
w.onmessage = function (e) {
    let o = deserialize(e.data)
    assert(o.name == "Mary")
    assert(o.address == "123 Park Ave")
}
Worker.join()
