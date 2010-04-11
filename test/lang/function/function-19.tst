/*
 *  Multiline function expressions
 */

x = (function (n) { return n + 1; })(77);
assert(x == 78)

var longer = (function () {
    var tasks = {}
    return ""
})();
