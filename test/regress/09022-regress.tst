/*
 *  Embedded comment in XML 
 */


/* This caused a crash */
x = new XML("<x><!-- Demo comment --><a>aaa</a></x>")
assert(x.a == "aaa")
