/*
 *  Path testing
 */

const TestLength = 500
const TestPath: String = "lib/io/file.dat"

//  Constructor

Path("/a/b/c")
Path("")
Path(Path("/a/b/c"))

//	Test basic file attributes

p = new Path(TestPath)
assert(p.exists)
assert(p.isRegular)
assert(!p.isDir)
assert(!p.isLink)
assert(!p.isOpen)
assert(p.isRelative)
assert(!p.isAbsolute)
assert(!p.hasDrive)
assert(p.name == TestPath)
assert(p.size == TestLength)
assert(p.length == TestPath.length)
assert(p.created is Date)
assert(p.accessed is Date )
assert(p.modified is Date )
if (p.hasDriveSpecs) {
	assert(p.name[0].isAlpha)
}

//  basename, dirname, extension

p = Path("/a/b/c.dat")
assert(p.basename == "c.dat")
assert(p.dirname == "/a/b")
assert(p.parent.basename == "b")
assert(p.extension == "dat")

//	absolute, relative

p = Path("file.dat")
assert(p.parent.isAbsolute)
assert(p.relative == "file.dat")
assert(Path("a.b").relative == "a.b")
assert(Path("./a.b").relative == "a.b")
assert(Path("/tmp/a.b").relative.toString().indexOf("../") >= 0)
assert(Path("/tmp/a.b").relative.isRelative)
assert(Path("/tmp/a.b").absolute.isAbsolute)

//  portable, natural

assert(p.portable == "file.dat")
p = Path("\\dir\\ABCdef.txt")
assert(p.portable == "/dir/ABCdef.txt")
if (Config.OS == "WIN") {
    assert(Path("c:\\a\\b\\c").portable == "c:/a/b/c")
    assert(Path("\\a\\b\\c").portable == "/a/b/c")
    p = Path("/a/b/c").natural
    assert(p.isAbsolute)
    assert(p.toString().indexOf('\\') >= 0)
}

//  normalize

assert(Path("////a/b/c").normalize == "/a/b/c")
assert(Path("////a/b/c////").normalize == "/a/b/c")
assert(Path("a/././b/c").normalize == "a/b/c")
assert(Path("/a/b/../c").normalize == "/a/c")
assert(Path("/a/b/../c/.").normalize == "/a/c")

//  startsWith, endsWith

p = Path("/a/b/c.gif")
assert(p.startsWith("/a"))
assert(p.endsWith(".gif"))

//  tosString

p = Path("/a/b/c.gif")
assert(p.toString() == "/a/b/c.gif")

//  join

assert(Path("/tmp").join("my.dat") == "/tmp/my.dat")
assert(Path("/tmp").join("/my.dat") == "/my.dat")
assert(Path("/tmp").join("////my.dat") == "/my.dat")
assert(Path("/tmp").join("./my.dat") == "/tmp/my.dat")
assert(Path("/tmp").join("./././my.dat") == "/tmp/my.dat")
assert(Path("").join("./././my.dat") == "my.dat")
assert(Path("/tmp").join("") == "/tmp")
assert(Path("/tmp").join("") == "/tmp")

//  joinExt

assert(Path("/a/b/c").joinExt(".dat") == "/a/b/c.dat")
assert(Path("/a/b/c.mod").joinExt(".dat") == "/a/b/c.mod")

//  same

assert(Path("./Makefile").same("Makefile"))

assert(Path("../test/Makefile").same("Makefile"))
assert(Path("../test/Makefile").absolute.same("Makefile"))
assert(Path("../test/Makefile").relative.same("Makefile"))

//  copy and remove

name = "temp-" + hashcode(self) + ".tdat"
Path(TestPath).copy(name)
p = new Path(name)
assert(p.exists)
assert(p.size == TestLength)
p.remove()
assert(!p.exists)

//  rename

name = "temp-" + hashcode(self) + ".tdat"
p = new Path(name)
Path(TestPath).copy(name)
assert(p.exists)
p.rename("another.dat")
assert(Path("another.dat").exists)
Path("another.dat").remove()

//  rename directory

name = "temp-" + hashcode(self) + ".tdat"
p = new Path(name)
p.remove()
assert(!p.exists)
p.makeDir()
assert(p.exists)
assert(p.isDir)
another = "another-" + hashcode(self) + ".tdat"
p.rename(another)
assert(Path(another).exists)
assert(Path(another).isDir)
Path(another).remove()

//  files

files = Path(".").files(true)
assert(files is Array)
assert(files.length > 5)
assert(files.find(function (e) { return e.same("test.init"); }) == "test.init")

//	makeDir, remove

name = "tempDir-" + hashcode(self) + ".tdat"
p = new Path("tempDir")
p.remove()
assert(!p.exists)
p.makeDir()
assert(p.exists)
assert(p.isDir)
p.remove()
if (Config.OS != 'windows') {
    //  On windows, exists sometimes takes time to full reveal
    assert(!p.exists)
}

//  open

file = Path(TestPath).open({ mode: "r" })
buf = new ByteArray()
count = file.read(buf)
assert(buf.available == TestLength)
file.close()

// openTextStream

stream = Path(TestPath).openTextStream({ mode: "r" })
line = stream.readLine()
assert(line = "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789")
stream.close()

//	readLines

lines = Path(TestPath).readLines()
for each (l in lines) {
	assert(l.search("12345") == 0);
}

// readString

str = Path(TestPath).readString()
assert(str.length == 500)
assert(str.contains("123456789"))

// readXml

xml = Path("lib/io/file.xml").readXML()
assert(xml is XML)
assert(xml.customer != null)

//	write 

name = "temp-" + hashcode(self) + ".tdat"
Path(name).write("Hello World\r\n", new Date, "\r\n")
p = Path(name)
assert(p.exists)
p.remove()
assert(!p.exists)

//  truncate

name = "temp-" + hashcode(self) + ".tdat"
Path(TestPath).copy(name)
p = new Path(name)
assert(p.exists)
assert(p.size == TestLength)
p.truncate(10)
assert(p.size == 10)
p.remove()

//  makeTemp

p = Path(".").makeTemp()
assert(p.exists)
p.remove()

//
//  Read lines ending with CRLF
//
filename = "temp.dat"
Path(filename).write("Hello\r\nWorld\r\n")
lines = Path(filename).readLines()
assert(lines[0] == "Hello")
assert(lines[1] == "World")
rm(filename)


//
//  Read lines ending with LF
//
filename = "temp.dat"
Path(filename).write("Hello\nWorld\n")
lines = Path(filename).readLines()
assert(lines[0] == "Hello")
assert(lines[1] == "World")
rm(filename)


//  Test links

if (Config.OS != "WIN" && Config.OS != "VXWORKS") {
    p = Path("sym.tmp")
    p.remove()
    p.makeLink("Makefile")
    assert(p.isLink)
    assert(p.linkTarget == "Makefile")
    assert(Path("UNKNOWN").linkTarget == null)
    assert(p.isLink)
    p.remove()
}

//  Iteration
p = new Path(".")
count = 0
for each (f in p) {
    count++
}
assert(count >= 4)
