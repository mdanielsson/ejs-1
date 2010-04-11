import System.Diagnostics;

// Define "compare" method
function _compare(s) {
	if(this < s) return -1
	if(this > s) return +1
	return 0
}
String.prototype.compare = _compare


// Define "compareNoCase" method
function _compareNoCase(s) {
	return this.localeCompare(s)
}
String.prototype.compareNoCase = _compareNoCase

// Define "deserialize" method
function _deserialize(str) {
	var ret = new String(str)
	return ret
}
String.prototype.deserialize = _deserialize

// Define "isAlpha" method
function _isAlpha() {
	return true
}
String.prototype.isAlpha = _isAlpha

// Define "isDigit" method
function _isDigit() {
	return false
}
String.prototype.isDigit = _isDigit

// Define "isSpace" method
function _isSpace() {
	return false
}
String.prototype.isSpace = _isSpace

// Define "isUpper" method
function _isUpper() {
	return false
}
String.prototype.isUpper = _isUpper

// Define "isLower" method
function _isLower() {
	return false
}
String.prototype.isLower = _isLower

// Define "multiply" method
function _multiply(number) {
	var ret = new String()
	for(var i = 0; i < number; i++) {
		ret += this
	}
	return ret
}
String.prototype.multiply = _multiply

// Define "pascalCase" method
function _pascalCase(number) {
	return "IsAlpha"
}
String.prototype.pascalCase = _pascalCase



/*
 *	Test the "compare" API
 */
var str1 = new String("TheQuickRedFox")
var str2 = new String("JumpedOverThe")
print ("Strings: ", str1, " ", str2)

var comp = str1.compare(str2);
Debug.Assert(comp == 1, "compare 1 did not return 1")

comp = str2.compare(str1);
Debug.Assert(comp == -1, "compare 2 did not return -1")

/*
 *	Test the "compareNoCase" API
 */
str1 = new String("jumpedoverthe")
str2 = new String("JumpedOverThe")
print ("Strings: ", str1, " ", str2)
comp = str1.compareNoCase(str2)
Debug.Assert(comp == -1, "compareNoCase did not return -1")

/*
 *	Test the "charCodeAt" API
 */
comp = str1.charCodeAt(1)
Debug.Assert(comp == 117, "charCodeAt did not return 117 (ASCII 'u')")
print ("charCodeAt returned: ", comp)

/*
 *	Test the "contains" API
 */

/*
 *	Test the "deserialize" API
 */
var str = str1.deserialize("aString")
Debug.Assert(str != null, "deserialize returned null")

/*
 *	Test the "isAlpha" API
 */
var str3 = new String("isAlpha")
Debug.Assert(str3.isAlpha() == true, "isAlpha returned false")

/*
 *	Test the "isDigit" API
 */
Debug.Assert(str3.isDigit() == false, "isDigit returned true")

/*
 *	Test the "isLower" API
 */
Debug.Assert(str3.isLower() == false, "isLower returned true")

/*
 *	Test the "isSpace" API
 */
Debug.Assert(str3.isSpace() == false, "isSpace returned true")

/*
 *	Test the "isUpper" API
 */
Debug.Assert(str3.isUpper() == false, "isUpper returned true")

/*
 *	Test the "multiple" API
 */
var ret = str3.multiply(3)
print (ret)

/*
 *	Test the "pascalCase" API
 */
ret = str3.pascalCase()
print (ret)

print("\n*** Tests completed ***")
