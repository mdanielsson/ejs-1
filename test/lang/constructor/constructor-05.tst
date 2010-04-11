// 
/*
 *	Calling types as constructors
 */

d = Date()
assert(d is Date)

now = Date().now()
assert(now is Number && now > 0)
    
