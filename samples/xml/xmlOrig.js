class XML extends Object {

	var _localName : String
	var _properties : Array
	var _parent : XML
	var _value : Object
	
	/*	Ignoring comments, ignoring processing instruction,
	 *	ignoring white space and pretty printing and indenting
	 */
	var _defaultSettings : Array
	
	/* Types: element, attribute, comment, processing-instruction or text */
	var _type : String
	
	static var _indent : int = 0
	
	public function XML(name : String, value : Object, xmlType : String) 
	{
		_localName = name;
		_type = xmlType
		_value = value
		_properties = new Array()
		_defaultSettings = new Array()
		_defaultSettings["_ignoreComments"] = true
		_defaultSettings["_ignoreProcInst"] = true
		_defaultSettings["_ignoreWhiteSpace"] = true
		_defaultSettings["_prettyPrint"] = true
		_defaultSettings["_indent"] = true
	}
	
	public function appendChild(child : XML) : XML
	{
		_properties.push(child)
		child._parent = this
		return child
	}

	public function attribute(name : String) : XMLList
	{
		var ret = new XMLList("tmp")
		for (var i in _properties) 
		{
			if((_properties[i]._localName == name) && (_properties[i]._type == "attribute")) 
			{
				ret.add(_properties[i])
			}
		}
		return ret
	}

	public function attributes() : XMLList
	{
		return helper("attribute") 
	}

	public function helper(str : String) : XMLList
	{
		var ret = new XMLList("tmp")
		for (var i in _properties) 
		{
			if(_properties[i]._type == str) 
			{
				ret.add(_properties[i])
			}
		}
		return ret
	}


	public function child(name : String) : XMLList
	{
		var ret = new XMLList("tmp")
		for (var i in _properties) 
		{
			if(_properties[i]._localName == name) 
			{
				ret.add(_properties[i])
			}
		}
		return ret
	}

	// Dummy method
	public function childIndex() : Number
	{
		return 0
	}

	public function children() : XMLList
	{
		var ret = new XMLList("tmp")
		for (var i in _properties) 
		{
			ret.add(_properties[i])
		}
		return ret
	}

	public function comments() : XMLList
	{
		return helper("comment") 
	}

	// Dummy method
	public function contains(obj : Object) : Boolean
	{
		return true
	}

	public function copy() : XML
	{
		var newXml = new XML(this._localName, this._value, this._type)
		for (var i in _properties)
		{
			newXml.appendChild(_properties[i])
		}
		return newXml
	}

	public function defaultSettings() : Object
	{
		return _defaultSettings
	}

	public function descendants(name : String) : Object
	{
		var ret = new XMLList("tmp")
		for (var i in _properties) 
		{
			if((_properties[i]._localName == name) && (_properties[i]._type == "element"))
			{
				ret.add(_properties[i])
				ret.add(i.descendants(name))
			}
		}
		return ret
	}

	public function elements() : XMLList
	{
		return helper("element") 
	}

	public function hasOwnProperty(name : String) : Boolean
	{
		var list = child(name)
		if(list.length() == 0) {
			return false
		}
		return true
	}

	public function hasComplexContent() : Boolean
	{
		var list = elements()
		if(list.length() == 0) {
			return false
		}
		return true
	}

	public function hasSimpleContent() : Boolean
	{
		return !hasComplexContent()
	}
	
	// Dummy method
	public function insertChildAfter(marker : Object, child : Object) : XML
	{
		return this
	}

	// Dummy method
	public function insertChildBefore(marker : Object, child : Object) : XML
	{
		return this
	}

	public function length() : int
	{
		return 1
	}

	public function localName() : String
	{
		return _localName
	}

	// Just returns local name for now
	public function name() : String
	{
		return _localName
	}

	public function nodeKind() : String
	{
		return _type
	}

	// Dummy method
	public function normalize() : XML
	{
		return this
	}

	public function parent() : XML
	{
		return _parent
	}

	// Dummy method
	public function prependChild(child : Object) : XML
	{
		return this
	}

	public function processingInstructions(name : String) : XMLList
	{
		var ret = new XMLList("tmp")
		for (var i in _properties) 
		{
			if((_properties[i]._localName == name) && (_properties[i]._type == "processing-instruction")) 
			{
				ret.add(_properties[i])
			}
		}
		return ret
	}

	// Dummy method
	public function propertyIsEnumerable(property : Object) : Boolean
	{
		return true
	}

	public function replace(property : Object, value : Object) : void
	{
		for (var i in _properties)
		{
			if(property == _properties[i]) 
			{
				_properties[i]._value = value
			}
		}
	}

	public function setChildren(properties : Object) : XML
	{
		_properties = new Array()
		appendChild(properties)
		return this
	}

	public function setLocalName(name : String) : void
	{
		_localName = name
	}

	// Sets local name
	public function setName(name : String) : void
	{
		_localName = name
	}

	public function settings() : Object
	{
		return _defaultSettings
	}

	public function setSettings(settings : Object) : void
	{
		_defaultSettings = settings
	}

	public function text(name : String) : XMLList
	{
		var ret = new XMLList("tmp")
		for (var i in _properties) 
		{
			if((_properties[i]._localName == name) && (_properties[i]._type == "text")) 
			{
				ret.add(_properties[i])
			}
		}
		return ret
	}

	public function toString() : String
	{
		var ret = "Name: " + _localName + " Type: " + _type + " Value: " + _value
		_indent++
		
		for (var i in _properties)
		{
			if(_indent > 0) ret += "\n"
			for(var j = 0; j < _indent; j++)
			{
				ret += "   "
			}
			ret += "property: " + _properties[i].toString()
		}
		_indent--
		return ret
	}

	// Pseudo-XML
	public function toXMLString() : String 
	{
		var ret = "<XML><localName>" + _localName + "</localName><type>" + _type + "</type><value>" + _value + "</value>"
		
		for (var i in _properties)
		{
			ret += "<property><" + _properties[i].toString() + "</property>"
		}
		ret += "</XML>"
		return ret
	}

	public function valueOf() : XML 
	{
		return this
	}
	
}

class XMLList extends Object {

	var _localName : String
	var _properties : Array
	
	public function XMLList(name : String) 
	{
		_localName = name
		_properties = new Array()
	}
	
	public function add(obj : XML) : void
	{
		_properties.push(obj)
	}
	
	public function length() : int
	{
		return _properties.length
	}
	
	public function toString() : String
	{
		var ret = "Name: " + _localName
		
		for (var i in _properties)
		{
			ret += "\n" + "   member: " + _properties[i].toString()
		}
		return ret
	}
}

/*
 *	Test the "Xml" API
 */

print("\nCreate the top level XML node")
print("var xml = new XML(\"top\", \"\", \"element\")")
var xml = new XML("top", "", "element")
print("var xml = ", xml.toString())

print("\nAdd a child attribute")
print("var child = new XML(\"child1\", \"\", \"attribute\")")
var child = new XML("child1", "", "attribute")
print("xml.appendChild(child)")
xml.appendChild(child)
print("xml = ", xml.toString())

print("\nCheck for a named attribute")
print("var attrib = xml.attribute(\"animal\")")
var attribs = xml.attribute("animal")
print("attributes: ", attribs.toString())

print("\nAdd a named attribute")
print("var child2 = new XML(\"animal\", \"donkey\", \"attribute\")")
var child2 = new XML("animal", "donkey", "attribute")
print("xml.appendChild(child2)")
xml.appendChild(child2)
print("xml = ", xml.toString())

print("\nCheck again for the named attribute")
print("attribs = xml.attribute(\"animal\")")
attribs = xml.attribute("animal")
print("attribute: ", attribs.toString())

print("\nAdd a text property")
print("var child3 = new XML(\"A text property\", \"This is the text\", \"text\")")
var child3 = new XML("A text property", "This is the text", "text")
print("xml.appendChild(child3)")
xml.appendChild(child3)
print("xml = ", xml.toString())

print("\nShow all attributes")
print("attribs = xml.attributes()")
attribs = xml.attributes()
print("attributes: ", attribs.toString())

print("\nAll children with this name")
print("child = xml.child(\"child1\")")
var child = xml.child("child1")
print("List: ", child.toString())

print("\nIndex of the first child")
print("xml.childIndex(\"child1\")")
print("Position: ", xml.childIndex("child1"))

print("\nAll children")
print("var children = xml.children()")
var children = xml.children()
print("List: ", children.toString())

print("\nAdd some comment properties")
child3 = new XML("A comment", "This is a comment", "comment")
xml.appendChild(child3)
child3 = new XML("Another comment", "This is a comment", "comment")
xml.appendChild(child3)
child3 = new XML("Third comment", "This is a comment", "comment")
xml.appendChild(child3)
child3 = new XML("Fourth comment", "This is a comment", "comment")
xml.appendChild(child3)

print("\nShow all child comments")
print("var comments = xml.comments()")
var comments = xml.comments()
print("comments: ", comments.toString())

print("\nxml.contains(child3)")
print("contains? ", xml.contains(child3))

print("\nDeep copy a node")
print("var newNode = xml.copy()")
var newNode = xml.copy()
print("newNode: ", newNode.toString())

print("\nShow default settings")
print("var defaultSettings = xml.defaultSettings()")
var defaultSettings = xml.defaultSettings()
for(var i in defaultSettings)
{
	print("Setting: ", i, " : value ", defaultSettings[i])
}

print("\nAdd child and grand child elements")
var child = new XML("A child element", "", "element")
var grandchild = new XML("A grandchild element", "", "element")
xml.appendChild(child)
child.appendChild(grandchild)
xml._indent = 0
print("xml node: ", xml.toString())

print("\nShow all child elements")
print("var elements = xml.elements()")
var elements = xml.elements()
print("elements: ", elements.toString())

print("\nTest if has a property of the given name")
print("xml.hasOwnProperty(\"apple\") : ", xml.hasOwnProperty("apple"))
print("xml.hasOwnProperty(\"animal\") : ", xml.hasOwnProperty("animal"))

print("\nTest if has complex content")
print("child.hasComplexContent() : ", child.hasComplexContent())
print("grandchild.hasComplexContent() : ", grandchild.hasComplexContent())

print("\nTest if has simple content")
print("child.hasSimpleContent() : ", child.hasSimpleContent())
print("grandchild.hasSimpleContent() : ", grandchild.hasSimpleContent())

print("\nInsert a child after the marker")
print("var childAfter = new XML(\"childAfter\", \"\", \"element\")")
var childAfter = new XML("childAfter", "", "element")
print("xml.insertChildAfter(child, childAfter)")
xml.insertChildAfter(child, childAfter)
print("Dummy method - xml node does not change")

print("\nInsert a child before the marker")
print("var childBefore = new XML(\"childBefore\", \"\", \"element\")")
var childBefore = new XML("childBefore", "", "element")
print("xml.insertChildBefore(child, childBefore)")
xml.insertChildBefore(child, childBefore)
print("Dummy method - xml node does not change")

print("\nLength of this node")
print("child.length() : ", child.length())

print("\nLocal name of this node")
print("child.localName() : ", child.localName())

print("\nName of this node")
print("Dummy method - set to equal localName")
print("child.name() : ", child.name())

print("\nKind of node this is")
print("child.nodeKind() : ", child.nodeKind())

print("\nCollapse text nodes")
print("Dummy method - does nothing")
print("child.normalize() : ", child.normalize())

print("\nGet parent node")
print("child.parent().toString() : \n", child.parent().toString())

print("\nInsert a child at the beginning")
print("var prepend = new XML(\"prepend\", \"\", \"element\")")
var prepend = new XML("prepend", "", "element")
print("xml.prependChild(prepend)")
xml.prependChild(prepend)
print("Dummy method - xml node does not change")

print("\nAdd a processing instruction child")
var child4 = new XML("Processing instruction 1", "an instruction", "processing-instruction")
xml.appendChild(child4)

print("Show all child processing-instructions")
print("var procs = xml.processingInstructions(\"Processing instruction 1\")")
var procs = xml.processingInstructions("Processing instruction 1")
print("processingInstructions: ", procs.toString())

print("\nTest if this property is enumerable")
print("Dummy method - always returns true")
print("child.propertyIsEnumerable(\"animal\") : ", child.propertyIsEnumerable("animal"))

print("\nChange the value of a property")
print("xml.replace(child2, \"mule\")")
xml.replace(child2, "mule")
print("xml node: ", xml.toString())

print("\nReplace all the properties")
print("xml.setChildren(child2)")
xml.setChildren(child2)
print("xml node: ", xml.toString())

print("\nSet local name and name (which sets local name)")
print("Local name: ", xml.localName(), " Name: ", xml.name())
xml.setLocalName("bug")
print("Local name: ", xml.localName(), " Name: ", xml.name())

print("\nGet the settings (returns defaultSettings)")
print("Settings: \n")
settings = xml.settings()
for(var i in settings) {
	print(i, ":", settings[i])
}

print("\nSet the settings (sets the default settings)")
var settings = new Array()
settings["_ignoreComments"] = false
settings["_ignoreProcInst"] = false
settings["_ignoreWhiteSpace"] = false
settings["_prettyPrint"] = false
settings["_indent"] = false
xml.setSettings(settings)

print("\nGet the settings (returns defaultSettings)")
print("Settings: \n")
settings = xml.settings()
for(var i in settings) {
	print(i, ":", settings[i])
}

print("\nAdd some text properties")
child3 = new XML("A text property", "The text property value", "text")
xml.appendChild(child3)
child3 = new XML("A second text property", "The second text property value", "text")
xml.appendChild(child3)
print("xml node: ", xml.toString())

print("\nOutput as (pseudo) XML: ", xml.toXMLString())

print("\nValueOf just returns the node: ", xml.valueOf().toString())



