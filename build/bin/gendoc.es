/*
    gendoc.es - Generate HTML doc from Doxygen XML files

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module embedthis.doc {

    var all: Boolean
    var bare: Boolean
    var files: String
    var out: String
    var symbols = {}
    var seeAlso = {}
    var tagFile = null
    var title: String = "Documentation"

    function usage(): String {
        return "usage: getdoc [--tags tagfile] [--title Title] files..."
    }

    function parseArgs(): Array {

        files = []

        for (argind = 1 ; argind < App.args.length; argind++) {
            arg = App.args[argind]
            switch (arg) {
            case "--all":
            case "-a":
                all = true
                break

            case "--bare":
                bare = true
                break

            case "--out":
                if (++argind >= App.args.length) {
                    throw usage()
                }
                out = App.args[argind]
                break

            case "--tags":
                if (++argind >= App.args.length) {
                    throw usage()
                }
                tagFile = App.args[argind]
                break

            case "--title":
                if (++argind >= App.args.length) {
                    throw usage()
                }
                title = App.args[argind]
                break

            default:
                files = App.args.slice(argind) 
                argind = App.args.length
            }
        }
        return files
    }


    function emit(...args) {
        print(args.toString())
    }


    function emitHeader() {
        if (bare) {
            return
        }
        emit('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"')
        emit('"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">')
        emit('<html xmlns="http://www.w3.org/1999/xhtml">')
        emit('<head>')
        emit('<meta http-equiv="Content-Type" content="text/html; charset=us-ascii" />')

        emit('<title>' + title + ' Documentation</title>')
        emit('<link href="api.css" rel="stylesheet" type="text/css" />')
        emit('</head>\n<body>\n')
        emit('  <div class="contents">')
    }


    function emitFooter() {
        if (bare) {
            return
        }
        emit('<div class="footnote">Generated on ' + new Date + '<br/>')
        emit('  Copyright &copy; <a href="http://www.embedthis.com">Embedthis Software</a> ' + 
            new Date().fullYear + '.')
        emit('</div></div></body></html>')
    }


    /*
     *  Emit top level navigation
     */
    function emitNavigation() {
        if (bare) {
            return
        }
        emit('<div>')
        result = []
        for each (kind in ["Components", "Typedefs", "Functions", "Defines"]) {
            result.append('<a href="#' + kind + '">' + kind + '</a>')
        }
        emit(result.join(" | "))
        emit('</div>')
    }


    /*
     *  Parse all symbol references and build an index of symbols
     */
    function parseReferences(xml: XML) {

        for each (def in xml) {
            if (def.@kind == "group" || def.@kind == "struct") {
                symbols[def.compoundname] = def.@id
                for each (sect in def.detaileddescription.para.simplesect) {
                    if (sect.@kind != "see") {
                        continue
                    }
                    seeAlso[def.compoundname] = sect.para
                }
            }
        }

        var sections: XML = xml.compounddef.sectiondef
        for each (section in sections) {
            var members: XML = section.memberdef
            for each (m in members) {
                if (m.name != "" && m.@id != "") {
                    symbols[m.name] = m.@id
                }
            }
        }
    }


    /*
     *  Emit a reference
     */
    function ref(name: String): String {

        /* Split the core type name off from "*" suffixes */
        parts = name.split(" ")
        typeSpec = parts.slice(0, 1)
        rest = parts.slice(1)
        sym = typeSpec.toString().trim("*")

        if (symbols[sym]) {
            result = '<a href="#' + symbols[sym] + '" class="ref">' + typeSpec + '</a>'
            if (rest && rest != "") {
               result += " " + rest
            }
            return result
        }
        return name
    }


    function strip(str: String): String {
        if (str == "") {
            return str
        }
        str = str.replace(/<para>|<emphasis>|<title>|<type>|<\/para>|<\/emphasis>|<\/title>|<\/type>/g, "")
        str = str.replace(/<ref refid="[^>]*>/g, '')
        str = str.replace(/<\/ref>/g, '')
        str = str.replace(/ kindref="[^"]*"/g, '')
        str = str.replace(/itemizedlist>/g, '')
        str = str.replace(/listitem>/g, '')
        str = str.replace(/<linebreak\/>/g, '')
        str = str.replace(/bold>/g, '')
        str = str.trim().trim(".").trim().trim(".")
        return str
    }


    /*
     *  Remove or map XML tags into HTML
     */
    function clean(str: String): String {
        if (str == "") {
            return str
        }
        str = str.replace(/<para>|<emphasis>|<title>|<type>|<\/para>|<\/emphasis>|<\/title>|<\/type>/g, "")
        str = str.replace(/<ref refid="/g, '<a class="ref" href="#')
        str = str.replace(/<\/ref>/g, '</a>')
        str = str.replace(/ kindref="[^"]*"/g, "")
        str = str.replace(/itemizedlist>/g, 'ul>')
        str = str.replace(/listitem>/g, 'li>')
        str = str.replace(/<linebreak\/>/g, "<br/>")
        str = str.replace(/bold>/g, "b>")
        str = str.trim().trim(".").trim().trim(".")
        return str
    }


    /*
     *  Clean the string of XML tags and append a "."
     */
    function cleanDot(str: String): String {
        s = clean(str)
        if (s == "") {
            return s
        }
        return s + "."
    }


    /*
     *  Intelligently transform a @return directive
     */
    function cleanReturns(str: String): String {
        str = cleanDot(str)
        str = str.replace(/^Return[s]* the/, "The")
        return str
    }


    /*
     *  Emit a file level overview
     */
    function emitOverview(xml: XML) {

        emit("<h1>" + title + "</h1>")
        for each (def in xml) {
            if (def.@kind != "file") {
                continue
            }
            name = def.compoundname

            if (!all && def.briefdescription == '' && def.detaileddescription == '') {
                return
            }
            emit('<a name="' +  name + '"></a>')

            if (def.briefdescription != "") {
                emit('<p>' + cleanDot(def.briefdescription.para) + '</p>')
            }
            if (def.detaileddescription != "") {
                for each (node in def.detaileddescription.*) {
                    str = node.toString()
                    str = str.replace(/para>/g, "p>")
                    str = str.replace(/title>/g, "h1>")
                    str = str.replace(/<linebreak\/>/g, "<br/>")
                    str = str.replace(/<sect[^>]*>|<\/sect[0-9]>/g, "")
                    str = str.replace(/<simplesect[^>]*>|<\/simplesect>/g, "")
                    emit(str)
                }
            }
        }
    }


    /*
     *  Emit an index of all services
     */
    function emitServiceIndex(def: XML) {

        if (all || def.briefdescription != "" || def.detaileddescription != '') {
            emit('<tr class="apiDef">')
            emit('<td class="apiName"><a href="#' + def.@id + '" class="nameRef">' + def.compoundname + '</a></td>')
            emit('<td class="apiBrief">' + cleanDot(def.briefdescription.para) + '</td></tr>')
        }
        symbols[def.compoundname] = def.@id
    }


    /*
     *  Emit an index of all functions
     */
    function emitFunctionIndex(def: XML, section: XML) {

        var members: XML = section.memberdef

        for each (m in members) {
            if (m.@kind == 'function') {
                if (!all && m.briefdescription == '' && m.detaileddescription == '') {
                    continue
                }
                if (def.@kind == "file" && m.@id.toString().startsWith("group__")) {
                    continue
                }

                def = m.definition.toString().split(" ")
                typedef = def.slice(0, -1).join(" ")
                name = def.slice(-1)

                emit('<tr class="apiDef">')
                str = '<td class="apiType">' + ref(typedef) + '</td><td>'
                str += '<a href="#' + m.@id + '" class="nameRef">' + name + '</a>'

                args = m.argsstring.toString().trim('(').split(",")
                if (args.length > 0) {
                    result = []
                    for (i in args) {
                        arg = args[i]
                        result.append(ref(arg))
                    }
                    str += "(" + result.join(", ")
                }
                emit(str + '</td></tr>')
                if (m.briefdescription != "") {
                    emit('<tr class="apiBrief"><td>&nbsp;</td><td>' + 
                        cleanDot(m.briefdescription.para) + '</td></tr>')
                }
            }
        }
    }


    /*
     *  Emit an index of all #define directives
     */
    function emitDefineIndex(section: XML) {

        var members: XML = section.memberdef

        for each (m in members) {
            if (m.@kind == 'define') {
                if (!all && m.briefdescription == '' && m.detaileddescription == '') {
                    continue
                }
                symbols[m.name] = m.@id

                emit('<tr class="apiDef">')
                emit('<td>#define</td><td><a href="#' + m.@id + '" class="nameRef">' + m.name + '</a>' + 
                    '&nbsp;&nbsp;&nbsp;' + m.initializer + '</td>')
                emit('</tr>')
                if (m.briefdescription != "") {
                    emit('<tr class="apiBrief"><td>&nbsp;</td><td>' + 
                        cleanDot(m.briefdescription.para) + '</td></tr>')
                }
            }
        }
    }


    /*
     *  Emit an index of struct based typedefs
     */
    function emitTypeIndex(def: XML) {

        if (all || def.briefdescription != "" || def.detaileddescription != '') {
            emit('<tr class="apiDef">')
            name = def.compoundname

            symbols[name] = def.@id

            emit('<td class="apiName"><a href="#' + symbols[name] + '" class="nameRef">' + name + '</a></td>')
            emit('<td class="apiBrief">' + cleanDot(def.briefdescription.para) + '</td></tr>')
        }
    }


    /*
     *  Emit an index of all simple (non-struct) typedefs
     */
    function emitStructTypeIndex(section: XML) {

        var members: XML = section.memberdef

        for each (m in members) {
            if (m.@kind == 'typedef') {
                if (!all && m.briefdescription == '' && m.detaileddescription == '') {
                    continue
                }
                symbols[m.name] = m.@id

                def = m.definition.toString()
                if (def.contains("(")) {
                    /* Parse "typedef void(* MprLogHandler)(cchar *file, ... cchar *msg) */
                    name = def.toString().replace(/typedef[^\(]*\([^\w]*([\w]+).*/, "$1")

                } else {
                    def = def.toString().split(" ")
                    typedef = def.slice(0, -1).join(" ")
                    name = def.slice(-1)
                }

                emit('<tr class="apiDef">')
                emit('<td class="apiName"><a href="#' + m.@id + '" class="nameRef">' + name + '</a></td>')
                if (m.briefdescription != "") {
                    emit('<td class="apiBrief">' + cleanDot(m.briefdescription.para) + '</td></tr>')
                }
            }
        }
    }


    /*
     *  Get master group references for a given group name
     */
    function getGroupReferences(group: String): Array {
        references = []
        items = seeAlso[group]
        if (items != undefined) {
            for each (see in items.ref.*) {
                references.append(see)
            }
        }
        return references
    }


    /*
     *  Emit a See Also section. Handle groups/struct @references
     */
    function emitSeeAlso(name: String, node: XML) {
        if (node.para.toString().startsWith('@-')) {
            let name = node.para.toString().slice(2).trim()
            references = getGroupReferences(name)

        } else {
            references = []
            for each (see in node.para.ref.*) {
                references.append(see)
            }
        }
        emitSeeAlsoReferences(name, references)
    }


    function emitSeeAlsoReferences(name: String, references: Array) {
        emit('    <dl><dt>See Also:</dt><dd>')
        references.sort()
        trimmed = []
        for each (see in references) {
            if (see == name) {
                continue
            }
            trimmed.append(ref(see))
        }
        emit('    ' + trimmed.join(", ") + '</dd></dl>')
    }


    function emitSimpleSect(name: String, node: XML) {
        if (node.@kind == "see") {
            emitSeeAlso(name, node)

        } else if (node.@kind == "return") {
            emit('    <dl><dt>Returns:</dt><dd>' + cleanReturns(node.para).toPascal() + '</dd></dl>')

        } else if (node.@kind == "remark") {
            emit('    <dl><dt>Remarks:</dt><dd>' + cleanDot(node.para).toPascal() + '</dd></dl>')

        } else {
            emit('    <dl><dt>' + clean(node.title) + '</dt><dd>' + cleanDot(node.para).toPascal() + '</dd></dl>')
        }
    }


    /*
     *  Emit function args
     */
    function emitArgs(args: XML) {

        result = []
        for each (p in args.param) {
            if (p.type == "...") {
                result.append("...")
            } else {
                s = clean(p.type) + " " + p.declname
                s = s.replace(/ ([\*]*) /, " $1")
                result.append(s)
            }
        }
        emit("(" + result.join(", ") + ")")
    }


    /*
     *  Used for function and simple typedef details
     */
    function emitDetail(def: XML, section: XML) {

        var members: XML = section.memberdef

        if (section.@kind == "func") {
            kind = "function"

        } else if (section.@kind == "typedef") {
            kind = "typedef"
        }

        for each (m in members) {
            if (m.@kind == kind) {
                if (!all && m.briefdescription == '' && m.detaileddescription == '') {
                    continue
                }
                if (def.@kind == "file" && m.@id.toString().startsWith("group__")) {
                    continue
                }

                emit('<a name="' + m.@id + '"></a>')

                emit('<div class="api">')
                emit('  <div class="prototype">')

                if (kind == "function") {
                    emit('    ' + ref(strip(m.type)))
                    str = m.definition.toString().split(" ").slice(1).join(" ")
                    emit('    ' + clean(str))
                    emitArgs(m)

                } else if (kind == "typedef") {
                    emit('    ' + cleanDot(m.definition))
                }
                emit('  </div>')

                emit('  <div class="apiDetail">')

                if (m.briefdescription != "") {
                    emit('<p>' + cleanDot(m.briefdescription.para) + '</p>')
                }

                seen = false
                for each (n in m.detaileddescription.para.*) {
                    if (n.name() == "simplesect") {
                        if (n.@kind == "see") {
                            seen = true
                        }
                        emitSimpleSect(m.name, n)

                    } else if (n.name() == "parameterlist") {
                        /*
                         *  Parameters
                         */
                        emit('    <dl><dt>Parameters:</dt><dd>')
                        emit('    <table class="parameters" summary="Parameters">')
                        for each (p in n.parameteritem) {
                            emit('    <tr><td class="param">' + p.parameternamelist.parametername + '</td><td>' + 
                                cleanDot(p.parameterdescription.para) + '</td>')
                        }
                        emit('    </table></dd></dl>')

                    } else {
                        emit(clean(n))
                    }
                }
                if (!seen && def.@kind == "group") {
                    references = getGroupReferences(def.compoundname)
                    emitSeeAlsoReferences(m.name, references)
                }
                emit('  </div>')
                emit('</div>')
            }
        }
    }


    function emitFields(name: String, def: XML) {
        let doneHeader = false
        for each (m in def.sectiondef.memberdef) {
            if (!doneHeader) {
                emit('    <dl><dt>Fields:</dt><dd>')
                emit('    <table class="parameters" summary="Parameters">')
                doneHeader = true
            }
            if (m.@kind == "variable") {
                field = m.definition.toString().replace(/.*::/, "")
                if (m.briefdescription != "" || m.detaileddescription != "") {
                    emit('    <tr><td class="param">' + clean(m.type) + '</td><td><td>' + field + '</td><td>')
                    if (m.briefdescription != "") {
                        s = cleanDot(m.briefdescription.para)
                        if (m.detaileddescription != "") {
                            s += " "
                        }
                        emit(s)
                    }
                    if (m.detaileddescription != "") {
                        s = cleanDot(m.detaileddescription.para)
                        emit(s)
                    }
                    emit('</td>')
                }
            }
        }
        if (doneHeader) {
            emit('    </table></dd></dl>')
        }
    }


    function emitStructDetail(def: XML, fields: XML) {
        let name = def.compoundname
        if (!all && def.briefdescription == '' && def.detaileddescription == '') {
            return
        }
        emit('<a name="' + symbols[name] + '"></a>')
        emit('<div class="api">')
        emit('  <div class="prototype">' + clean(name) + '</div>')
        emit('  <div class="apiDetail">')

        if (def.briefdescription != "") {
            emit('<p>' + cleanDot(def.briefdescription.para) + '</p>')
        }
        let doneFields = false
        if (def.detaileddescription != "") {
            for each (n in def.detaileddescription.para.*) {
                if (n.name() == "simplesect") {
                    emitSimpleSect(name, n)

                    if (!doneFields && fields == null) {
                        emitFields(name, def)
                        doneFields = true
                    }
                }
            }
        }
        if (fields) {
            emitFields(name, fields)
        } else if (!doneFields) {
            emitFields(name, def)
        }
        emit('  </div>')
        emit('</div>')
    }


    function emitIndicies(xml: XML) {

        var sections: XML

        /*
         *  Emit the group indicies
         */
        emit('<a name="Components"></a><h1>Components</h1>')
        emit('  <table class="apiIndex" summary="Components">')

        for each (def in xml) {
            if (def.@kind == "group") {
                emitServiceIndex(def)
            }
        }
        emit('</table>')

        /*
         *  Emit the navigation indicies
         */
        emit('<a name="Functions"></a><h1>Functions</h1>')
        emit('  <table class="apiIndex" summary="Functions">')

        for each (def in xml) {
            sections = def.sectiondef
            for each (section in sections) {
                if (section.@kind == "func") {
                    emitFunctionIndex(def, section)
                }
            }
        }
        emit('</table>')

        emit('<a name="Typedefs"></a><h1>Typedefs</h1>')
        emit('<table class="apiIndex" summary="typedefs">')
        for each (def in xml) {
            if (def.@kind == "struct") {
                emitTypeIndex(def)
            } else {
                sections = def.sectiondef
                for each (section in sections) {
                    if (section.@kind == "typedef") {
                        emitStructTypeIndex(section)
                    }
                }
            }
        }
        emit('</table>')

        emit('<a name="Defines"></a><h1>Defines</h1>')
        emit('<table class="apiIndex" summary="Defines">')
        for each (def in xml) {
            sections = def.sectiondef
            for each (section in sections) {
                if (section.@kind == "define") {
                    emitDefineIndex(section)
                }
            }
        }
        emit("  </table>")
    }


    function emitDetails(xml: XML) {
        /*
            Emit groups
         */
        for each (def in xml) {
            if (def.@kind != "group") {
                continue
            }
            emit('<h1>' + def.compoundname + '</h1>')
            let foundStruct = false
            for each (d in xml) {
                if (d.@kind == "struct" && d.compoundname.toString() == def.compoundname.toString()) {
                    foundStruct = true
                    emitStructDetail(def, d)
                }
            }
            if (!foundStruct) {
                for each (d in xml) {
                    if (d.compoundname.toString() == def.compoundname.toString()) {
                        emitStructDetail(def, d)
                    }
                }
            }
            let sections: XML = def.sectiondef
            for each (section in sections) {
                if (section.@kind == "func") {
                    emitDetail(def, section)
                }
            }
        }

        /*
            Emit functions
         */
        emit('<h2>Functions</h2>')

        for each (def in xml) {
            if (def.@kind == "group") {
                continue
            }
            let sections: XML = def.sectiondef
            for each (section in sections) {
                if (section.@kind == "func") {
                    emitDetail(def, section)
                }
            }
        }

        emit('<h2>Typedefs</h2>')
        for each (def in xml) {
            if (def.@kind == "struct") {
                emitStructDetail(def, null)
            } else {
                let sections: XML = def.sectiondef
                for each (section in sections) {
                    if (section.@kind == "typedef") {
                        emitDetail(def, section)
                    }
                }
            }
        }
    }


    function parse(xml: XML) {
        parseReferences(xml)
        emitHeader()
        emitNavigation()
        emitOverview(xml)
        emitIndicies(xml)
        emitDetails(xml)
        emitFooter()
    }


    function saveTags() {
        if (tagFile) {
            Path(tagFile).write(serialize(symbols))
        }
    }


    function main() {
        var xml: XML

        for each (f in parseArgs()) {
            var tmp = new XML(f)
            if (xml) {
                len = xml.compounddef.length()
                xml.compounddef[len] = tmp.compounddef
            } else {
                xml = tmp
            }
        }
/*
for each (n in xml) {
    e("id " + n.@id)
}
*/
        parse(xml)
        saveTags()
    }

    main()

    function e(...args) {
        App.errorStream.write(args + "\n")
    }
}



/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
    
    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire 
    a commercial license from Embedthis Software. You agree to be fully bound 
    by the terms of either license. Consult the LICENSE.TXT distributed with 
    this software for full details.
    
    This software is open source; you can redistribute it and/or modify it 
    under the terms of the GNU General Public License as published by the 
    Free Software Foundation; either version 2 of the License, or (at your 
    option) any later version. See the GNU General Public License for more 
    details at: http://www.embedthis.com/downloads/gplLicense.html
    
    This program is distributed WITHOUT ANY WARRANTY; without even the 
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
    
    This GPL license does NOT permit incorporating this software into 
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses 
    for this software and support services are available from Embedthis 
    Software at http://www.embedthis.com 
    
    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
