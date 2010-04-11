/*
 *	Basic benchmarks
 */
function when() {
    return (new Date()).getTime()
}

/*
 *	For loops
 */
startMark = mark = when();
var count = 10000000;
for (i = 0; i < count; i++) {
	;
}
elapsed = when() - mark;
print("" + count + " for loop iterations: " + elapsed);


/*
 *	Parsing
 */
mark = when();
// include("bench2.js");
// include("bench2.js");
elapsed = when() - mark;
print("768000 statements:          " + elapsed);


/*
 *	Function calls
 */
var count = 4000000;
function bench(a, b)
{
	return a;
}
mark = when();
for (var i = 0; i < count; i++) {
	bench(i, i);
}
elapsed = when() - mark;
print("" + count + " function calls :     " + elapsed);

/*
 *	If / else creation
 */
var count = 3000000;
mark = when();
for (var i = 0; i < count; i++) {
	if (i % 2) {
		if (i / 2) {
			j = i;
		} else {
			j = i;
		}
	} else {
		if (i / 2) {
			j = i;
		} else {
			j = i;
		}
	}
}
elapsed = when() - mark;
print("" + count + " if/elses:            " + elapsed);


/*
 *	Object creation
 */
var count = 6000000;
mark = when();
for (var i = 0; i < count; i++) {
	x = new Object();
}
elapsed = when() - mark;
print("" + count + " object creations:    " + elapsed);


/*
 *	Property creation
 */
john = new Object();
var count = 200000;
mark = when();
for (var i = 0; i < count; i++) {
	john.age = 32;
	john.address = "22 Park Ave";
	john.married = true;
	john.smoker = false;
	john.dependents = 4;
	john.driver = true;
	john.height = 72;
	john.weight = 190;
}
elapsed = when() - mark;
print("" + count + " property creation:    " + elapsed);


/*
 *	Property access
 */
user = new Object();
user.peter = new Object();
user.peter.age = 32;
var count = 800000;
mark = when();
for (var i = 0; i < count; i++) {
	user.peter.age = 24;
	x = user.peter.age;
}
elapsed = when() - mark;
print("" + count + " property accesses:   " + elapsed);

print("\nTOTAL :                    " + (when() - startMark));
