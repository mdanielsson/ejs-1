/*
 *	Try blocks starting at the same location were failing rethrows
 */

var state = 0

try {
	try {
		throw "Go no further"
	} catch {
		throw "RETHROW"

	} finally {
	}

} catch {
    assert(state++ == 0)

} finally {
    assert(state++ == 1)
}
assert(state == 2)
