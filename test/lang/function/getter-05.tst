/*
    Test object literal getters and setters
 */
var o = {
    shade: "red",
    get color() {
        return this.shade
    },
    set color(shade) {
        this.shade = shade
    },
    get determineColorFunction() {
        return function () {
            return this.shade
        }
    }
}

//  Test getter and getter that returns a function

assert(o.color == "red")
assert(o.determineColorFunction is Function)
assert(o.determineColorFunction() == "red")


//  Test setter and setter trying to invoke as a function

assert(o.shade == "red")
o.color = "blue"
assert(o.shade == "blue")

assert(o.color == "blue")

caught = false
try {
    o.color("yellow")
} catch (e) {
    caught = true
}
assert(caught)
