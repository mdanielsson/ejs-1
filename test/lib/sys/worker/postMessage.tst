/*
 *  PostMessage Worker Tests
 */
var w: Worker

w = new Worker("lib/sys/worker/postMessage.es")
w.postMessage("Back to you")
Worker.join()
