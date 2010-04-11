
//
//  Test nested break and correctly handling eval stack
//
k = 0
for each (j in 10) {
    k += 1
    for each (i in 10) {
        break
    }
}
assert(k == 10)


//
//  Test nested switch and correctly handling eval stack
//
k = 0
for (i in 10) {
    switch (i) {
    case 0:
        break
    default:
        k += 1
    }
}
assert(k == 9)


//
//  Test nested try/catch and correctly handling eval stack
//
k = 0
for (j in 10) {
    k++
    try {
        if (j == 4) {
            break
        }
    } catch (e) {
    } finally {
    }
}
assert(k == 5)
