/*
 *  Worker -- Worker classes
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.sys {

    use strict

    /**
     *  Worker Class. Worker threads are medium-weight thread-based virtual machine instances. They run separate 
     *  interpreters with tightly controlled data interchange. This class is currently being specified by the
     *  WebWorker task group (See http://www.whatwg.org/specs/web-workers/current-work/#introduction).
     *  This class is prototype and highly likely to change
     *  @stability prototype.
     *  @spec WebWorker-W3C
     */
    class Worker {
        use default namespace public

        /**
         *  Callback function invoked when the worker exits. 
         *  The "this" object is set to the worker object.
         */
        var onclose: Function

        /**
         *  Callback function to receive incoming messages. This is invoked when postMessage is called in another Worker. 
         *  The "this" object is set to the worker object.
         *  This is invoked as: function (event) { }
         */
        var onmessage: Function

        /**
         *  Callback function to receive incoming error events. This is invoked when the Worker thows an exception. 
         *  The "this" object is set to the worker object.
         */
        var onerror: Function

        /**
         *  Worker name. This name is initialized but workers can update as required.
         *  @spec ejs
         */
        var name: String

        /**
         *  Create a new Worker instance. This call returns an outside worker object for using in the calling interpreter.
         *      Inside the worker interpreter, a corresponding "insdie" worker object is created that is paired to the
         *      outside worker.
         *  @params script Optional path to a script or module to execute. If supplied, then a new Worker instance will
         *      invoke load() to execute the script.
         *  @params options Options hash
         *  @options search Search path
         *  @options name Name of the Worker instance.
         *  @spec WebWorker
         */
        native function Worker(script: Path = null, options: Object = null)

        /**
         *  Load the script. The literal script is compiled as a JavaScript program and loaded and run.
         *  This is similar to the global eval() command but the script is run in its own interpreter and does not
         *  share any data the the invoking interpreter. The result is serialized in the worker and then deserialized
         *  (using JSON) in the current interpreter. The call returns undefined if the timeout expires.
         *  @param script Literal JavaScript program string.
         *  @param timeout If the timeout is non-zero, this call blocks and will return the value of the last expression in
         *      the script. Otherwise, this call will not block and join() can be used to wait for completion. Set the
         *      timeout to -1 to block until the script completes. The default is -1.
         *  @returns The value of the last expression evaluated in the script. Returns undefined if the timeout 
         *      expires before the script completes.
         *  @throws an exception if the script can't be compiled or if it thows a run-time exception.
         *  @spec ejs
         */
        native function eval(script: String, timeout: Number = -1): String

        /**
         *  Exit the worker.
         *  @spec ejs
         */
        native static function exit(): Void

        /**
         *  Wait for Workers to exit
         *  @param workers Set of Workers to wait for. Can be a single Worker object or an array of Workers. If null or 
         *      if the array is empty, then all workers are waited for.
         *  @param timeout Timeout to wait in milliseconds. The value -1 disables the timeout.
         *  @spec ejs
         */
        native static function join(workers: Object = null, timeout: Number = -1): Boolean

        /**
         *  Load and run a script in a dedicated worker thread. 
         *  @params script Filename of a script or module to load and run. 
         *  @param timeout If the timeout is non-zero, this call blocks and will return the value of the last expression in
         *      the script. Otherwise, this call will not block and join() can be used to wait for completion. Set the
         *      timeout to -1 to block until the script completes. The default is to not block.
         *  @spec ejs
         */
        native function load(script: Path, timeout: Number = 0): Void

        /**
         *  Preload the specified script or module file to initialize the worker. This will run a script using the current
         *  thread and will block. To run a worker using its own thread, use load() or Worker(script).
         *  This call will load the script/module and initialize and run global code. The call will block until 
         *  all global code has completed and the script/module is initialized. 
         *  @param path Filename path for the module or script to load. This should include the file extension.
         *  @returns the value of the last expression in the script or module.
         *  @throws an exception if the script or module can't be loaded or initialized or if it thows an exception.
         *  @spec ejs
         */
        native function preload(path: Path): String

        /**
         *  Lookup a Worker by name
         *  @param name Lookup a Worker
         *  @spec ejs
         */
        native static function lookup(name: String): Worker

        /**
         *  Post a message to the Worker's parent
         *  @param data Data to pass to the worker's onmessage callback.
         *  @param ports Not implemented
         */
        native function postMessage(data: Object, ports: Array = null): Void

        /**
         *  Terminate the worker
         */
        native function terminate(): Void

        /**
         *  Wait for receipt of a message
         *  @param timeout Timeout to wait in milliseconds
         *  @stability prototype
         */
        native function waitForMessage(timeout: Number = -1): Boolean
    }


    /*
     *  Globals for inside workers.
     */
    use default namespace "ejs.sys.worker"

    /**
     *  Reference to the Worker object for use inside a worker script
     *  @returns a Worker object
     */
    var self: Worker

    /**
     *  Exit the worker
     *  @spec ejs
     */
    function exit(): Void
        self.exit()

    /**
     *  Post a message to the Worker's parent
     *  @param data Data to pass to the worker's onmessage callback.
     *  @param ports Not implemented
     */
    function postMessage(data: Object, ports: Array = null): Void
        self.postMessage(data, ports)

    /**
     *  The error callback function
     */
    function get onerror(): Function
        self.onerror

    /**
     *  Set the error callback function
     *  @param fun Callback function to receive incoming data from postMessage() calls.
     */
    function set onerror(fun: Function): Void {
        self.onerror = fun
    }

    /**
     *  The callback function configured to receive incoming messages. 
     */
    function get onmessage(): Function
        self.onmessage

    /**
     *  Set the callback function to receive incoming messages. 
     *  @param fun Callback function to receive incoming data from postMessage() calls.
     */
    function set onmessage(fun: Function): Void {
        self.onmessage = fun
    }

    # WebWorker // Only relevant in browsers 
    var location: WorkerLocation

}
