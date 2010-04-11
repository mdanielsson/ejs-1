import System.Diagnostics;

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

// Define "serialize" method
function _serialize() {
	return "thisObject"
}
Object.prototype.serialize = _serialize

// Define "setProperty" method
function _setProperty(slot, value) {
}
Object.prototype.setProperty = _setProperty

// Define "sort" method
function _sort(funct) {
	var ret = new Array()
	for(var i in this) {
		ret.push(this[i])
	}
	return ret
}
Object.prototype.sort = _sort

// Define "toSource" method
function _toSource(depth, hidden, base) {
	return this.dump()
}
Object.prototype.toSource = _toSource

// Define "toString" method
function _toString() {
	return this.dump()
}
Object.prototype.toString = _toString

// Define "transform" method
function _transform(funct) {
	var arr = new Array()
	for(var i in this) {
		arr.push(funct(this[i]))
	}
	return arr
}
Object.prototype.transform = _transform

// Define "valueOf" method
function _valueOf() {
	return "anObject"
}
Object.prototype.valueOf = _valueOf

// Define "wait" method
function _wait(timeout) {
}
Object.prototype.wait = _wait

/*
 *	Test the "serialize" API
 */
var obj = new Object()
obj.one = 1
obj.two = 2
obj.three = 3

var res = obj.serialize()
Debug.Assert(res == "thisObject", "serialize failed")

/*
 *	Test the "setProperty" API
 */
obj.setProperty(1, "Red")

/*
 *	Test the "sort" API
 */
function testSortFunction(arg1, arg2) {
	if(arg1 > arg2) return 1
	if(arg1 < arg2) return -1
	return 0
}
res = obj.sort(testSortFunction)
print ("Sorted object: ", res)

/*
 *	Test the "toSource" API
 */
var ret = obj.toSource()
print ("toSource returned: ", ret)

/*
 *	Test the "toString" API
 */
ret = obj.toString()
print ("toString returned: ", ret)

/*
 *	Test the "transform" API
 */
function testTransformFunction(arg) {
	return arg + 1
}
ret = obj.transform(testTransformFunction)
print ("transform returned: ", ret)

/*
 *	Test the "valueOf" API
 */
ret = obj.valueOf()
print ("valueOf returned: ", ret)

/*
 *	Test the "wait" API
 */
obj.wait()


print("\n*** Tests completed ***")
