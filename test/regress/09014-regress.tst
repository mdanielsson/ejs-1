/*
 *  Was not handling !dir.same(). Was not pushing result. Code in genUnaryOp was not pushing result value.
 */

var dir: Path = new Path(".")
var other: Path = new Path("./a")
var i = 0
do {
    if (i++ == 10) break
} while (!dir.same(other))

assert(i == 11)
