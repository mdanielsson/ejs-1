/*
    Test basic Config types
*/

assert(Config.Debug is Boolean)
assert(Config.Legacy is Boolean)

assert(Config.CPU is String)
assert(Config.OS is String)
assert(Config.Product is String)
assert(Config.Title is String)
assert(Config.Version is String)

assert(Config.LibDir is Path)
assert(Config.BinDir is Path)
assert(Config.ModDir is Path)
