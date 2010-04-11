
/*
 *  Test object literals with namespace qualified field names
 */
var o = {
    "blue"::name: 1,
}

assert(o."blue"::["name"] == 1)
