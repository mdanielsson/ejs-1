/*
 *	GC -- Test deleted objects can be recovered.
 *
 *	Create lots of objects and then remove references so that GC should cleanup
 */

function abs(a)
{
	if (a < 0) {
		return -a;
	}
	return a;
}

/*
 *	Create lots of objects and keep references
 */
var gc = System.GC;
gc.run();
memoryMark = gc.getUsedMemory();

var keep = new Array();
for (var j = 0; j < 1024; j++) {
	keep[j] = new Object();
}

/*
 *	used should be > 80% of allocated at this point
 */
usedMemory = gc.getUsedMemory();
allocatedMemory = gc.getAllocatedMemory();
assert((allocatedMemory / (allocatedMemory - usedMemory)) > 5);

delete keep;
gc.run();

/*
 *	We should be back close to where we started within 10%
 */
delta = abs(memoryMark - gc.getUsedMemory());
assert((delta * 100 / gc.getAllocatedMemory()) < 10);

delete delta;
delete memoryMark;
delete usedMemory;
delete allocatedMemory;


