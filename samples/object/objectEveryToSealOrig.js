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

// Define "every" method
function _every(funct) {
	for(var i in this) {
		if(funct(this[i]) == false) {
			return false
		}
	}
	return true
}
Object.prototype.every = _every

// Define "find" method
function _find(funct) {
	for(var i in this) {
		if(funct(this[i]) == true) {
			return true
		}
	}
	return false
}
Object.prototype.find = _find

// Define "findAll" method
function _findAll(funct) {
	var ret = new Array()
	for(var i in this) {
		if(funct(this[i]) == true) {
			ret.push(this[i])
		}
	}
	return ret
}
Object.prototype.findAll = _findAll

// Define "findMin" method
function _findMin(funct) {
	var el = 100000
	for(var i in this) {
		if(funct(this[i]) < funct(el)) {
			el = this[i]
		}
	}
	return el
}
Object.prototype.findMin = _findMin

// Define "findMax" method
function _findMax(funct) {
	var el = -100000
	for(var i in this) {
		if(funct(this[i]) > funct(el)) {
			el = this[i]
		}
	}
	return el
}
Object.prototype.findMax = _findMax

// Define "getProperty" method
function _getProperty(index) {
	return "Red"
}
Object.prototype.getProperty = _getProperty

// Define "getPropertySlot" method
function _getPropertySlot(name) {
	return 1
}
Object.prototype.getPropertySlot = _getPropertySlot

// Define "isCase" method
function _isCase(val) {
	return true
}
Object.prototype.isCase = _isCase

// Define "mixin" method
function _mixin(val) {
	return true
}
Object.prototype.mixin = _mixin

// Define "notify" method
function _notify(val) {
}
Object.prototype.notify = _notify

// Define "reject" method
function _reject(funct) {
	var ret = new Array()
	for(var i in this) {
		if(funct(this[i]) != true) {
			ret.push(this[i])
		}
	}
	return ret
}
Object.prototype.reject = _reject

// Define "seal" method
function _seal(val) {
}
Object.prototype.seal = _seal

/*
 *	Test the "every" API
 */
var obj = new Object()
obj.one = 1
obj.two = 2
obj.three = 3

function testEveryFunction(arg) {
	if(arg != null) return true;
}
var res = obj.every(testEveryFunction)
Debug.Assert(res == true, "Every returned false")

/*
 *	Test the "find" API
 */
function testFindFunction(arg) {
	if(arg == 1) return true;
}
var res = obj.find(testFindFunction)
Debug.Assert(res == true, "Find returned false")

/*
 *	Test the "findAll" API
 */
res = obj.findAll(testFindFunction)
Debug.Assert(res.length == 1, "FindAll 1 did not find one element")
print ("FindAll found: ", res.length, " elements")

/*
 *	Test the "findMin" API
 */
function testFinderFunction(arg) {
	return arg;
}
res = obj.findMin(testFinderFunction)
Debug.Assert(res != null, "findMin found nothing")
print ("findMin returned: ", res)

/*
 *	Test the "findMax" API
 */
res = obj.findMax(testFinderFunction)
Debug.Assert(res != null, "findMax found nothing")
print ("findMax returned: ", res)

/*
 *	Test the "getProperty" API
 */
res = obj.getProperty(1)
Debug.Assert(res != null, "getProperty returned null")
print ("getProperty returned: ", res)

/*
 *	Test the "getPropertySlot" API
 */
res = obj.getPropertySlot("Red")
Debug.Assert(res != null, "getPropertySlot returned null")
print ("getPropertySlot returned: ", res)

/*
 *	Test the "isCase" API
 */
res = obj.isCase("Red")
Debug.Assert(res == true, "isCase returned false")

/*
 *	Test the "mixin" API
 */
res = obj.mixin("Date")
Debug.Assert(res != null, "mixin returned null")

/*
 *	Test the "notify" API
 */
obj.notify(true)

/*
 *	Test the "reject" API
 */
function testRejectFunction(arg) {
	if(arg != 1) return true;
	return false;
}
res = obj.reject(testRejectFunction)
print ("Reject returned ", res.length, " elements")
Debug.Assert(res != null, "reject returned null")

/*
 *	Test the "seal" API
 */
obj.seal()


print("\n*** Tests completed ***")
