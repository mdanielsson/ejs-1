/*
 *  This creates lots of objects and memory. Can be used to test maxMemory limits
 */

// GC.maxMemory = 50 * 1024 * 1024

/*
 *  Set this to 7 for very big, 8 is huge
 */
var DEPTH = 7

public class Tree {
    private var _children: Array
    private var _jsResource: Object

    function Tree(name: String) {
        print("Tree node :" + name)
        _children = []
        _jsResource = {}
        _jsResource.resource = {}
        _jsResource.resource.name = name
     }

     function generateNChildren(depth: Number) {
        if (depth > 0) {
            for (i = 0; i < depth; i++) {
                name = _jsResource.resource.name+"." + i
                child = new Tree(name)
                child.generateNChildren(depth - 1)
                _children.append(child)
            }
        }
    }

    function getJSObject(): Object {
        _jsResource.resource.children = []
        if (_children.length > 0) {
            for each (var s in _children) {
                _jsResource.resource.children.append(s.getJSObject())
            }
        }
       return _jsResource
     }
}

public class TestController {

    function TestController() {
        root = new Tree("root")
        root.generateNChildren(DEPTH)

        jsonResult = JSON.stringify(root.getJSObject())
        print(jsonResult)

        xmlResult = Object2Xml(root.getJSObject())
        print(xmlResult)

        print(jsonResult)
    }

    function Object2Xml(obj) {
        var result = ""
       if (obj is String) {
            result = obj.replace(/&/g,'&amp;').replace(/\</g,'&lt;').replace(/\>/g,'&gt;').replace(/\'/g,'&apos;').replace(/"/g,'&quot;')

       } else if (typeof obj === "object") {
            for (var s in obj) {
                var value = Object2Xml(obj[s])
                if (!value) {
                    return false
                }
                if (obj instanceof Array) {
                    result += value
                } else {
                    result += "<" + s + ">" + value + "</" + s + ">"
                }
           }
        } else if (obj.toString) {
            result = obj.toString()
print("TO STRING " + result)

        } else {
            return false
        }
        return result
    }
}

t = new TestController
