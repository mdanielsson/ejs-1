class Shape {
    function fun() {
        return function () {
            return 77
        }
    }
    function moreFun() {
        let f = function () {
            return 88
        }
        return f
    }
}

s = new Shape()
assert(s.fun()() == 77)
assert(s.moreFun()() == 88)
