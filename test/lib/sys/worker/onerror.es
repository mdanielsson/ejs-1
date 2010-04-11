/*
 *  onerror worker
 */

function fun() {
    throw new Error("Can't continue")
}

assert(self)
fun()
