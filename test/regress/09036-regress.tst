/*
    RegExp constructors were creating bad matches
 */
let guid = "13046af2_2d8a_4e9d_8c98_dd6ac8704c87";

parts1 = guid.split(/[-_]/g, -1)

r = RegExp("[-_]", "g")
parts2 = guid.split(r, -1)

assert(parts1.toString() == parts2.toString())


