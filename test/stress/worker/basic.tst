/*
 *  Basic Worker Tests
 */
var w: Worker

if (test.depth >= 3) {
    for (i in 1000) {
        w = startWorker("stress/worker/basic.es")
        Worker.join(w)
    }
} else {
    test.skip("Run at level 3")
}

function startWorker(file: Path): Worker {
    let export = { 
        anything: "some words",
    }
    let w: Worker = new Worker
    w.name = "Some Name"
    w.postMessage(export)
    // w.preload(_top.join("build/bin/utest.worker"))

    let test = this
    w.onmessage = function (e) {
        print("GOT MSG")
    }
    w.onerror = function (e) {
        if (e.stack) {
            throw "Test Failed: \"" + e.message + " At:\n" + e.stack
        } else {
            throw "Test Failed: " + e.message
        }
    }
    w.onclose = function (e) {
        // print("CLOSED")
    }
    w.load(file)
    return w
}
