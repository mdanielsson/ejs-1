/*
 *	Test lots of objects
 */

for (var i = 0; i < 10240; i++) {
	var x = new Object();
}

System.GC.run();
delete x;
System.GC.run();
