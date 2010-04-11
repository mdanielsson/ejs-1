/*
 *  Injected into the tests global scope
 */
module ejs.test {

    class Test {
        public var bin: Path = new Path(".")
        public var config: Object = {}
        public var depth: Number = 1
        public var lib: Path = new Path(".")
        public var multithread: Number = 1
        public var top: Path = new Path(".")
        public var os: Path = new Path(".")
        public var verbosity: Number = false

        public function log(level: Number, name: String, ...items): Void {
        }

        function searchUp(path: Path): String
            ""

        public function mapVerbosity(adj: Number): String
            ""

        public function skip(message: String): Void {
        }
    }

    function assert(cond: Boolean, message: String = null): Void {
    }

    function cleanDir(path) {
    }

    function sh(args): Void {
        System.runx("/bin/sh -c \"" + env() + args + "\"")
    }

    function testCmd(args): Boolean
        true

    function env(): String
        ""

    function locate(program): String
        ""

    function runx(cmd) {
        System.runx(cmd)
    }

    public function share(key: String, value: String): void {
    }

    public var onmessage = function(e) {
    }

    public var test: Test = new Test
    public var session: Object = {}

}
