/*
 *	Test memory exceptions
 */

/*
 *	Define a low memory maximum
 */

System.GC.tune(0, 10000);

var keep = new Array();					// Store all refs to defeat GC
try {
	for (var i = 0; i < 10240; i++) {
		keep[i] = new Object();
	} 
}
catch (error) {
	caught = 1;
	assert(error.name == "MemoryError");
}
finally {
	assert(caught == 1);
}

System.GC.tune(0, 0);

delete keep;

try { delete caught; } catch {}
