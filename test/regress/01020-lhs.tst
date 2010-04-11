/*
 *  Was failing in AST preventing function calls on LHS
 */
var o = {}
o["hello".toLower()] = 1

