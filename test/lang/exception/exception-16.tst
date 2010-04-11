
/*
 *  Test nested exception blocks
 */

state = 0
try {
    assert(state++ == 0)
    try {
        assert(state++ == 1)
        throw 0
    } catch {
        assert(state++ == 2)
    } finally {
        assert(state++ == 3)
    }
} catch {
    assert(0)
} finally {
    assert(state++ == 4)
}
assert(state++ == 5)
