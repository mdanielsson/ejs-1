/*
 *  Regression test to make sure namespaces are scoped to the right blocks
 */

var x = 0
function fun() {
    "blue" var x = 5
    assert(x == 0)
        try {
            /*
             *  ISSUE: this namespace must be scoped just to the try block
             */
            use namespace "blue"
            assert(x == 5)
            throw "THROW"

        } catch {
            assert(x == 0)
        } finally {
            assert(x == 0)
        }
        assert(x == 0)
}

fun()
