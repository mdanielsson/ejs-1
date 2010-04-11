/*
 *  Ensure stack residual cacluations are 
 */
var o = {}
for (i in 10) {
    /*
     *  Was crashing on the 2nd loop iteration because the iterator was being poped off the stack
     */
    Reflect(o).type
}


