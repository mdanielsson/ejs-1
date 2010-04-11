// 

/*
 *  Test various scenarios which test bound and unbound accesse
 */
class Shape {
    var origin = 1

    public function getOrigin() {
        return origin
    }

    public static function getName() {
        return "Shape"
    }
}

dynamic class Rectangle {
}

//  Use an untyped arg to mask the type
function mask(o) {
    return o.getOrigin()
}


var s: Shape = new Shape
//  Bound access
assert(s.origin == 1)
assert(s.getName() == "Shape")

s2 = new Shape
//  Unbound access
assert(s2.origin == 1)
assert(s2.getName() == "Shape")

//  Mask type and invoke 
assert(mask(s) == 1)
