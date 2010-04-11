/*
    Test boolean constructor
 */
assert(new Boolean("test"))
assert(new Boolean("true"))
assert(new Boolean(1))
assert(new Boolean(true))

assert(!Boolean(false))
assert(!Boolean(null))
assert(!Boolean(undefined))
