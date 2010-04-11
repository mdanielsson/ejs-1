/*
 *  Fix regression where tenary conditions were not saving the "else" portion result correctly
 */
var x  = false ? true : "abc".toPascal()

assert(x == "Abc")
