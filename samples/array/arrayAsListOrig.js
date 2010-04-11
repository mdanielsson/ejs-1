import System.Diagnostics;

// Define compact function
function _compact() {
	var newarr = [], ind = 0
	for (var ind2 = 0; ind2 < this.length; ind2++) {
		var value = this[ind2]
		if (value) {
			newarr[ind++] = value
		}
	}
	return newarr
}
Array.prototype.compact = _compact

// Define clear function
function _clear() {
	var x
	for (x in this) {
		this.shift()
	}
}
Array.prototype.clear = _clear

// Define insert function
function _insert(x, el) {
	this.splice(x, 0, el)
}
Array.prototype.insert = _insert

// Define unique function
function _unique() {
	return this
}
Array.prototype.unique = _unique

/*
 *	An array can be used as a list
 */
var o = new Array()

//	Add three elements to the list
o.unshift(9);
Debug.Assert(o.length == 1, "Array length wrong_1")
Debug.Assert(o[0] == 9, "Array content wrong_1")

o[1] = 7;
Debug.Assert(o.length == 2, "Array length wrong_2")
Debug.Assert(o[1] == 7, "Array content wrong_2")

o[3] = 3;
Debug.Assert(o.length == 4, "Array length wrong_3")
Debug.Assert(o[3] == 3, "Array content wrong_3")

print (o)

// Remove the zero'th element
var el = o.shift();
Debug.Assert(o.length == 3, "Array length wrong_4")
Debug.Assert(el == 9, "Value wrong_1")
print (el)

// Append a second list
var o2 = new Array()
o2.unshift(20)
o2.unshift(22)
print (o2)

var o3 = new Array()
o3 = o2.concat(o)
Debug.Assert(o3.length == 5, "Array length wrong_5")
print (o3)

// Compact the list to remove the null
o = o3.compact();
print("Finished compacting to length ", o.length)
Debug.Assert(o.length == 4, "Array length wrong_6")

// Insert an item into the array
o.insert(1, 10)
print (o);

// Slice out the first two elements
var sl = o.slice(1, 3)
Debug.Assert(sl.length == 2, "Slice array length wrong")
print ("Array slice: ", sl)

// Convert the array to a single string
print ("toString: ", o.toString())

// Concatenate all the elements into a single string
var str = o.join(":")
print ("join result: ", str)

// Print out the list of unique array elements
var uni = o.unique()
print ("uniques: ", uni)

// Empty the list
o.clear()
print("Finished clearing")
Debug.Assert(o.length == 0, "Array length wrong_7")

print("\n*** Tests completed ***")
