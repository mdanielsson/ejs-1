/*
 *  ejsmod.tst - Test stand-alone VM
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
sh(locate("ejsc").portable + " --out ejs.test.mod misc/test.stub.es")

if (test.depth >= 4) {
    for (i = 0; i < test.depth && i < compileFlags.length; i++) {
        let compile = locate("ejsc").portable + " --doc " + compileFlags[i]

        for each (f in find(["lang", "db", "lib", "regress"], "*.tst")) {
            rmdir(".doctmp", true)

            if (testCmd(compile + " " + f)) {
                assert(exists("default.mod"))
                if (testCmd("ejsmod --listing --showDebug default.mod")) {
                    assert(exists("default.mod"))
                    assert(exists("default.lst"))
                    rm("default.lst")
                }
                if (testCmd("ejsmod --cslots default.mod")) {
                    assert(exists("default.slots.h"))
                    rm("default.slots.h")
                }
                if (testCmd("ejsmod --html .doctmp default.mod")) {
                    assert(exists(".doctmp/index.html"))
                    rmdir(".doctmp", true)
                }
                rm("default.mod")
            }
        }
    }
    rm("ejs.test.mod")

} else {
    test.skip("Runs at depth 2")
}
