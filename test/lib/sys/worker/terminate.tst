/*
 *  Terminate Worker Tests
 */
var w: Worker

w = new Worker("lib/sys/worker/terminate.es")
App.sleep(100)
w.terminate()
w.join()
