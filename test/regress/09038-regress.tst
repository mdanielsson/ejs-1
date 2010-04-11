/*
    Was failing with an explicit -1 in copyIn
 */
source = new ByteArray
source.write("Hello")

dest = new ByteArray
dest.copyIn(0, source, 0, -1)
dest.writePosition += source.available
assert(dest == "Hello")
