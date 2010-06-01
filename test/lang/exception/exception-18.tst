/*
    Test stack clearing in exception blocks
 */
count = 0
for each (obj in [1,2]) {
    assert(obj is Number)
    try {
        tmp = (new XML("bad_xml_file_name.xml")).someElementName
    } catch (e) {
        count++;
    }
}
assert(count == 2)


//  Test inside a function without args

function funWithout() {
    for (j in 2) {
        for (i in 5) {
            try { throw "Boom"; } catch {}
        }
    }
}
funWithout()


//  Test inside a function with args

function funWith(a,b,c,d) {
    for (j in 2) {
        for (i in 5) {
            try { throw "Boom"; } catch {}
        }
    }
}
funWith(1,2,3,4)
