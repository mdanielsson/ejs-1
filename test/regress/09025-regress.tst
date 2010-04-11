/*
 *  Fix iterations inside finally blocks
 */

function fun() {
    try {
        throw new Error("11")
    } finally {
        for (i in 1) { }
    }
}

try {
    fun()
} catch (e) {
    /* The bug was "e" was being set to the iteration object in the above finally block */
    assert(e is Error)
}
