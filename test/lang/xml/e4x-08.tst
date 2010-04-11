/*
    Test XML.toJSON
 */
var x = <Colors>
    <name intensity="bright">red</name>
    <name intensity="subdued">brown</name>
</Colors>

var s = x.toJSON()
assert(s == '"<Colors><name intensity=\\"bright\\">red</name><name intensity=\\"subdued\\">brown</name></Colors>"')
var o = deserialize(s)
assert(o is String)
x = XML(o)
assert(x is XML)
