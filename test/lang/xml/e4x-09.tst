/*
    Construct from antoher XML
 */

function write(args) {
    print(args)
}

var xml: XML = <user>
        <hobby>
            painting
        </hobby>
    </user>

var x = XML(xml);
assert(x is XML)
assert(x == "<user><hobby>painting</hobby></user>")
