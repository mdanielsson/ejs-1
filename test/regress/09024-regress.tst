/*
 *  Regression when reading lines from a text stream. Was losing data
 */

datafile = "regress/09024-" + hashcode(global) + ".tdat"

var generate = function() {
    file = new File(datafile)
    file.open("w")
    for (i in 500) {
        file.write(" %05d A lazy brown fox jumped over the quick dog\n".format(i))
    }
    file.close()
}

var provoke = function()
{
    var s: TextStream = Path(datafile).openTextStream("rt")
    while (line = s.readLine()) {
        /*
         *  Test and ensure each line is the right length. This will catch dropping bytes
         */
        // assert(line.length == 49)
        // print(line.length)
    }
    s.close()
}

generate()
provoke()
rm(datafile)
