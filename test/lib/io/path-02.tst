/*
    Test Path representations: natural, portable, join
 */

assert(Path("/a/b/c").separator == "/")

if (Config.OS == "WIN") {
    assert(Path("/a\\b/c").join("d/e/f") == "/a/b/c/d/e/f")
    assert(Path("\\a\\b/c").join("d/e/f") == "\\a\\b\\c\\d\\e\\f")
    assert(Path("\\a/b/c").separator == "\\")
    assert(Path("a/b\\c").natural == "a\\b\\c")
    assert(Path("/a/b\\c").portable == "/a/b/c")
    assert(Path("/a/b\\c").same("/a/b/c"))
} else {
    assert(Path("/a/b/c").join("d/e/f") == "/a/b/c/d/e/f")
    assert(Path("/a/b/c").join("d/e/f") == "/a/b/c/d/e/f")
    assert(Path("a/b/c").natural == "a/b/c")
    assert(Path("a/b/c").portable == "a/b/c")
}

