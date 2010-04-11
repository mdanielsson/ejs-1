class Collection extends Object {

	var _duplicates : Boolean
	var _ordered: Boolean
	var _nulls : Boolean
	var _length : int = 0
	var _items : Array
	var _order : Array
	
	public function add(...arguments : Object[])
	{
		for (var i = 0; i < arguments.length; i += 2) {
			if (typeof(arguments[i + 1]) != 'undefined') {
				if (typeof(_items[arguments[i]]) == 'undefined') {
					_order[arguments[i]] = _length
					_length++;
				}
				_items[arguments[i]] = arguments[i + 1];				
			}
		}
	}

	public function clear() {
		_length = 0
		_items = new Array()
		_order = new Array()
	}
	
	public function Collection(duplicates, ordered, nulls)
	{
		_duplicates = duplicates
		_ordered = ordered
		_nulls = nulls
		_length = 0;
		_items = new Array()
		_order = new Array()
	}

	public function compact()
	{
		var newArray = new Array()
		var newOrder = new Array()
		_length = 0
		
		for (var i in _items) {
			if(_items[i] != null) {
				newArray[i] = _items[i]
				newOrder[i] = _length
				_length++
			}
		}
		_items = newArray
		_order = newOrder
	}
	
	public function concat(collection : Collection)
	{
		for (var i in collection._items) {
			this.add(i, collection._items[i])
		}
	}
	
	public function del(range : Range) {
		var inRange = false
		
		for (var i in _items) {
			if(i == range.start) inRange = true
			if(inRange) {
				_items[i] = null
			}
			if(i == range.end) inRange = false
		}
	}
		
	public function fill(obj : Object)
	{
		for (var i in _items) {
			_items[i] = obj
		}
	}
	
	public function first() : Object
	{
		for (var i in _items) {
			if(_length > 0) {
				_length--
			}
			return _items[i]
		}
	}
		
	public function get length(): int
	{
		return _length
	}
	
	public function join(sep : String) : String
	{
		var ret = ""
		for (var i in _items) {
			ret += i + sep + _items[i] + sep
		}
		return ret
	}
			
	public function last() : Object
	{
		for (var i in _items) {
		}
		return _items[i]
	}
				
	public function pop() : Object
	{
		var ret = null
		
		for (var i in _items) {
			ret = _items[i]
			break
		}
		this.remove(new Range(i, i))

		return ret
	}
		
	public function remove(range : Range) {
		var newArray = new Array()
		var newOrder = new Array()
		var inRange = false
		_length = 0
		
		for (var i in _items) {
			if(i == range.start) inRange = true
			if(!inRange) {
				newArray[i] = _items[i]
				newOrder[i] = length
				_length++
			}
			if(i == range.end) inRange = false
		}
		_items = newArray
		_order = newOrder
	}
	
	// Dummy - does nothing
	public function reverse()
	{
	}
	
	// Dummy - does nothing
	public function rotate(num : int)
	{
	}
	
	public function search(obj : Object) : Object
	{
		return _items[obj]
	}
		
	// Dummy - does nothing
	public function shuffle() : void
	{
	}
	
	public function slice(range : Range) : Collection
	{
		var newArray = new Array()
		var newOrder = new Array()
		var inRange = false
		_length = 0
		
		for (var i in _items) {
			if(i == range.start) inRange = true
			if(inRange) {
				newArray[i] = _items[i]
				newOrder[i] = length
				_length++
			}
			if(i == range.end) inRange = false
		}
		var newColl = new Collection(false, false, true)
		newColl._items = newArray
		newColl._order = newOrder
		return newColl
	}
	
	// Dummy - does nothing
	public function splice(start: Object, deleteCount: int, ...values : Object[]): Collection 
	{
		return null
	}
	
	public function swap(key1 : Object, key2 : Object) : void
	{
		var tmp = _items[key1]
		_items[key1] = _items[key2]
		_items[key2] = tmp
	}
	
	public function toString() : String
	{
		var ret = ""
		
		for (var i in _items) {
			ret += "\nkey is: " + i + ", value is: " + _items[i]
		}
		return ret
	}
	
	public function unique() : Collection
	{
		var newArray = new Array()
		var newOrder = new Array()
		var length = 0
		
		for (var i in _items) {
			var unique = true
			for (var j in newArray) {
				if(newArray[j] == _items[i]) {
					unique = false
					break
				}
			}
			
			if(unique == true) {
				newArray[i] = _items[i]
				newOrder[i] = length
				length++
			}

		}
		var newColl = new Collection(false, false, true)
		newColl._items = newArray
		newColl._order = newOrder
		newColl._length = length
		return newColl
	}
	
}

class Range extends Object {
	var _start
	var _end
	
	public function Range(start : Object, end : Object)
	{
		_start = start
		_end = end
	}
	
	public function get start() 
	{
		return _start
	}
	
	public function get end() 
	{
		return _end
	}
}

/*
 *	API tests
 */
 
print("\nvar coll = new Collection(false, false, true)")
var coll = new Collection(false, false, true)

print("\ncoll.add(\"key1\", \"value1\", \"key2\", \"value2\")")
coll.add("key1", "value1", "key2", "value2")
print("coll.length(): ", coll.length)

print("\ncoll.clear()")
coll.clear()
print("coll.length(): ", coll.length)

print("\ncoll.add(\"key1\", \"value1\", \"key2\", \"value2\", \"key3\", \"value3\")")
coll.add("key1", "value1", "key2", "value2", "key3", "value3")
print("coll.length(): ", coll.length)

print("\ncoll.add(\"key2\", null)")
coll.add("key2", null)
print("coll.length(): ", coll.length)
print("\ncoll.toString(): ", coll.toString())

print("\ncoll.compact()")
coll.compact()
print("coll.length(): ", coll.length)
print("\ncoll.toString(): ", coll.toString())

print("\nvar coll2 = new Collection(false, false, true)")
var coll2 = new Collection(false, false, true)
print("coll2.add(\"key4\", \"value1\", \"key5\", \"value2\")")
coll2.add("key4", "value1", "key5", "value2")
print("coll2.length(): ", coll2.length)
print("coll2.toString(): ", coll2.toString())

print("\ncoll.concat(coll2)")
coll.concat(coll2)
print("coll.length(): ", coll.length)
print("coll.toString(): ", coll.toString())

print("\ncoll.del(new Range(\"key3\", \"key4\"))")
coll.del(new Range("key3", "key4"))
print("coll.toString(): ", coll.toString())

print("\ncoll.fill(\"fillValue\")")
coll.fill("fillValue")
print("coll.toString(): ", coll.toString())

print("\ncoll.add(\"key1\", \"firstValue\")")
print("coll.add(\"key5\", \"lastValue\")")
coll.add("key1", "firstValue")
coll.add("key5", "lastValue")
print("\ncoll.toString(): ", coll.toString())

print("\ncoll.first() : ", coll.first())

print("\ncoll.join(\":\") : ", coll.join(":"))

print("\ncoll.last() : ", coll.last())

print("\ncoll.add(\"key1\", \"value1\", \"key2\", \"value2\", \"key3\", \"value3\")")
coll.add("key1", "value1", "key2", "value2", "key3", "value3")
print("\ncoll.toString(): ", coll.toString())
print("\ncoll.remove(new Range(\"key3\", \"key4\"))")
coll.remove(new Range("key3", "key4"))
print("coll.length(): ", coll.length)
print("coll.toString(): ", coll.toString())

print("\ncoll.pop() : ", coll.pop())
print("coll.length(): ", coll.length)
print("coll.toString(): ", coll.toString())

print("\ncoll.reverse() : dummy, does nothing")
coll.reverse()

print("\ncoll.rotate(1) : dummy, does nothing")
coll.rotate()

print("\ncoll.search(\"key5\") : ", coll.search("key5"))

print("\ncoll.shuffle() : dummy, does nothing")
coll.shuffle()

print("\ncoll.add(\"key1\", \"value1\", \"key2\", \"value2\", \"key3\", \"value3\")")
coll.add("key1", "value1", "key2", "value2", "key3", "value3")
print("coll.toString(): ", coll.toString())

print("\nvar new Coll = coll.slice(new Range(\"key3\", \"key4\"))")
var newColl = coll.slice(new Range("key3", "key4"))
print("newColl.toString(): ", newColl.toString())

print("\ncoll.splice() : dummy, does nothing")
coll.splice()

print("\ncoll.swap(\"key1\", \"key2\"))")
coll.swap("key1", "key2")
print("coll.toString(): ", coll.toString())

print("\ncoll.fill(\"fillValue\")")
coll.fill("fillValue")
print("coll.toString(): ", coll.toString())

print("\nvar coll3 = coll.unique()")
var coll3 = coll.unique()
print("coll3.toString(): ", coll3.toString())


print("\n*** Tests completed ***")
