import System.Diagnostics;

class Range {
	var start
	var end
	function Range(start : int, end : int) {
		this.start = start
		this.end = end
	}
}

// Define (dummy) "printable" method
function _printable(s) {
	return new String(this)
}
String.prototype.printable = _printable

// Define "remove" method
function _remove(r) {
	var ret = new String()
	ret = this.slice(0, r.start)
	ret += this.slice(r.end, this.length - 1)
	return ret
}
String.prototype.remove = _remove

// Define (dummy) "reverse" method
function _reverse(s) {
	return "xoFdeRkciuQehT"
}
String.prototype.reverse = _reverse

// Define (dummy) "serialize" method
function _serialize(s) {
	return new String(this)
}
String.prototype.serialize = _serialize

// Define (dummy) "splitDelimiter" method
function _splitDelimiter(delim) {
	var arr = new Array()
	arr.push("Th")
	arr.push("QuickR")
	arr.push("dFox")
	return arr
}
String.prototype.splitDelimiter = _splitDelimiter

// Define "toLower" method
function _toLower() {
	return this.toLowerCase()
}
String.prototype.toLower = _toLower

// Define "toUpper" method
function _toUpper() {
	return this.toUpperCase()
}
String.prototype.toUpper = _toUpper

// Define (dummy) "trim" method
function _trim() {
	return this
}
String.prototype.trim = _trim



/*
 *	Test the "printable" API
 */
var str1 = new String("TheQuickRedFox")
print ("Printable: ", str1.printable())

/*
 *	Test the "remove" API
 */
var r = new Range(2, 4)
var str2 = str1.remove(r)
print ("After remove: ", str2)

/*
 *	Test the "replace" API
 */
var re = /The/g
var str3 = str1.replace(re, "Bob")
print ("After replace: ", str3)

/*
 *	Test the "reverse" API
 */
var str4 = str1.reverse()
print ("After reverse: ", str4)

/*
 *	Test the "serialize" API
 */
var str5 = str1.serialize()
print ("After serialize: ", str5)

/*
 *	Test the "split" API with reg exp
 */
re = /Red/g
var str6 = str1.split(re)
print ("After split: ", str6)

/*
 *	Test the "split" API with string delimiter
 */
var arr = str1.splitDelimiter("e")
print ("After splitDelimiter: ", arr)

/*
 *	Test the "toLower" API
 */
var str7 = str1.toLower()
print ("After toLower: ", str7)

/*
 *	Test the "toUpper" API
 */
str7 = str1.toUpper()
print ("After toUpper: ", str7)

/*
 *	Test the "trim" API
 */
str7 = str1.trim()
print ("After trim: ", str7)

print("\n*** Tests completed ***")
