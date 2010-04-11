/*
 *  Test that closures are correctly created for block nested outer functions
 */
{
    use namespace "blue"

    function fun(a) {
        return function inner() { return a; }
    }
    a = fun(1)
    assert(a() == 1)
}
