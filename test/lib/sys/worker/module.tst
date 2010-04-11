/*
 *  Module Worker Tests
 */

w = new Worker("lib/sys/worker/module.mod")
let msg = null
w.onmessage = function (e) {
    msg = e.data
}
Worker.join()
assert(msg.contains("Module Loaded"))
