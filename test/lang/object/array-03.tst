
/*
 *	Test array iteration
 */

var a:Array = new Array(20)
assert(a.length == 20)

for (i = 0; i < a.length; i++) {
	assert(a[i] == undefined)
}
assert(i == 20)
