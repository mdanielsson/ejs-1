/*
 *  Join Worker Tests
 */
var w: Worker

//  Single-join
w = new Worker("lib/sys/worker/join.es")
Worker.join()

//  Multi-join
w = new Worker("lib/sys/worker/join.es")
for (i = 0; i < 10; i++) {
    if (Worker.join(null, 1000)) {
        break
    }
}
assert(i < 10)
