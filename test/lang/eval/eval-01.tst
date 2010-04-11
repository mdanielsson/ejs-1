/*
 *  Test eval()
 */

// 

eval("new Date")

//  Eval evaluates scripts into this interpreter
var x = 2
eval("x = 3")
assert(x == 3)

//  Test eval expression return
assert(eval("5 + 4") == 9)

//  BUG
//  assert(eval("x = 2") == 2)
