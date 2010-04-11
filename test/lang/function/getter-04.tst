/*
    Ensure getters can't be invoked as functions
 */
class Shape {
    var shade = "red"
    function get color() {
        return shade
    }
    function set color(shade) {
        this.shade = shade
    }
    function get determineColorFunction() {
        return function () {
            return shade
        }
    }
}

var s: Shape = new Shape

//  Test getter and getter that returns a function

assert(s.color == "red")
assert(s.determineColorFunction is Function)
assert(s.determineColorFunction() == "red")


//  Test setter and setter trying to invoke as a function

s.color = "blue"
assert(s.color == "blue")

caught = false
try {
    s.color("yellow")
} catch (e) {
    caught = true
}
assert(caught)
