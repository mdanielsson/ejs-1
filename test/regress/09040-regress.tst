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
