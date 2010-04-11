import System.Diagnostics;

// Define (dummy) "ln" method
function _ln() {
	return 1
}
Object.prototype.ln = _ln


/*
 *	API tests
 */
var res = Math.acos(0)
print("Math.acos(0) = ", res)

res = Math.asin(0)
print("Math.asin(0) = ", res)

res = Math.atan(1)
print("Math.atan(1) = ", res / 3.14159263 * 180, " degrees")

res = Math.ln(2.71828183)
print("Math.ln(2.71828183) = ", res)

res = Math.log(10)
print("Math.log(10) = ", res)

res = Math.random()
print("Math.random() = ", res)

res = Math.sin(0)
print("Math.sin(0) = ", res)

res = Math.sqrt(16)
print("Math.sqrt(16) = ", res)

res = Math.tan(Math.PI/4.0)
print("Math.tan(Math.PI/4.0) = ", res)


print("\n*** Tests completed ***")
