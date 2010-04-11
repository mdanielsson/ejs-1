
/*
 *	Test namespaces qualification of types, functions and vars
 */
use strict

// public namespace DEBUG = "http://www.embedthis.com/ns/debug"
public namespace DEBUG = "debug_space"

DEBUG class Shape {
	DEBUG static function size(): Number {
		return 77
	}
}

use namespace DEBUG

assert(Shape.size() ==77)
