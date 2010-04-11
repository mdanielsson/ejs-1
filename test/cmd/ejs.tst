/*
 *  ejs.tst - Test compiler
 */

// 

let ejsFlags = [
    "",
    "--optimize 1",
    "--optimize 9",
    "--nodebug",
    "--nobind",
]

/*
 *  Compile a test stub so we can use the same tests in commands
 */
sh(locate("ejsc") + " --out ejs.test.mod misc/test.stub.es")

if (test.depth >= 2) {
    for (i = 0; i < test.depth && i < ejsFlags.length; i++) {
        let flags = ejsFlags[i]
        let command = "ejs" + " " + flags

        for each (f in find(["lang", "db", "lib", "regress"], "*.tst")) {
            testCmd(command + f)
        }
    }
    rm("ejs.test.mod")

} else {
    test.skip("Runs at depth 2")
}
