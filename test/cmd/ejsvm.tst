/*
 *  ejsvm.tst - Test stand-alone VM
 */

// 

let compileFlags = [
    "",
    "--debug",
]

/*
 *  Compile a test stub so we can use the same tests in commands
 */
rm("default.mod")
sh(locate("ejsc") + " --out ejs.test.mod misc/test.stub.es")

if (test.depth >= 2) {
    for (i = 0; i < test.depth && i < compileFlags.length; i++) {
        let compile = locate("ejsc") + " " + compileFlags[i]

        for each (f in find(["lang", "db", "lib", "regress"], "*.tst")) {
            if (testCmd(compile + f)) {
                assert(exists("default.mod"))
                testCmd("ejsvm default.mod")
            }
            rm("default.mod")
        }
    }
    rm("ejs.test.mod")

} else {
    test.skip("Runs at depth 2")
}
