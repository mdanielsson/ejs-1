
/*
 *	Tests for the App class
 */

/*
    stream = App.outputStream()
    stream.write("Hello World\n")

    stream = App.errorStream()
    stream.write("Error\n")


    ip = new TextStream(App.inputStream())
    while (s = ip.readLine()) {
        print("GOT \"" + s + "\"")
    }
    print(s)
    App.exit(1)
*/


// exeDir 

assert(App.exeDir.name.length > 0)
assert(App.exeDir.exists)
assert(App.exeDir.name.search("%") < 0)

//	args

/* set in workers
print("ARGS " + App.args)
print("LEN " + App.args.length)
assert(App.args != "")
assert(App.args.length > 0)
*/

//	sleep

d = new Date
App.sleep(100)
elapsed = new Date - d
