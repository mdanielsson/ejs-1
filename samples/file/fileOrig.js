class File extends Object {

	public static const READ : int			= 1
	public static const WRITE : int			= 2
	public static const READ_WRITE : int	= 3
	
	var _name : String
	var _raw : Boolean
	var _length : int = -1
	var _touched : Date = null
	var _readable : Boolean
	var _permissions : int
	var _last : Date = null
	
	public function create(name : String, raw : Boolean, permissions : int) : void
	{
		_raw = raw
		_permissions = permissions
		_length = 0
		_touched = new Date()
	}
	
	public function get exists() : Boolean
	{
		if(_length < 0) return false
		return true
	}

	public function get last() : Date
	{
		return _last
	}

	public function get length() : int
	{
		return _length
	}

	public function File(name : String)
	{
		_name = name
	}
	
	public function get name() : String
	{
		return _name
	}

	public function get readable() : Boolean
	{
		if((_permissions & File.READ) > 0) return true
		return false
	}

	public function get touched() : Date
	{
		return _touched
	}

	public function get writable() : Boolean
	{
		if((_permissions & File.WRITE) > 0) return true
		return false
	}

	public function read(obj : Object) : int
	{
		_last = new Date()
		return 0
	}

	public function seek(num : int) : Boolean
	{
		return true
	}

	public function truncate(num : int) : void
	{
		_touched = new Date()
	}

	public function write(obj : Object) : int
	{
		return (0)
	}
}

/*
 *	API tests
 */
 
var f = new File("test.txt")
print("\nvar f = new File(\"test.txt\")")
print("f.name = ", f.name)
print("f.touched = ", f.touched)
print("f.length = ", f.length)

print("\nf.create(\"test.txt\", false, File.READ_WRITE)")
f.create("test.txt", false, File.READ_WRITE)

print("f.touched = ", f.touched)
print("f.length = ", f.length)
print("f.readable = ", f.readable)
print("f.writable = ", f.writable)

print("\nf.last = ", f.last)
print("f.read(null)")
f.read(null)
print("f.last = ", f.last)

print("\nf.seek(0) = ", f.seek(0))

print("\nf.truncate(100) = ")
f.truncate(100)

print("\nvar arr = new Array()")
var arr = new Array()
print("f.write(arr) = ", f.write(arr))


print("\n*** Tests completed ***")
