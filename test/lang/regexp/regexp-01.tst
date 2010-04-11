/*
 *  Test regular expressions
 */

re = new RegExp("def")
assert(re is RegExp)

/*
 *  "test" method
 */
assert(re.test("abcdef"))
assert(!re.test("xyz"))

/*
 *  Basic exec
 */
assert(re.exec("abcdef") == "def")
assert(re.exec("xyz") == null)

/*
 *  With global flag
 */
re = /def/g
assert(re.exec("abcdef") == "def")
assert(re.exec("xyz") == null)

re = /(a)(b)(c)/g
assert((re.exec("abc def abc")).length == 4)
assert(re.exec("abc def abc") == "abc,a,b,c")

/*
 *  Case insensitive matching
 */
assert(/def/.test("def"))
assert(!/def/.test("DEF"))
assert(/def/i.test("DEF"))

/*
 *  toString
 */
assert(/def/ == "/def/")
assert(/def/g == "/def/g")
v = /def/g
assert(v.toString() == "/def/g")


re = /def/g
assert(re.global)
assert(!re.ignoreCase)
assert(!re.multiline)
assert(!re.sticky)
assert(/def/y.sticky)
assert(/def/m.multiline)
assert(/def/i.ignoreCase)

re = /def/g
re.exec("abcdefghi")
assert(re.lastIndex == 6)
assert(re.start == 3)
re.lastIndex = 0
assert(re.lastIndex == 0)
assert(re.matched == "def")

//  Test RegExp constructor

let s = "abc_def-ghi_jkl"
re = new RegExp("def")

let parts = s.split(RegExp("[-_]", "g"))
assert(parts.length == 4)
assert(parts == "abc,def,ghi,jkl")

assert(RegExp("abc", "g") == "/abc/g")
assert(RegExp("abc", "g").toString() == "/abc/g")
assert(RegExp("/abc/g") == "//abc/g/")

//  replace

var value = new RegExp("z|d", "g")
assert(value.replace("The lazy dog", "ZZZ") == "The laZZZy ZZZog")

//  source
var result = new RegExp("z|d", "g")
assert(result.source == "z|d")

