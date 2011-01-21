/*
 *  ejsc.tst - Test compiler
 */

// 

let compileFlags = [
    "",
    "--parse",
    "--debug",
    "--optimize 1",
    "--nobind",
]

/*
 *  Compile a test stub so we can use the same tests in commands
 */
sh(locate("ejsc") + " --out ejs.test.mod misc/test.stub.es")

if (test.depth >= 3) {
    for (i = 0; i < test.depth && i < compileFlags.length; i++) {
        let flags = compileFlags[i]
        let command = locate("ejsc") + " " + flags

        for each (f in find(["lang", "db", "lib", "regress"], "*.tst")) {
            testCmd(command + " " + f)
        }
    }
    rm("ejs.test.mod")

} else {
    test.skip("Runs at depth 2")
}
