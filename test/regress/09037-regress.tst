/*
    Regression parsing numeric strings inside arrays
 */

s = '{
    group: [
        "127.0.0.1"
    ]
}'

o = deserialize(s)
assert(o.group[0] == "127.0.0.1")
