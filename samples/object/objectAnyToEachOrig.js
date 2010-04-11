import System.Diagnostics;

// Define "any" method
function _any(f) {
	for(var i in this) {
		print (i, ": ", this[i])
		var j = f(this[i])
		if(j == true) return true
	}
}
Object.prototype.any = _any

// Define "dump" method
function _dump() {
	var out = new String()
	for(var i in this) {
		out += i.toString() + ": "
		out += this[i].toString() + "\n"
	}
	return out
}
Object.prototype.dump = _dump

// Define "call" method
function _call(slot) {
	print ("Call number of args: ", arguments.length)
	return true
}
Object.prototype.call = _call

// Define "cast" method
function _cast(type) {
	if(type == "String") {
		return new String()
	}
	return null
}
Object.prototype.cast = _cast

// Define "clone" method
function _clone() {
	return true
}
Object.prototype.clone = _clone

// Define "deepCopy" method
function _deepCopy() {
	return true
}
Object.prototype.deepCopy = _deepCopy

// Define "deleteProperty" method
function _deleteProperty(index) {
	delete obj.one
}
Object.prototype.deleteProperty = _deleteProperty

// Define "deserialize" method
function _deserialize(str) {
	var ret = new String(str)
	return ret
}
Object.prototype.deserialize = _deserialize

// Define "each" method
function _each(funct) {
	for(var i in this) {
		this[i] = funct(this[i])
	}
}
Object.prototype.each = _each

/*
 *	Test the "any" API
 */
var obj = new Object()
obj.one = 1
obj.two = 2
obj.three = 3
obj.eight = 8

function testAnyFunction(arg) {
	if(arg > 5) return true;
}
var res = obj.any(testAnyFunction)

Debug.Assert(res == true, "Any returned false")

/*
 *	Test the "call" API
 */
res = obj.call(0)
Debug.Assert(res == true, "Call-1 returned false")
res = obj.call(1, "arg1", "arg2")
Debug.Assert(res == true, "Call-2 returned false")

/*
 *	Test the "cast" API
 */
res = obj.cast("String")
Debug.Assert(res instanceof String, "Cast did not return a String")

// This should fail with the locally defined cast method
res = obj.cast("Date")
Debug.Assert((res instanceof Date) == false, "Cast did not return a Date")

/*
 *	Test the "clone" API
 */
res = obj.clone()
Debug.Assert(res != null, "Clone returned null")

/*
 *	Test the "deepCopy" API
 */
res = obj.deepCopy()
Debug.Assert(res != null, "deepCopy returned null")

/*
 *	Test the "deleteProperty" API
 */
print ("Obj before deleting property: ")
print (obj.dump())
obj.deleteProperty(0)
print ("Obj after deleting property: ")
print (obj.dump())

/*
 *	Test the "deserialize" API
 */
var str = obj.deserialize("aString")
Debug.Assert(str != null, "deserialize returned null")

/*
 *	Test the "each" API
 */
function testEachFunction(arg) {
	return arg + 1;
}
var arr = new Array(1, 3, 5)
print ("Array before each method: ", arr)
arr.each(testEachFunction)
print ("Array after each method: ",arr)


print("\n*** Tests completed ***")
