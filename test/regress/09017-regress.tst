/*
 *  Fix regression where local variables were not being found when namespaces were being used
 */

var o = {}
function fun() {
    let name = "abc"
    o.public::[name] = 7
}
fun()
assert(o.public::["abc"] == 7)
