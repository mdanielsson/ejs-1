/*
 *  Test delete actually removes a property
 */
o = {
    name: "John",
    address: "1234 lane",
    color: "blue",
}

delete o.name

count = 0
for (i in o) {
    count++
}
assert(count == 2)
