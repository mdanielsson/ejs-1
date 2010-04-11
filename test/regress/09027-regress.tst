/*
 *  Test extending a native class
 */
class MyError extends Error
{
    public function MyError(message: String) {
         super(message);
    }
}

var caught = false
try {
    throw new MyError("foobar");
} catch (e) {
    caught = true
    assert(e is Error)
}
assert(caught == true)
