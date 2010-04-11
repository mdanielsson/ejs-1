
//
//  Test return/break inside a catch block. Finally must still be executed resuming instruction
//

var state = 1

//
//  Test return in catch
//
function fun() {
    assert(state++ == 1)
    try {
        assert(state++ == 2)
        throw "THROW"
    } catch {
        assert(state++ == 3)
        return
        assert(0)
    } finally {
        assert(state++ == 4)
    }
}
fun()
assert(state++ == 5)


//
//  Test break in catch
//
state = 1
while (1) {
    try {
        assert(state++ == 1)
        throw "THROW"
    } catch {
        assert(state++ == 2)
        break
        assert(0)
    } finally {
        assert(state++ == 3)
    }
    assert(0)
}
assert(state++ == 4)


//
//  Test break inside continue
//
state = 1
try {
    assert(state++ == 1)
    throw "THROW"
} catch {
    assert(state++ == 2)
    switch (1) {
    case 1:
        assert(state++ == 3)
        break;
    default:
        assert(0)
    }
    assert(state++ == 4)
} finally {
    assert(state++ == 5)
}
assert(state++ == 6)


//
//  Test catch break inside a function. Ensure access to local var remains
//
function fun2() {
    var x = 7
    for (i in 1) {
        try {
            assert(x == 7)
            throw "THROW " + x
        } catch (e) {
            assert(x == 7)
            break
        } finally {
            assert(x == 7)
        }
    }
    assert(x == 7)
}

fun2()
