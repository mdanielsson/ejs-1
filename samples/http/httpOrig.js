class Http extends Object {

	public static const HTTP_OKAY				: int	= 200

	var _connected : boolean
	var _requestProps : Array
	var _requestMethod : String
	
	public function addRequestProperty(key : String, val : String) : void
	{ 
		_requestProps[key] = val
	}

	public function get caching() : boolean
	{
		return false
	}

	public function connect(timeout : int) : int
	{ 
		_connected = true
		return Http.HTTP_OKAY
	}

	public function get connected() : boolean
	{
		return _connected
	}

	public function get contentEncoding() : String
	{
		return "text"
	}

	public function get requestMethod() : String
	{
		return _requestMethod
	}

	public function getRequestProperties() : Array
	{ 
		return _requestProps
	}

	public function Http(URL : String, input : Boolean, output 
		: Boolean, SSL : Boolean)
	{ 
		_requestProps = new Array()
		_requestProps["Url"] = URL
		_requestProps["Date"] = new Date()
		_requestMethod = "GET"
	}

	public function removeRequestProperty(key : String) : void
	{
		_requestProps[key] = null
	}

	public function printProps() : void
	{
		for(var i in props)
		{
			if((i == null) || (props[i] == null)) continue
			print("Prop : ", i, " is: ", props[i])
		}
	}

	public function setRequestMethod(request : String)
	{
		_requestMethod = request
	}

}

/*
 *	API tests
 */

print("\nCreate a new Http objectfor input only and no SSL")
print("var http = new Http(\"www.google.com\", false, true, false)")
var http = new Http("www.google.com", false, true, false)

print("\nhttp.addRequestProperty(\"Content-Type\", \"text\")")
http.addRequestProperty("Content-Type", "text")

print("\nvar props = http.getRequestProperties()")
var props = http.getRequestProperties()
http.printProps()

print("\nRemove a request property")
print("http.removeRequestProperty(\"Content-Type\")")
http.removeRequestProperty("Content-Type")
http.printProps()

print("\nConnect, http.connect(-1) returned: ", http.connect(-1))

print("Http using caching? http.caching = ", http.caching)
print("Http connected? http.connected = ", http.connected)
print("Http content encoding, http.contentEncoding = ", http.contentEncoding)

print("\nCurrent request method, http.requestMethod = ", http.requestMethod)

print("\nChange the request method, http.setRequestMethod(\"POST\")")
http.setRequestMethod("POST")

print("\nCurrent request method, http.requestMethod = ", http.requestMethod)


print("\n*** Tests completed ***")
