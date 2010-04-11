
/*
 *  Test nested exception blocks
 */
var state = 0

//
//  Test no exception
//
state = 0
try {
    assert(state++ == 0)
    try {
        assert(state++ == 1)
    } catch {
        assert(0)
    } finally {
        assert(state++ == 2)
    }
    assert(state++ == 3)
} catch {
    assert(0)
} finally {
    assert(state++ == 4)
}
assert(state++ == 5)

//
//  Test with simple throw in inner try
//
state = 0
try {
    assert(state++ == 0)
    try {
        assert(state++ == 1)
        throw new Error("FIRST")                        //  Throw here
    } catch {
        assert(state++ == 2)
    } finally {
        assert(state++ == 3)
    }
    assert(state++ == 4)
} catch {
    assert(0)
} finally {
    assert(state++ == 5)
}
assert(state++ == 6)

//
//  Test re-throw in catch
//
state = 0
try {
    assert(state++ == 0)
    try {
        assert(state++ == 1)
        throw new Error("FIRST")                        //  Throw here
    } catch {
        assert(state++ == 2)
        throw new Error("another")                      //  Re-throw here
    } finally {
        assert(state++ == 3)
    }
    assert(0)
} catch {
    assert(state++ == 4)
} finally {
    assert(state++ == 5)
}
assert(state++ == 6)

//
//  Test re-throw in finally
//
state = 0
try {
    assert(state++ == 0)
    try {
        assert(state++ == 1)
        throw new Error("FIRST")                        //  Throw here
    } catch {
        assert(state++ == 2)
    } finally {
        assert(state++ == 3)
        throw new Error("another")                      //  Re-throw here
    }
    assert(0)
} catch {
    assert(state++ == 4)
} finally {
    assert(state++ == 5)
    // throw new Error("SECOND")
}
assert(state++ == 6)

//
//  Test re-throw in outer catch
//
state = 0
try {
    assert(state++ == 0)
    try {
        assert(state++ == 1)
        throw new Error("FIRST")
    } catch {
        assert(state++ == 2)
    } finally {
        assert(state++ == 3)
        throw new Error("another")
    }
    assert(0)
} catch {
    assert(state++ == 4)
} finally {
    assert(state++ == 5)
}
assert(state++ == 6)
