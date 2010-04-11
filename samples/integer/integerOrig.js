class Integer extends Object {
	var i : int
	
	function Integer(val : int) {
		this.i = val
	}


	// Define "abs" method
	function abs() {
		return Math.abs(this.i)
	}

	// Define (dummy) "ceil" method
	function ceil() {
		return Math.ceil(this.i)
	}

	// Define "downto" method
	function downto(num, f) {
		for(var x = this.i; x >= num; x--) {
			f(x)
		}	
	}

	// Define (dummy) "floor" method
	function floor() {
		return Math.floor(this.i)
	}

	// Define (dummy) "isFinite" method
	function isFinite() {
		return true
	}

	// Define (dummy) "isNaN" method
	function isNaN() {
		return false
	}

	// Define "max" method
	function max(arg) {
		return Math.max(this.i, arg)
	}

	// Define "min" method
	function min(arg) {
		return Math.min(this.i, arg)
	}

	// Define "power" method
	function power(arg) {
		return Math.pow(this.i, arg)
	}

	// Define (dummy) "round" method
	function round() {
		return this.i
	}

	// Define "step" method
	function step(limit, dist, f) {
		for(var x = this.i; x <= limit; x = x + dist) {
			f(x)
		}	
	}

	// Define "times" method
	function times(f) {
		if(this.i < 0) return
		for(var x = 0; x < this.i; x++) {
			f(x)
		}	
	}

	// Define (dummy) "toExponential" method
	function toExponential() {
		return "1.10000E+01"
	}

	// Define (dummy) "toFixed" method
	function toFixed() {
		return "11.0000"
	}

	// Define (dummy) "toPrecision" method
	function toPrecision() {
		return "11.000"
	}

	// Define (dummy) "toString" method
	function toString() {
		var ret : String = this.i
		return ret;
	}

	// Define "upto" method
	function upto(num, f) {
		for(var x = this.i; x <= num; x++) {
			f(x)
		}	
	}
}

/*
 *	Test the "abs" API
 */
var i = new Integer(-11)
print ("Absolute value: " + i.abs())

/*
 *	Test the "ceil" API
 */
print ("ceiling: " + i.ceil(5))

/*
 *	Test the "downto" API
 */
var i2 = new Integer(11)
i2.downto(5, function(arg) { print (arg) })

/*
 *	Test the "floor" API
 */
print ("floor: " + i.ceil(5))

/*
 *	Test the "isFinite" API
 */
print ("isFinite: " + i.isFinite())

/*
 *	Test the "isNaN" API
 */
print ("isNaN: " + i.isNaN())

/*
 *	Test the "max" API
 */
print ("max: " + i.max(20))

/*
 *	Test the "min" API
 */
print ("min: " + i.min(5))

/*
 *	Test the "round" API
 */
print ("round: " + i.round())

/*
 *	Test the "step" API
 */
print ("step: ")
i.step(2, 2, function(arg) { print (arg) })

/*
 *	Test the "times" API
 */
print ("times: ")
i2.times(function(arg) { print (arg) })

/*
 *	Test the "toExponential" API
 */
print ("toExponential: ", i2.toExponential(5))

/*
 *	Test the "toFixed" API
 */
print ("toFixed: ", i2.toFixed(4))

/*
 *	Test the "toPrecision" API
 */
print ("toPrecision: ", i2.toPrecision(3))

/*
 *	Test the "toString" API
 */
print ("toString: ", i2.toString())

/*
 *	Test the "upto" API
 */
print ("upto: ")
i2.upto(15, function(arg) { print (arg) })

print("\n*** Tests completed ***")
