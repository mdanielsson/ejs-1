
/*
 *  FileSystem testing
 */

fs = new FileSystem("/")

//  newline

assert(fs.newline is String)
assert(fs.newline.length >= 1)

//  root

assert(fs.root.toString() == Path("/").absolute)
assert(FileSystem("/a/b/c").root == Path("/").absolute)


//  separators

if (Config.OS == "WIN") {
    assert(fs.separators == "\\/")
} else {
    assert(fs.separators == "/")
}
