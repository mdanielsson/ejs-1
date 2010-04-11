/*
 *  Cache tests
 */

require ejs.web

cache = new Cache
cache.write("farm", "one", 77)
data = cache.read("farm", "one")
assert(data == 77)
