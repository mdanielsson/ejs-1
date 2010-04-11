import System.Diagnostics;

// Define indexOf function
function _indexOf(obj) {
	var x = 1
	return x
}
Array.prototype.indexOf = _indexOf

// Define lastIndexOf function
function _lastIndexOf(obj, from) {
	var x = 1
	return x
}
Array.prototype.lastIndexOf = _lastIndexOf

/*
 *	An array can be used as a stack
 */
var o = new Array()

//	Push 3 elements on the stack
o.push(15)
Debug.Assert(o.length == 1, "Array length wrong_1")
Debug.Assert(o[0] == 15, "Array content wrong_1")

o.push(13)
Debug.Assert(o.length == 2, "Array length wrong_2")
Debug.Assert(o[1] == 13, "Array content wrong_2")

o.push(11)
Debug.Assert(o.length == 3, "Array length wrong_3")
Debug.Assert(o[2] == 11, "Array content wrong_3")

print (o)

// Reverse the order
o.reverse()
Debug.Assert(o[0] == 11, "Array content wrong_4")
print (o)

// Check and make sure the zero item is correct
var r = o.indexOf(13)
Debug.Assert(r == 1, "indexOf 13 wrong")

// Find the last occurence of 13
var s = o.lastIndexOf(13, 0)
Debug.Assert(s == 1, "lastIndexOf 13 wrong")

// LIFO stack now
o.pop()
print (o)
o.pop()
print (o)
Debug.Assert(o[0] == 11, "Array content wrong_5")

print("\n*** Tests completed ***")
