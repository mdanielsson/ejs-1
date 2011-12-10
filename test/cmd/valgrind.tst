/*
 *  valgrind.tst - Test compiler
 */

// 

module test.api.c {

    let ejsFlags = [
        "",
        "--optimize 1",
        "--optimize 9",
        "--nodebug",
        "--nobind",
    ]
    let valgrind = "/usr/bin/env valgrind -q --tool=memcheck --suppressions=ejs.supp " 

    if (test.os == "LINUX" && test.depth >= 2) {
        for (i = 0; i < test.depth && i < ejsFlags.length; i++) {
            let flags = ejsFlags[i]
            let command = "ejs " + flags

            for each (f in find("../scripts", "*.es")) {
                testCmd(valgrind + command + f.portable)
            }
        }
    } else {
        test.skip("Run on Linux")
    }

}
