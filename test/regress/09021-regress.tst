/*
 *  Test rethrow inside a function without a finally block
 */
var state = 0

function fun() {
    try {
        assert(state++ == 1)
        throw "inside try"
        assert(0)
    } catch {
        assert(state++ == 2)
        throw "Inside catch"
        assert(0)
    }
}


try {
    assert(state++ == 0)
    fun()
} catch (e) {
    assert(state++ == 3)
}
assert(state++ == 4)
