/**
 *	HtmlConnector.es -- Basic HTML control connector
 */

module ejs.web {

    require ejs.db

	/**
	 *	The Html Connector provides bare HTML encoding of Ejscript controls
        @hide
	 */
	class HtmlConnector {

        use default namespace "ejs.web"

        private var nextId: Number = 0
        private var controller: Controller

        function HtmlConnector(controller) {
            this.controller = controller
        }

        /*
         *  Options to implement:
         *      method
         *      update
         *      confirm     JS confirm code
         *      condition   JS expression. True to continue
         *      success
         *      failure
         *      query
         *
         *  Not implemented
         *      submit      FakeFormDiv
         *      complete
         *      before
         *      after
         *      loading
         *      loaded
         *      interactive
         */
        /**
         *  Render an asynchronous (ajax) form.
         *  @param record Initial data
         *  @param url Action to invoke when the form is submitted. Defaults to "create" or "update" depending on 
         *      whether the field has been previously saved.
         *  @param options Optional extra options. See $getOptions for a list of the standard options.
         *  @option url String Use a URL rather than action and controller for the target url.
         */
		function aform(record: Object, url: String, options: Object): Void {
            if (options.id == undefined) {
                options.id = "form"
            }
            onsubmit = ""
            if (options.condition) {
                onsubmit += options.condition + ' && '
            }
            if (options.confirm) {
                onsubmit += 'confirm("' + options.confirm + '"); && '
            }
            onsubmit = '$.ajax({ ' +
                'url: "' + url + '", ' + 
                'type: "' + options.method + '", '

            if (options.query) {
                onsubmit += 'data: ' + options.query + ', '
            } else {
                onsubmit += 'data: $("#' + options.id + '").serialize(), '
            }

            if (options.update) {
                if (options.success) {
                    onsubmit += 'success: function(data) { $("#' + options.update + '").html(data).hide("slow"); ' + 
                        options.success + '; }, '
                } else {
                    onsubmit += 'success: function(data) { $("#' + options.update + '").html(data).hide("slow"); }, '
                }
            } else if (options.success) {
                onsubmit += 'success: function(data) { ' + options.success + '; } '
            }
            if (options.error) {
                onsubmit += 'error: function(data) { ' + options.error + '; }, '
            }
            onsubmit += '}); return false;'

            write('<form action="' + "/User/list" + '"' + getOptions(options) + "onsubmit='" + onsubmit + "' >")
        }

        /*
         *  Extra options:
         *      method
         *      update
         *      confirm     JS confirm code
         *      condition   JS expression. True to continue
         *      success
         *      failure
         *      query
         *
         */
        /** 
         *  Emit an asynchronous (ajax) link to an action. The URL is constructed from the given action and the 
         *      current controller. The controller may be overridden by setting the controller option.
         *  @param text Link text to display
         *  @param url Action to invoke when the link is clicked
         *  @param options Optional extra options. See $getOptions for a list of the standard options.
         *  @option controller String Name of the target controller for the given action
         *  @option url String Use a URL rather than action and controller for the target url.
         */
		function alink(text: String, url: String, options: Object): Void {
            if (options.id == undefined) {
                options.id = "alink"
            }
            onclick = ""
            if (options.condition) {
                onclick += options.condition + ' && '
            }
            if (options.confirm) {
                onclick += 'confirm("' + options.confirm + '"); && '
            }
            onclick = '$.ajax({ ' +
                'url: "' + url + '", ' + 
                'type: "' + options.method + '", '

            if (options.query) {
                'data: ' + options.query + ', '
            }

            if (options.update) {
                if (options.success) {
                    onclick += 'success: function(data) { $("#' + options.update + '").html(data); ' + 
                        options.success + '; }, '
                } else {
                    onclick += 'success: function(data) { $("#' + options.update + '").html(data); }, '
                }
            } else if (options.success) {
                onclick += 'success: function(data) { ' + options.success + '; } '
            }
            if (options.error) {
                onclick += 'error: function(data) { ' + options.error + '; }, '
            }
            onclick += '}); return false;'
            write('<a href="' + options.url + '"' + getOptions(options) + "onclick='" + onclick + "' >" + text + '</a>')
		}

        /**
         *  @duplicate ejs.web::View.button
         */
		function button(value: String, buttonName: String, options: Object): Void {
            write('<input name="' + buttonName + '" type="submit" value="' + value + '"' + getOptions(options) + ' />')
        }

        /**
         *  @duplicate ejs.web::View.buttonLink
         */
		function buttonLink(text: String, url: String, options: Object): Void {
			// write('<a href="' + url + '"><button>' + text + '</button></a>')
			write('<button onclick="window.location=\'' + url + '\';">' + text + '</button></a>')
        }

        /**
            @hide
         */
		function chart(data: Array, options: Object): Void {
            throw 'HtmlConnector control "chart" not implemented.'
		}

        /**
         *  Render an input checkbox. This creates a checkbox suitable for use within an input form. 
         *  @param name Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
         *      input element. If used inside a model form, it is the field name in the model containing the checkbox
         *      value to display. If used without a model, the value to display should be passed via options.value. 
         *  @param value Value to display
         *  @param submitValue Value to submit if checked. Defaults to "true"
         *  @param options Optional extra options. See $getOptions for a list of the standard options.
         */
		function checkbox(name: String, value: String, submitValue: String, options: Object): Void {
            let checked = (value == submitValue) ? ' checked="yes" ' : ''
            write('<input name="' + name + '" type="checkbox" "' + getOptions(options) + checked + 
                '" value="' + submitValue + '" />')
            write('<input name="' + name + '" type="hidden" "' + getOptions(options) + '" value="" />')
        }

        /**
         *  @duplicate ejs.web::View.endform
         */
		function endform(): Void {
            write('</form>')
        }

        /** 
         *  Emit a flash message area. 
         *  @param kind Kind of flash messages to display. 
         *  @param msg Flash message to display
         *  @param options Optional extra options. See $getOptions for a list of the standard options.
         *  @option retain Number. Number of seconds to retain the message. If <= 0, the message is retained until another
         *      message is displayed. Default is 0.
         *  @example
         *      <% flash("status") %>
         *      <% flash() %>
         *      <% flash(["error", "warning"]) %>
         */
		function flash(kind: String, msg: String, options: Object): Void {
            write('<div' + getOptions(options) + '>' + msg + '</div>\r\n')
            if (kind == "inform") {
                write('<script>$(document).ready(function() {
                        $("div.-ejs-flashInform").animate({opacity: 1.0}, 2000).hide("slow");});
                    </script>')
            }
		}

        /**
         *  Render a form.
         *  @param record Model record to edit
         *  @param url Action to invoke when the form is submitted. Defaults to "create" or "update" depending on 
         *      whether the field has been previously saved.
         *  @param options Optional extra options. See $getOptions for a list of the standard options.
         *  @option url String Use a URL rather than action and controller for the target url.
         */
		function form(record: Object, url: String, options: Object): Void {
            write('<form method="post" action="' + url + '"' + getOptions(options) + ' xonsubmit="ejs.fixCheckboxes();">')
//          write('<input name="id" type="hidden" value="' + record.id + '" />')
        }

        /**
         *  @duplicate ejs.web::View.image
         */
        function image(src: String, options: Object): Void {
			write('<img src="' + src + '"' + getOptions(options) + '/>')
        }

        /**
         *  @duplicate ejs.web::View.label
         */
        function label(text: String, options: Object): Void {
            write('<span ' + getOptions(options) + ' type="' + getTextKind(options) + '">' +  text + '</span>')
        }

        /** 
         *  Emit a link to an action. The URL is constructed from the given action and the current controller. The controller
         *  may be overridden by setting the controller option.
         *  @param text Link text to display
         *  @param action Action to invoke when the link is clicked
         *  @param options Optional extra options. See $getOptions for a list of the standard options.
         *  @option controller String Name of the target controller for the given action
         *  @option url String Use a URL rather than action and controller for the target url.
         */
		function link(text: String, action: String, options: Object): Void {
			write('<a href="' + action + '"' + getOptions(options) + '>' + text + '</a>')
		}

        /**
         *  @duplicate ejs.web::View.extlink
         */
		function extlink(text: String, url: String, options: Object): Void {
			write('<a href="' + url + '"' + getOptions(options) + '>' + text + '</a>')
		}

        /**
         *  Emit a selection list. 
         *  @param field Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
         *      input element. If used inside a model form, it is the field name in the model containing the list item to
         *      select. If used without a model, the value to select should be passed via options.value. 
         *  @param choices Choices to select from. This can be an array list where each element is displayed and the value 
         *      returned is an element index (origin zero). It can also be an array of array tuples where the first 
         *      tuple entry is the value to display and the second is the value to send to the app. Or it can be an 
         *      array of objects such as those returned from a table lookup. If choices is null, the $field value is 
         *      used to construct a model class name to use to return a data grid containing an array of row objects. 
         *      The first non-id field is used as the value to display.
         *  @param defaultValue Current value
         *  @params options Extra options
         *  Examples:
         *      list("stockId", Stock.stockList) 
         *      list("low", ["low", "med", "high"])
         *      list("low", [["low", "3"], ["med", "5"], ["high", "9"]])
         *      list("low", [{low: 3{, {med: 5}, {high: 9}])
         *      list("Stock Type")                          Will invoke StockType.findAll() to do a table lookup
         */
		function list(field: String, choices: Object, defaultValue: String, options: Object): Void {
            write('<select name="' + field + '" ' + getOptions(options) + '>')
            let isSelected: Boolean
            let i = 0
            for each (choice in choices) {
                if (choice is Array) {
                    isSelected = (choice[0] == defaultValue) ? 'selected="yes"' : ''
                    write('  <option value="' + choice[0] + '"' + isSelected + '>' + choice[1] + '</option>')
                } else {
                    if (choice && choice.id) {
                        for (field in choice) {
                            isSelected = (choice.id == defaultValue) ? 'selected="yes"' : ''
                            if (field != "id") {
                                write('  <option value="' + choice.id + '"' + isSelected + '>' + choice[field] + '</option>')
                                done = true
                                break
                            }
                        }
                    } else {
                        isSelected = (i == defaultValue) ? 'selected="yes"' : ''
                        write('  <option value="' + i + '"' + isSelected + '>' + choice + '</option>')
                    }
                }
                i++
            }
            write('</select>')
        }

        /**
         *  @duplicate ejs.web::View.mail
         */
		function mail(nameText: String, address: String, options: Object): Void  {
			write('<a href="mailto:' + address + '" ' + getOptions(options) + '>' + nameText + '</a>')
		}

        /**
         *  @duplicate ejs.web::View.progress
            @hide
         */
		function progress(initialData: Array, options: Object): Void {
            write('<p>' + initialData + '%</p>')
		}

        //  Emit: <input name ="model.name" id="id" class="class" type="radio" value="text"
        /** 
         *  Emit a radio autton. The URL is constructed from the given action and the current controller. The controller
         *      may be overridden by setting the controller option.
         *  @param name Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
         *      input element. If used inside a model form, it is the field name in the model containing the radio data to
         *      display. If used without a model, the value to display should be passed via options.value. 
            @param selected Selected option
         *  @param choices Array or object containing the option values. If array, each element is a radio option. If an 
         *      object hash, then they property name is the radio text to display and the property value is what is returned.
         *  @param action Action to invoke when the button is clicked or invoked
         *  @param options Optional extra options. See $getOptions for a list of the standard options.
         *  @option controller String Name of the target controller for the given action
         *  @option value String Name of the option to select by default
         *  @example
         *      radio("priority", ["low", "med", "high"])
         *      radio("priority", {low: 0, med: 1, high: 2})
         *      radio(priority, Message.priorities)
         */
        function radio(name: String, selected: String, choices: Object, options: Object): Void {
            let checked: String
            if (choices is Array) {
                for each (v in choices) {
                    checked = (v == selected) ? "checked" : ""
                    write(v + ' <input type="radio" name="' + name + '"' + getOptions(options) + 
                        ' value="' + v + '" ' + checked + ' />\r\n')
                }
            } else {
                for (item in choices) {
                    checked = (choices[item] == selected) ? "checked" : ""
                    write(item + ' <input type="radio" name="' + name + '"' + getOptions(options) + 
                        ' value="' + choices[item] + '" ' + checked + ' />\r\n')
                }
            }
        }

		/** 
		 *	@duplicate ejs.web::View.script
		 */
		function script(url: String, options: Object): Void {
            write('<script src="' + url + '" type="text/javascript"></script>\r\n')
		}

        /**
         *  @duplicate ejs.web::View.status
            @hide
         */
		function status(data: Array, options: Object): Void {
            write('<p>' + data + '</p>\r\n')
        }

		/** 
		 *	@duplicate ejs.web::View.stylesheet
		 */
		function stylesheet(url: String, options: Object): Void {
            write('<link rel="stylesheet" type="text/css" href="' + url + '" />\r\n')
		}

        /**
         *  @duplicate ejs.web::View.tabs
         */
		function tabs(initialData: Array, options: Object): Void {
            write('<div class="-ejs-tabs">\r\n')
            write('   <ul>\r\n')
            for each (t in initialData) {
                for (name in t) {
                    let url = t[name]
                    write('      <li onclick="window.location=\'' + url + '\'"><a href="' + url + '">' + name + '</a></li>\r\n')
                }
            }
            write('    </ul>')
            write('</div>')
        }

        private function getColumns(data, options: Object): Object {
            let columns
            if (options.columns) {
                if (options.columns is Array) {
                    columns = {}
                    for each (name in options.columns) {
                        columns[name] = name
                    }
                } else {
                    columns = options.columns
                }
            } else {
                /*
                 *  No supplied columns. Infer from data
                 */
                columns = {}
                if (data is Array) {
                    for (let name in data[0]) {
                        if (name == "id" && !options.showId) continue
                        columns[name] = name
                    }
                }
            }
            return columns
        }
    
/*
        private function getSort(columns: Object, options: Object): Array {
            let sort = options.sort || true
            if (!sort) return [-1, 0]
            let sortCol = -1 
            let sortOrder = 0
            if (options.sort) {
                let col = 0
                for (name in columns) {
                    if (name == options.sort) {
                        sortCol = col
                        sortOrder = (options.sortOrder.toLower().contains("asc")) ? 0 : 1
                        break
                    }
                    col++
                }
            }
            if (sortCol < 0) {
                col = 0
                for each (column in columns) {
                    if (column.sort) {
                        sortCol = col
                        sortOrder = (column.sort.toLower().contains("asc")) ? 0 : 1
                        break
                    }
                }
            }
            return [sortCol, sortOrder]
        }
*/

        /**
         *  @duplicate ejs.web::View.table
         */
		function table(data, options: Object = null): Void {
            let originalOptions = options
            let tableId = view.getNextId()

            if (data is Array) {
                if (data.length == 0) {
                    write("<p>No Data</p>")
                    return
                }
            } else if (!(data is Array) && data is Object) {
                data = [data]
			}

            options = (originalOptions && originalOptions.clone()) || {}
            let columns = getColumns(data, options)

            let refresh = options.refresh || 10000
            let sortOrder = options.sortOrder || ""
            let sort = options.sort
            if (sort == undefined) sort = true

            if (!options.ajax) {
                let url = (data is String) ? data : null
                url ||= options.data
                write('  <script type="text/javascript">\r\n' +
                    '   $(function() { $("#' + tableId + '").eTable({ refresh: ' + refresh + 
                    ', sort: "' + sort + '", sortOrder: "' + sortOrder + '"' + 
                    ((url) ? (', url: "' + url + '"'): "") + 
                    '})});\r\n' + 
                    '  </script>\r\n')
                if (data is String) {
                    /* Data is an action method */
                    write('<table id="' + tableId + '" class="-ejs-table"></table>\r\n')
                    return
                }
            } else {
                write('  <script type="text/javascript">$("#' + tableId + '").eTableSetOptions({ refresh: ' + refresh +
                    ', sort: "' + sort + '", sortOrder: "' + sortOrder + '"})' + ';</script>\r\n')
            }
			write('  <table id="' + tableId + '" class="-ejs-table ' + (options.styleTable || "" ) + '">\r\n')

            /*
             *  Table title and column headings
             */
            if (options.showHeader != false) {
                write('    <thead class="' + (options.styleHeader || "") + '">\r\n')
                if (options.title) {
                    if (columns.length < 2) {
                        write('  <tr><td>' + options.title + ' ' + '<img src="' + controller.appUrl + 
                            '/web/images/green.gif" ' + 'class="-ejs-table-download -ejs-clickable" onclick="$(\'#' + 
                            tableId + '\').eTableToggleRefresh();" />\r\n  </td></tr>\r\n')
                    } else {
                        write('  <tr><td colspan="' + (columns.length - 1) + '">' + options.title + 
                            '</td><td class="right">' + '<img src="' + controller.appUrl + '/web/images/green.gif" ' + 
                            'class="-ejs-table-download -ejs-clickable" onclick="$(\'#' + tableId + 
                            '\').eTableToggleRefresh();" />\r\n  </td></tr>\r\n')
                    }
                }
                /*
                 *  Emit column headings
                 */
                if (columns) {
                    write('    <tr>\r\n')
                    for (let name in columns) {
                        if (name == null) continue
                        let header = (columns[name].header) ? (columns[name].header) : name.toPascal()
                        let width = (columns[name].width) ? ' width="' + columns[name].width + '"' : ''
                        write('    <th ' + width + '>' + header + '</th>\r\n')
                    }
                }
                write("     </tr>\r\n    </thead>\r\n")
            }

            let styleBody = options.styleBody || ""
            write('    <tbody class="' + styleBody + '">\r\n')

            let row: Number = 0

			for each (let r: Object in data) {
                let url = null
                let urlOptions = { controller: options.controller, query: options.query }
                if (options.click) {
                    urlOptions.query = (options.query is Array) ? options.query[row] : options.query
                    if (options.click is Array) {
                        if (options.click[row] is String) {
                            url = view.makeUrl(options.click[row], r.id, urlOptions)
                        }
                    } else {
                        url = view.makeUrl(options.click, r.id, urlOptions)
                    }
                }
                let odd = options.styleOddRow || "-ejs-oddRow"
                let even = options.styleOddRow || "-ejs-evenRow"
                styleRow = ((row % 2) ? odd : even) || ""
                if (options.styleRows) {
                    styleRow += " " + (options.styleRows[row] || "")
                }
                if (url) {
                    write('    <tr class="' + styleRow + 
                        '" onclick="window.location=\'' + url + '\';">\r\n')
                } else {
                    write('    <tr class="' + styleRow + '">\r\n')
                }

                let col = 0
				for (name in columns) {
                    if (name == null) {
                        continue
                    }
                    let column = columns[name]
                    let styleCell: String = ""

                    if (options.styleColumns) {
                        styleCell = options.styleColumns[col] || ""
                    }
                    if (column.style) {
                        styleCell += " " + column.style
                    }
                    if (options.styleCells && options.styleCells[row]) {
                        styleCell += " " + (options.styleCells[row][col] || "")
                    }
                    styleCell = styleCell.trim()
                    data = view.getValue(r, name, { render: column.render, formatter: column.formatter } )

                    let align = ""
                    if (column.align) {
                        align = ' align="' + column.align + '"'
                    }
                    let cellUrl
                    if (options.click is Array && options.click[0] is Array) {
                        if (options.query is Array) {
                            if (options.query[0] is Array) {
                                urlOptions.query = options.query[row][col]
                            } else {
                                urlOptions.query = options.query[row]
                            }
                        } else {
                            urlOptions.query = options.query
                        }
                        cellUrl = view.makeUrl(options.click[row][col], r.id, urlOptions)
                    }
					styleCell = styleCell.trim()
                    if (cellUrl) {
                        write('    <td class="' + styleCell + '"' + align + 
                            ' xonclick="window.location=\'' + cellUrl + '\';"><a href="' + cellUrl + '">' + 
                            data + '</a></td>\r\n')
                    } else {
                        write('    <td class="' + styleCell + '"' + align + '>' + data + '</td>\r\n')
                    }
                    col++
				}
                row++
				write('    </tr>\r\n')
			}
			write('    </tbody>\r\n  </table>\r\n')
		}

        //  Emit: <input name ="model.name" id="id" class="class" type="text|hidden|password" value="text"
        /**
         *  Render a text input field as part of a form.
         *  @param name Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
         *      input element. If used inside a model form, it is the field name in the model containing the text data to
         *      display. If used without a model, the value to display should be passed via options.value. 
            @param value Text to display
         *  @param options Optional extra options. See $getOptions for a list of the standard options.
         *  @option escape Boolean Escape the text before rendering. This converts HTML reserved tags and delimiters into
         *      an encoded form.
         *  @option style String CSS Style to use for the control
         *  @option visible Boolean Make the control visible. Defaults to true.
         *  @examples
         *      <% text("name") %>
         */
        function text(name: String, value: String, options: Object): Void {
            write('<input name="' + name + '" ' + getOptions(options) + ' type="' + getTextKind(options) + 
                '" value="' + value + '" />')
        }

        // Emit: <textarea name ="model.name" id="id" class="class" type="text|hidden|password" value="text"
        /**
         *  Render a text area
         *  @param name Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
         *      input element. If used inside a model form, it is the field name in the model containing the text data to
         *      display. If used without a model, the value to display should be passed via options.value. 
            @param value Text to display
         *  @option Boolean escape Escape the text before rendering. This converts HTML reserved tags and delimiters into
         *      an encoded form.
         *  @param options Optional extra options. See $getOptions for a list of the standard options.
         *  @option data String URL or action to get data 
         *  @option numCols Number number of text columns
         *  @option numRows Number number of text rows
         *  @option style String CSS Style to use for the control
         *  @option visible Boolean Make the control visible. Defaults to true.
         *  @examples
         *      <% textarea("name") %>
         */
        function textarea(name: String, value: String, options: Object): Void {
            numCols = options.numCols
            if (numCols == undefined) {
                numCols = 60
            }
            numRows = options.numRows
            if (numRows == undefined) {
                numRows = 10
            }
            write('<textarea name="' + name + '" type="' + getTextKind(options) + '" ' + getOptions(options) + 
                ' cols="' + numCols + '" rows="' + numRows + '">' + value + '</textarea>')
        }

        /**
         *  @duplicate ejs.web::View.tree
            @hide
         */
        function tree(initialData: Array, options: Object): Void {
            throw 'HtmlConnector control "tree" not implemented.'
        }

        private function getTextKind(options): String {
            var kind: String

            if (options.password) {
                kind = "password"
            } else if (options.hidden) {
                kind = "hidden"
            } else {
                kind = "text"
            }
            return kind
        }

		private function getOptions(options: Object): String
            view.getOptions(options)

        private function write(str: String): Void
            view.write(str)
	}
}


/*
 *	@copy	default
 *	
 *	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
 *	Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
 *	
 *	This software is distributed under commercial and open source licenses.
 *	You may use the GPL open source license described below or you may acquire 
 *	a commercial license from Embedthis Software. You agree to be fully bound 
 *	by the terms of either license. Consult the LICENSE.TXT distributed with 
 *	this software for full details.
 *	
 *	This software is open source; you can redistribute it and/or modify it 
 *	under the terms of the GNU General Public License as published by the 
 *	Free Software Foundation; either version 2 of the License, or (at your 
 *	option) any later version. See the GNU General Public License for more 
 *	details at: http://www.embedthis.com/downloads/gplLicense.html
 *	
 *	This program is distributed WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	
 *	This GPL license does NOT permit incorporating this software into 
 *	proprietary programs. If you are unable to comply with the GPL, you must
 *	acquire a commercial license to use this software. Commercial licenses 
 *	for this software and support services are available from Embedthis 
 *	Software at http://www.embedthis.com 
 *	
 *	@end
 */
