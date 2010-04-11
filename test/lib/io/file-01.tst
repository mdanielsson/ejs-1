
/*
 *	Length in bytes of file.dat
 */
const TestLength = 500
const TestFile: String = "lib/io/file.dat"

//	readBytes

f = new File(TestFile)
f.open("r")
assert(f.isOpen)
assert(f.canRead)
assert(!f.canWrite)
bytes = f.readBytes(1024)
assert(f.options.mode == "r")
assert(f.options.permissions == 0664)
assert(bytes.length == 1024)
assert(bytes.available == TestLength)

//	position

f.position = 0
again = f.readBytes(1024)
assert(again.length == bytes.length)
assert(again.available == TestLength)

//	write

let filename = "temp-" + hashcode(self) + ".tdat"
f = new File(filename, { mode: "w", permissions: 0644 })
f.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\r\n")
f.flush()
f.close()
assert(f.size == 102)
assert(!f.isOpen)

//  path

path = f.path
assert(path.exists)
// assert(path.permissions = 0644)
// path.permissions = 777
// assert(path.permissions = 0777)
f.remove()
assert(!path.exists)

//	index access

f = new File(filename, { mode: "w" })
for (i in 256) {
	f[i] = i
}
f.close()
assert(!f.isOpen)
f.open()
for (i in 256) {
	assert(f[i] == i)
}
f.close()

//	Read via direct indexing 

f = new File(filename)
f.open()
count = 0
for (i in f) {
	assert(f[i] == i)
	count++
}
assert(count == 256)
f.close()
f.remove()
assert(!f.path.exists)

//  truncate

Path(TestFile).copy(filename)
assert(Path(filename).exists)
f = new File(filename)
f.truncate(5)
assert(f.size == 5)
assert(f.path.size == 5)
f.remove()

/*
 *  Create a file
 */
f = File(filename).open({mode: "w"})
for (i in (1024)) {
    f.write("" + i + " The lazy brown fox jump over the quick dog")
}
f.close
f.open()

out = "temp-2-" + hashcode(self) + ".tdat"
of = File(out).open({mode: "w"})

buf = new ByteArray
while (f.read(buf)) {
    of.write(buf)
}
of.close()
assert(Path(filename).size == Path(out).size)
f.close()
rm(filename)
rm(out)
