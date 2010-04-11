
/*
 *	New variables in EJS mode should be made locals
 */

function fun() {
	x = 2
	assert(x == 2)
}

fun()

assert(global.x == undefined)
