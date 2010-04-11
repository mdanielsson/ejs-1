// 
/*
 *	Test finally code is executed when no exceptions are being thrown.
 */

state = 0

try {
	assert(state == 0)
	state = 1
} catch {
	assert(0)
	state = 2
} finally {
	assert(state == 1)
	state = 2
}
assert(state == 2)

//
//  Test correct scope inside finally blocks without exception thrown
//
state = 0
function fun() {
    var x = 7
    try {
        assert(state++ == 1)
    } catch {
        assert(0)
    } finally {
        assert(state++ == 2)
        assert(x == 7)
    }
    assert(state++ == 3)
}
assert(state++ == 0)
fun()
assert(state++ == 4)

//
//  Test correct scope inside finally blocks with exception thrown
//
state = 0
function fun2() {
    var x = 7
    try {
        assert(state++ == 1)
        throw "THROWN"
    } catch {
        assert(state++ == 2)
    } finally {
        assert(state++ == 3)
        assert(x == 7)
    }
    assert(state++ == 4)
}
assert(state++ == 0)
fun2()
assert(state++ == 5)
