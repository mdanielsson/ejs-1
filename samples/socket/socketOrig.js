class Socket extends Object {

	var _address : String
	var _port : int
	var _data : Array
	var _connected : boolean
	
	public function Socket()
	{
		_address = "localhost"
		_port = 80
		_data = new Array()
	}
	
	public function available() : int
	{
		return _data.length
	}

	public function bind(address : String, port : int) : void
	{
		_address = address
		_port = port
	}

	public function connect(timeout : int) : boolean
	{
		_connected = true
		return true
	}

	public function get address() : String 
	{ 
		return _address
	}

	public function get port() : int 
	{ 
		return _port
	}
	
	public function send(buffer : Array, size : int, timeout : int) : void
	{
		_data = buffer
	}

	public function read(buffer : Array, number : int, timeout : int) : void
	{
	}


}

/*
 *	API tests
 */

print("\nCreate a new socket")
print("var sock = new Socket()")
var sock = new Socket()

print("\nAddress is: ", sock.address, " port is: ", sock.port)

print("\nsock.bind(\"192.0.0.1\", 1083)")
sock.bind("192.0.0.1", 1083)

print("Address is: ", sock.address, " port is: ", sock.port)

print("\nNumber of bytes available, sock.available() returns: ", sock.available())

print("\nConnect to the remote socket, sock.connect(0) returns: ", sock.connect(0))

print("\nSend a message")
print("var arr = new Array(\"This\", \"is\", \"a\", \"test\", \"message\")")
print("sock.send(arr, arr.length, 0)")
var arr = new Array("This", "is", "a", "test", "message")
sock.send(arr, arr.length, 0)

print("\nNumber of bytes available, sock.available() returns: ", sock.available())

print("\nRead the same message back")
print("sock.read(arr, -1, 0)")
sock.read(arr, -1, 0)
for(var i = 0; i < arr.length; i++)
{
	print("Read: ", arr[i])
}


print("\n*** Tests completed ***")
