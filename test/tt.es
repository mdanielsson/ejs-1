for (j in 1) {
for (i in 1) {
    try {
        var x = new XML("unknown")
    } catch {
        print("CATCH: " + i)
    }
    print(i)
}
}




/*
function fun() {
    {
        use namespace "gold"
        let x = 99

        try {
            use namespace "blue"
            // throw "Here"

            {
                use namespace "red"
                    //  Should this discard one or two blocks?
                throw "Here"
            }
        } catch {
            print("X " + x)
        }
    }
}
fun()
*/
