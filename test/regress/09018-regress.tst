/*
 *  Fix using a reserved namespace property via "this"
 */

dynamic class Shape {

    function fun() {
        var view;
        this.public::["x"] = 7
    }
}

var s = new Shape
s.fun()
assert(s.x == 7)
