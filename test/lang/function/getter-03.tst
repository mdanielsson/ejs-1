x = 88
o = { 
	one: 1, 
	get fun () { return x; },
	set fun (value) { x = value; }
}

assert(o.fun == 88)
