
/*
 *	The MS regular expressions (RE) class implements an API fairly close
 *	to the EcmaScript 4.0 proposed standard. The EJB RE class implements
 *	those API(s) and also the RE API(s) that the standard String class
 *	EcmaScript provides.
 */
public class RegExp2 
{
	var _re : RegExp
	var _pattern : String
	var _lastMatch : Array
	
	public function end() : int
	{
		if(_lastMatch != null)
		{
			return _lastMatch["lastIndex"]
		}
		return -1
	}

	public function exec(str : String) : Array
	{
		var ret = new Array()
		_lastMatch = _re.exec(str)
		return _lastMatch
	}
	
	public function matched() : String
	{
		if(_lastMatch != null)
		{
			return _lastMatch["input"]
		}
		return ""
	}
	
	public function get pattern() : String
	{
		return _pattern
	}

	public function RegExp2(pattern : String)
	{
		_re = new RegExp(pattern, "g")
		_pattern = pattern
	}
	
	public function replace(input : String, replacement : String) : String
	{
		return input.replace(_re, replacement)
	}
	
	public function split(input : String) : Array
	{
		return input.split(_re)
	}
	
	public function setPattern(pattern : String)
	{
		_re = new RegExp(pattern, "g")
		_pattern = pattern
	}
	
	public function start() : int
	{
		if(_lastMatch != null)
		{
			return _lastMatch["index"]
		}
		return -1
	}

	public function test(str : String) : Boolean
	{
		if(exec(str) == null)
		{
			return false
		}
		return true
	}
	
		
}

/*
 *	API tests
 */
print("\nCreate a new RegExp object")
print("var re = new RegExp2(\"s\")")
var re = new RegExp2("s")

print("\nvar arr = re.exec(\"Does this match anything?\")")
var arr = re.exec("Does this match anything?")
print(arr.index.toString(), "-", arr.lastIndex.toString(), "\t", arr.toString())

print("\nReg exp returned array properties")
for(var i in arr)
{
	print("Prop: ", i, " Value: ", arr[i])
}

print("\nThe starting index")
print("var start = re.start() = ", re.start().toString())

print("\nThe index one beyond the last match")
print("var end = re.end() = ", re.end().toString())

print("\nCreate a reg exp and use exec() to match")
print("var re2 = new RegExp(/\\w+/g)")
var re2 = new RegExp(/\w+/g)

print("resultsArray = re2.exec(\"one two three four\")")

while((arr = re2.exec("one two three four")) != null)
{
	print(arr.index.toString(), "-", arr.lastIndex.toString(), "\t", arr.toString())
}

print("\nImplicit reg exp creation and exec()")
print("var re3 = /\\w+/g")
print("var src = \"five six seven eight\"")
print("while ((arr = re3.exec(src)) != null)")
var re3 = /\w+/g
var src = "five six seven eight"

while ((arr = re3.exec(src)) != null)
{
	print(arr.index.toString(), "-", arr.lastIndex.toString(), "\t", arr.toString())
}

print("\nGet the flags for a reg exp")
print("Multiline: ", re2.multiline)
print("Ignore case: ", re2.ignoreCase)
print("Global: ", re2.global)

print("\nString that was matched: ", re.matched())
print("\nPattern that was used: ", re.pattern)

print("\nSetPattern that is used to [abcdef]")
re.setPattern("[abcdef]")

print("\narr = re.exec(\"Does this match anything?\")")
arr = re.exec("Does this match anything?")
print(arr.index.toString(), "-", arr.lastIndex.toString(), "\t", arr.toString())

print("\nReplacement: input is \"Does this match anything?\", pattern is [abcde] and replace string is \"NEW\"")
print("Output is: ", re.replace("Does this match anything?", "NEW"))

print("\nSplitting: re.split(\"Does this match anything?\") and pattern is [abcde]")
arr = re.split("Does this match anything?")
print("Output is: ", arr.toString())

print("\nTest to see if there is a match where pattern is [abcde]")
print("re.test(\"No\") : ", re.test("No"))
print("re.test(\"Yes\") : ", re.test("Yes"))

print("\n*** Tests completed ***")
