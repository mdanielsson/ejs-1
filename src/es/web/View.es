/**
    View.es -- View class as part of the Ejscript MVC framework
 */
module ejs.web {

    require ejs.db

    /**
        Base class for web framework views. This class provides the core functionality for all Ejscript view web pages.
        Ejscript web pages are compiled to create a new View class which extends the View base class. In addition to
        the properties defined by this class, user view classes will inherit at runtime all public properites of the
        current controller object. View objects are not instantiated manually by users but are created internally 
        by the web framework.
        @spec ejs
        @stability prototype
     */
    dynamic class View {
        /*
            Define properties and functions are (by default) in the ejs.web namespace rather than public to avoid clashes.
         */
        use default namespace "ejs.web"

        /**
            Current controller
         */
        var controller: Controller

        /*
            Current model being used inside a form
         */
        private var currentModel: Record

        /*
            Configuration from the applications config *.ecf
         */
        private var config: Object

        /*
            Id generator
         */
        private var nextId: Number = 0

        /** @hide */
        function getNextId(): Number {
            return "id_" + nextId++
        }

        /**
            Constructor method to initialize a new View
            @param controller Controller to manage this view
         */
        function View(controller: Controller) {
            this.controller = controller
            this.config = controller.config
            view = this
        }

        /**
            Process and emit a view to the client. Overridden by the views invoked by controllers.
         */
        public function render(): Void {}

        /************************************************ View Helpers ****************************************************/

        /**
            Render an asynchronous (ajax) form.
            @param action Action to invoke when the form is submitted. Defaults to "create" or "update" depending on 
                whether the field has been previously saved.
            @param record Model record to edit
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option url String Use a URL rather than action and controller for the target url.
         */
        function aform(action: String, record: Object = null, options: Object = {}): Void {
            if (record == null) {
                record = new LocalModel
            }
            currentModel = record
            formErrors(record)
            options = setOptions("aform", options)
            if (options.method == null) {
                options.method = "POST"
            }
            if (action == null) {
                action = "update"
            }
            let connector = getConnector("aform", options)
            options.url = makeUrl(action, record.id, options)
            connector.aform(record, options.url, options)
        }

        /** 
            Emit an asynchronous (ajax) link to an action. The URL is constructed from the given action and the 
                current controller. The controller may be overridden by setting the controller option.
            @param text Link text to display
            @param action Action to invoke when the link is clicked
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option controller String Name of the target controller for the given action
            @option url String Use a URL rather than action and controller for the target url.
         */
        function alink(text: String, action: String = null, options: Object = {}): Void {
            if (action == null) {
                action = text.split(" ")[0].toLower()
            }
            options = setOptions("alink", options)
            if (options.method == null) {
                options.method = "POST"
            }
            let connector = getConnector("alink", options)
            options.url = makeUrl(action, options.id, options)
            connector.alink(text, options.url, options)
        }

        /**
            Render a form button. This creates a button suitable for use inside an input form. When the button is clicked,
            the input form will be submitted.
            @param value Text to display in the button.
            @param name Form name of button.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            Examples:
                button("OK", "commit")
                button("Cancel", "commit")
         */
        function button(value: String, buttonName: String = null, options: Object = {}): Void {
            options = setOptions("button", options)
            if (buttonName == null) {
                buttonName = value.toLower()
            }
            let connector = getConnector("button", options)
            connector.button(value, buttonName, options)
        }

        /**
            Render a link button. This creates a button suitable for use outside an input form. When the button is clicked,
            the associated URL will be invoked.
            @param text Text to display in the button.
            @param action Target action to invoke when the button is clicked.
            @param url Override target URL to use instead of action.
            @param options Optional extra options. See $getOptions for a list of the standard options.
         */
        function buttonLink(text: String, action: String, options: Object = {}): Void {
            options = setOptions("buttonLink", options)
            let connector = getConnector("buttonLink", options)
            connector.buttonLink(text, makeUrl(action, options.id, options), options)
        }

        /**
            Render a chart. The chart control can display static or dynamic tabular data. The client chart control manages
            sorting by column, dynamic data refreshes, pagination and clicking on rows.
            @param initialData Optional initial data for the control. The data option may be used with the refresh option to 
                dynamically refresh the data.
            @param options Object Optional extra options. See also $getOptions for a list of the standard options.
            @option columns Object hash of column entries. Each column entry is in-turn an object hash of options. If unset, 
                all columns are displayed using defaults.
            @option kind String Type of chart. Select from: piechart, table, linechart, annotatedtimeline, guage, map, 
                motionchart, areachart, intensitymap, imageareachart, barchart, imagebarchart, bioheatmap, columnchart, 
                linechart, imagelinechart, imagepiechart, scatterchart (and more)
            @option onClick String Action or URL to invoke when a chart element  is clicked.
            @example
                <% chart(null, { data: "getData", refresh: 2" }) %>
                <% chart(data, { onClick: "action" }) %>
         */
        function chart(initialData: Array, options: Object = {}): Void {
            let connector = getConnector("chart", options)
            connector.chart(initialData, options)
        }

        /**
            Render an input checkbox. This creates a checkbox suitable for use within an input form. 
            @param field Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
                input element. If used inside a model form, it is the field name in the model containing the checkbox
                value to display. If used without a model, the value to display should be passed via options.value. 
            @param choice Value to submit if checked. Defaults to "true"
            @param options Optional extra options. See $getOptions for a list of the standard options.
         */
        function checkbox(field: String, choice: String = "true", options: Object = {}): Void {
            options = setOptions(field, options)
            let value = getValue(currentModel, field, options)
            let connector = getConnector("checkbox", options)
            connector.checkbox(options.fieldName, value, choice, options)
        }

        /**
            End an input form. This closes an input form initiated by calling the $form method.
         */
        function endform(): Void {
            let connector = getConnector("endform", null)
            connector.endform()
            currentModel = undefined
        }

        /**
            Render a form.
            @param action Action to invoke when the form is submitted. Defaults to "create" or "update" depending on 
                whether the field has been previously saved.
            @param record Model record to edit
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option url String Use a URL rather than action and controller for the target url.
         */
        function form(action: String, record: Object = null, options: Object = {}): Void {
            currentModel = record
            formErrors(record)
            options = setOptions("form", options)
            if (options.method == null) {
                options.method = "POST"
            }
            if (action == null) {
                action = "update"
            }
            let connector = getConnector("form", options)
            options.url = makeUrl(action, (record) ? record.id : null, options)
            connector.form(record, options.url, options)
        }

        /**
            Render an image control
            @param src Optional initial source name for the image. The data option may be used with the refresh option to 
                dynamically refresh the data.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @examples
                <% image("myPic.gif") %>
                <% image("myPic.gif", { data: "getData", refresh: 2, style: "myStyle" }) %>
         */
        function image(src: String, options: Object = {}): Void {
            let connector = getConnector("image", options)
            connector.image(src, options)
        }

        /**
            Render a clickable image. This creates an clickable image suitable for use outside an input form. 
            When the image is clicked, the associated URL will be invoked.
            @param image Optional initial source name for the image. The data option may be used with the refresh option to 
                dynamically refresh the data.
            @param action Target action to invoke when the image is clicked.
            @param url Override target URL to use instead of action.
            @param options Optional extra options. See $getOptions for a list of the standard options.
         */
        function imageLink(image: String, action: String, options: Object = {}): Void {
            options = setOptions("imageLink", options)
            let connector = getConnector("imageLink", options)
            connector.imageLink(text, makeUrl(action, options.id, options), options)
        }

        /**
            Render an input field as part of a form. This is a smart input control that will call the appropriate
                input control based on the model field data type.
            @param field Model field name containing the text data for the control.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @examples
                <% input(modelFieldName) %>
                <% input(null, { options }) %>
         */
        function input(field: String, options: Object = {}): Void {
            datatype = currentModel.getColumnType(field)
            switch (datatype) {
            case "binary":
            case "date":
            case "datetime":
            case "decimal":
            case "float":
            case "integer":
            case "number":
            case "string":
            case "time":
            case "timestamp":
                text(field, options)
                break

            case "text":
                textarea(field, options)
                break

            case "boolean":
                checkbox(field, "true", options)
                break

            default:
                throw "input control: Unknown field type: " + datatype + " for field " + field
            }
        }

        /**
            Render a text label field. This renders an output-only text field. Use text() for input fields.
            @param text Optional initial data for the control. The data option may be used with the refresh option to 
                dynamically refresh the data.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @examples
                <% label("Hello World") %>
                <% label(null, { data: "getData", refresh: 2, style: "myStyle" }) %>
         */
        function label(text: String, options: Object = {}): Void {
            options = setOptions("label", options)
            let connector = getConnector("label", options)
            connector.label(text, options)
        }

        /** 
            Emit a link to an action. The URL is constructed from the given action and the current controller. The controller
            may be overridden by setting the controller option.
            @param text Link text to display
            @param action Action to invoke when the link is clicked
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option controller String Name of the target controller for the given action
            @option url String Use a URL rather than action and controller for the target url.
         */
        function link(text: String, action: String = null, options: Object = {}): Void {
            if (action == null) {
                action = text.split(" ")[0].toLower()
            }
            options = setOptions("link", options)
            let connector = getConnector("link", options)
            connector.link(text, makeUrl(action, options.id, options), options)
        }

        /** 
            Emit an application relative link. If invoking an action, it is safer to use \a action.
            @param text Link text to display
            @param url Action or URL to invoke when the link is clicked
            @param options Optional extra options. See $getOptions for a list of the standard options.
         */
        function extlink(text: String, url: String, options: Object = {}): Void {
            let connector = getConnector("extlink", options)
            connector.extlink(text, controller.appUrl + url, options)
        }

        /**
            Emit a selection list. 
            @param field Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
                input element. If used inside a model form, it is the field name in the model containing the list item to
                select. If used without a model, the value to select should be passed via options.value. 
            @param choices Choices to select from. This can be an array list where each element is displayed and the value 
                returned is an element index (origin zero). It can also be an array of array tuples where the second 
                tuple entry is the value to display and the first is the value to send to the app. Or it can be an 
                array of objects such as those returned from a table lookup. If choices is null, the $field value is 
                used to construct a model class name to use to return a data grid containing an array of row objects. 
                The first non-id field is used as the value to display.
            @params options Extra options
            Examples:
                list("stockId", Stock.stockList) 
                list("low", ["low", "med", "high"])
                list("low", [["3", "low"], ["5", "med"], ["9", "high"]])
                list("low", [{low: 3{, {med: 5}, {high: 9}])
                list("Stock Type")                          Will invoke StockType.findAll() to do a table lookup
         */
        function list(field: String, choices: Object = null, options: Object = {}): Void {
            options = setOptions(field, options)
            if (choices == null) {
                modelTypeName = field.replace(/\s/, "").toPascal()
                modelTypeName = modelTypeName.replace(/Id$/, "")
                if (global[modelTypeName] == undefined) {
                    throw new Error("Can't find model to create list data: " + modelTypeName)
                }
                choices = global[modelTypeName].findAll()
            }
            let value = getValue(currentModel, field, options)
            let connector = getConnector("list", options)
            connector.list(options.fieldName, choices, value, options)
        }

        /**
            Emit a mail link
            @param nameText Recipient name to display
            @param address Mail recipient address
            @param options Optional extra options. See $getOptions for a list of the standard options.
         */
        function mail(nameText: String, address: String, options: Object = {}): Void  {
            let connector = getConnector("mail", options)
            connector.mail(nameText, address, options)
        }

        /** 
            Emit a progress bar. 
            @param initialData Optional initial data for the control. The data option may be used with the refresh option 
                to dynamically refresh the data. Progress data is a simple Number in the range 0 to 100 and represents 
                a percentage completion value.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @example
                <% progress(null, { data: "getData", refresh: 2" }) %>
            @hide
         */
        function progress(initialData: Object, options: Object = {}): Void {
            let connector = getConnector("progress", options)
            connector.progress(initialData, options)
        }

        /** 
            Emit a radio autton. The URL is constructed from the given action and the current controller. The controller
                may be overridden by setting the controller option.
            @param field Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
                input element. If used inside a model form, it is the field name in the model containing the radio data to
                display. If used without a model, the value to display should be passed via options.value. 
            @param choices Array or object containing the option values. If array, each element is a radio option. If an 
                object hash, then they property name is the radio text to display and the property value is what is returned.
            @param action Action to invoke when the button is clicked or invoked
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option controller String Name of the target controller for the given action
            @option value String Name of the option to select by default
            @example
                radio("priority", ["low", "med", "high"])
                radio("priority", {low: 0, med: 1, high: 2})
                radio(priority, Message.priorities)
         */
        function radio(field: String, choices: Object, options: Object = {}): Void {
            options = setOptions(field, options)
            let value = getValue(currentModel, field, options)
            let connector = getConnector("radio", options)
            connector.radio(options.fieldName, value, choices, options)
        }

        /** 
            Emit a script link.
            @param url URL for the script to load
            @param options Optional extra options. See $getOptions for a list of the standard options.
         */
        function script(url: Object, options: Object = {}): Void {
            let connector = getConnector("script", options)
            if (url is Array) {
                for each (u in url) {
                    connector.script(controller.appUrl + "/" + u, options)
                }
            } else {
                connector.script(controller.appUrl + "/" + url, options)
            }
        }

        /** 
            Emit a status control area. 
            @param initialData Optional initial data for the control. The data option may be used with the refresh option to 
                dynamically refresh the data. Status data is a simple String. Status messages may be updated by calling the
                \a statusMessage function.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @example
                <% status("Initial Status Message", { data: "getData", refresh: 2" }) %>
            @hide
         */
        function status(initialData: Object, options: Object = {}): Void {
            let connector = getConnector("status", options)
            connector.status(initialData, options)
        }

        /** 
            Emit a style sheet link.
            @param url Stylesheet url or array of stylesheets
            @param options Optional extra options. See $getOptions for a list of the standard options.
         */
        function stylesheet(url: Object, options: Object = {}): Void {
            let connector = getConnector("stylesheet", options)
            if (url is Array) {
                for each (u in url) {
                    connector.stylesheet(controller.appUrl + "/" + u, options)
                }
            } else {
                connector.stylesheet(controller.appUrl + "/" + url, options)
            }
        }

        /**
            Render a table. The table control can display static or dynamic tabular data. The client table control manages
                sorting by column, dynamic data refreshes, pagination and clicking on rows. If the table supplies a URL
                or action for the data parameter, the table data is retrieved asynchronously using Ajax requests on that
                action/URL value. The action routine should call the table() control to render the data and must set the
                ajax option to true.  
            @param data Data for the control or URL/action to supply data. If data is a String, it is interpreted as a URL
                or action that will be invoked to supply HTML for the table. In this case, the refresh option defines 
                how frequently to refresh the table data. The data parameter can also be a grid of data, ie. an Array of 
                objects where each object represents the data for a row. The column names are the object property names 
                and the cell text is the object property values. The data parameter can also be a model instance.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option ajax Set to true if the table control is being invoked as part of an Ajax data refresh.
            @option click String Action or URL to invoke an element in the table is clicked. The click arg can be is 
                a String to apply to all cells, a single-dimension array of strings for per-row URLs, and a 
                two-dimension array for per cell URLs (order is row/column).
            @option columns Object hash of column entries. Each column entry is in-turn an object hash of options. If unset, 
                all columns are displayed using defaults. Column options: align, formatter, header, render, sort, sortOrder, 
                style, width.
            @option pageSize Number Number of rows to display per page. Omit or set to <= 0 for unlimited. 
                Defaults to unlimited.
            @option pivot Boolean Pivot the table by swaping rows for columns and vice-versa
            @option query URL query string to add to click URLs. Can be a single-dimension array for per-row query 
                strings or a two-dimensional array for per cell (order is row/column).
            @option showHeader Boolean Control if column headings are displayed.
            @option showId Boolean If a columns option is not provided, the id column is normally hidden. 
                To display, set showId to be true.
            @option sort String Enable row sorting and define the column to sort by.
            @option sortOrder String Default sort order. Set to "ascending" or "descending".Defaults to ascending.
            @option style String CSS style to use for the table.
            @option styleColumns Array of styles to use for the table body columns. Can also use the style option in the
                columns option.
            @option styleBody String CSS style to use for the table body cells
            @option styleHeader String CSS style to use for the table header.
            @option styleRows Array of styles to use for the table body rows
            @option styleOddRow String CSS style to use for odd data rows in the table
            @option styleEvenRow String CSS style to use for even data rows in the table
            @option title String Table title
         
            Column options:
            <ul>
            <li>align</li>
            <li>format</li>
            <li>formatter</li>
            <li>header</li>
            <li>render</li>
            <li>sort String Define the column to sort by and the sort order. Set to "ascending" or "descending". 
                Defaults to ascending.</li>
            <li>style</li>
            </ul>
        
            @example
                <% table("getData", { refresh: 2, pivot: true" }) %>
                <% table(gridData, { click: "edit" }) %>
                <% table(Table.findAll()) %>
                <% table(gridData, {
                    click: "edit",
                    sort: "Product",
                    columns: {
                        product:    { header: "Product", width: "20%" }
                        date:       { format: date('%m-%d-%y) }
                    }
                 }) %>
         */
        function table(data, options: Object = {}): Void {
            options = setOptions("table", options)
            let connector = getConnector("table", options)
            if (options.pivot) {
                data = pivot(data)
            }
            connector.table(data, options)
        }

        /**
            Render a tab control. The tab control can display static or dynamic tree data.
            @param initialData Optional initial data for the control. The data option may be used with the refresh option to 
                dynamically refresh the data. Tab data is an array of objects -- one per tab. For example:
                [{"Tab One Label", "action1"}, {"Tab Two Label", "action2"}]
            @param options Optional extra options. See $getOptions for a list of the standard options.
         */
        function tabs(initialData: Array, options: Object = {}): Void {
            let connector = getConnector("tabs", options)
            connector.tabs(initialData, options)
        }

        /**
            Render a text input field as part of a form.
            @param field Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
                input element. If used inside a model form, it is the field name in the model containing the text data to
                display. If used without a model, the value to display should be passed via options.value. 
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option escape Boolean Escape the text before rendering. This converts HTML reserved tags and delimiters into
                an encoded form.
            @option style String CSS Style to use for the control
            @option visible Boolean Make the control visible. Defaults to true.
            @examples
                <% text("name") %>
         */
        function text(field: String, options: Object = {}): Void {
            options = setOptions(field, options)
            let value = getValue(currentModel, field, options)
            let connector = getConnector("text", options)
            connector.text(options.fieldName, value, options)
        }

        /**
            Render a text area
            @param field Name of the field to display. This is used to create a HTML "name" and "id" attribute for the 
                input element. If used inside a model form, it is the field name in the model containing the text data to
                display. If used without a model, the value to display should be passed via options.value. 
            @option Boolean escape Escape the text before rendering. This converts HTML reserved tags and delimiters into
                an encoded form.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option data String URL or action to get data 
            @option numCols Number number of text columns
            @option numRows Number number of text rows
            @option style String CSS Style to use for the control
            @option visible Boolean Make the control visible. Defaults to true.
            @examples
                <% textarea("name") %>
         */
        function textarea(field: String, options: Object = {}): Void {
            options = setOptions(field, options)
            let value = getValue(currentModel, field, options)
            let connector = getConnector("textarea", options)
            connector.textarea(options.fieldName, value, options)
        }

        /**
            Render a tree control. The tree control can display static or dynamic tree data.
            @param initialData Optional initial data for the control. The data option may be used with the refresh option to 
                dynamically refresh the data. The tree data is an XML document.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option data URL or action to get data 
            @option refresh If set, this defines the data refresh period in seconds. Only valid if the data option is defined
            @option style String CSS Style to use for the control
            @option visible Boolean Make the control visible. Defaults to true.
            @hide
         */
        function tree(initialData: XML, options: Object = {}): Void {
            let connector = getConnector("tree", options)
            connector.tree(initialData, options)
        }

        /*********************************** Wrappers for Control Methods ***********************************/
        /** 
            Emit a flash message area. 
            @param kinds Kinds of flash messages to display. May be a single string 
                ("error", "inform", "message", "warning"), an array of strings or null. If set to null (or omitted), 
                then all flash messages will be displayed.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @option retain Number. Number of seconds to retain the message. If <= 0, the message is retained until another
                message is displayed. Default is 0.
            @example
                <% flash("status") %>
                <% flash() %>
                <% flash(["error", "warning"]) %>
         */
        function flash(kinds = null, options: Object = {}): Void {
            options = setOptions("flash", options)

            let cflash = controller.flash
            if (cflash == null || cflash.length == 0) {
                return
            }

            let msgs: Object
            if (kinds is String) {
                msgs = {}
                msgs[kinds] = cflash[kinds]

            } else if (kinds is Array) {
                msgs = {}
                for each (kind in kinds) {
                    msgs[kind] = cflash[kind]
                }

            } else {
                msgs = cflash
            }

            for (kind in msgs) {
                let msg: String = msgs[kind]
                if (msg && msg != "") {
                    let connector = getConnector("flash", options)
                    options.style = "-ejs-flash -ejs-flash" + kind.toPascal()
                    connector.flash(kind, msg, options)
                }
            }
        }

        private function formErrors(model): Void {
            if (!model) {
                return
            }
            let errors = model.getErrors()
            if (errors) {
                write('<div class="-ejs-formError"><h2>The ' + Reflect(model).name.toLower() + ' has ' + 
                    errors.length + (errors.length > 1 ? ' errors' : ' error') + ' that ' +
                    ((errors.length > 1) ? 'prevent' : 'prevents') + '  it being saved.</h2>\r\n')
                write('    <p>There were problems with the following fields:</p>\r\n')
                write('    <ul>\r\n')
                for (e in errors) {
                    write('        <li>' + e.toPascal() + ' ' + errors[e] + '</li>\r\n')
                }
                write('    </ul>\r\n')
                write('</div>\r\n')
            }
        }

        /*********************************** Wrappers for Control Methods ***********************************/
        /**
            Enable session control. This enables session state management for this request and other requests from 
            the browser. If a session has not already been created, this call creates a session and sets the $sessionID 
            property in the request object. If a session already exists, this call has no effect. A cookie containing a 
            session ID is automatically created and sent to the client on the first response after creating the session. 
            If SessionAutoCreate is defined in the configuration file, then sessions will automatically be created for 
            every web request and your Ejscript web pages do not need to call createSession. Multiple requests may be 
            sent from a client's browser at the same time.  Ejscript will ensure that accesses to the sesssion object 
            are correctly serialized. 
            @param timeout Optional timeout for the session in seconds. If ommitted the default timeout is used.
         */
        function createSession(timeout: Number = 0): Void
            controller.createSession(timeout)

        /**
            Destroy a session. This call destroys the session state store that is being used for the current client. If no 
            session exists, this call has no effect.
         */
        function destroySession(): Void
            controller.destroySession()

        /** 
            HTML encode the arguments
            @param args Variable arguments that will be converted to safe html
            @return A string containing the encoded arguments catenated together
         */
        function html(...args): String
            controller.html(args)

        /**
            @duplicate ejs.web::Controller.makeUrl
         */
        function makeUrl(action: String, id: String = null, options: Object = {}): String
            controller.makeUrl(action, id, options)

        /**
            Redirect the client to a new URL. This call redirects the client's browser to a new location specified by 
            the @url. Optionally, a redirection code may be provided. Normally this code is set to be the HTTP 
            code 302 which means a temporary redirect. A 301, permanent redirect code may be explicitly set.
            @param url Url to redirect the client to
            @param code Optional HTTP redirection code
         */
        function redirectUrl(url: String, code: Number = 302): Void
            controller.redirectUrl(url, code)

        /**
            Redirect to the given action
            @param action Action URL to redirect to
            @param id Query ID value
            @param options Extra options
            @option id controller
         */
        function redirect(action: String, id: String = null, options: Object = {}): Void
            redirectUrl(makeUrl(action, id, options))

        /**
            Define a cookie header to include in the reponse
            @hide
         */
        function setCookie(name: String, value: String, path: String = null, domain: String = null, 
                lifetime: Number = 0, secure: Boolean = false): Void
            controller.setCookie(name, value, path, domain, lifetime, secure)

        /**
            of the format "keyword: value". If a header has already been defined and \a allowMultiple is false, 
            the header will be overwritten. If \a allowMultiple is true, the new header will be appended to the 
            response headers and the existing header will also be output. NOTE: case does not matter in the header keyword.
            @param key Header key string
            @param value Header value string
            @param allowMultiple If false, overwrite existing headers with the same keyword. If true, all headers are output.
         */
        function setHeader(key: String, value: String, allowMultiple: Boolean = false): Void
            controller.setHeader(key, value, allowMultiple)

        /**
            Set the HTTP response status code
            @param code HTTP response code to define
         */
        function setHttpCode(code: Number): Void
            controller.setHttpCode(code)

        /**
            Set the response body mime type
            @param format Mime type for the response. For example "text/plain".
         */
        function setMimeType(format: String): Void
            controller.setMimeType(format);

        /**
            @duplicate ejs.web::Controller.write
         */
        function write(...args): Void
            controller.write(args)

        /**
            Write HTML escaped text to the client. This call writes the arguments back to the client's browser after mapping
            all HTML control sequences into safe alternatives. The arguments are converted to strings before writing back to 
            the client and then escaped. The text, written using write, will be buffered up to a configurable maximum. This 
            allows text to be written prior to setting HTTP headers with setHeader.
            @param args Text or objects to write to the client
         */
        function writeHtml(...args): Void
            controller.write(html(args))

        /**
            @duplicate ejs.web::Controller.writeRaw
            @hide
         */
        function writeRaw(...args): Void 
            controller.writeRaw(args)

        /**
            Dump objects for debugging
            @param args List of arguments to print.
         */
        function d(...args): Void {
            write('<pre>\r\n')
            for each (var e: Object in args) {
                write(serialize(e) + "\r\n")
            }
            write('</pre>\r\n')
        }

        /******************************************** Support ***********************************************/

        private function addHelper(fun: Function, overwrite: Boolean = false): Void {
            let name: String = Reflect(fun).name
            if (this[name] && !overwrite) {
                throw new Error('Helper ' + name + ' already exists')
            }
            this[name] = fun
        }

        /*
            Get the view connector to render a control
         */
        private function getConnector(kind: String, options: Object) {
            var connectorName: String
            if (options && options["connector"]) {
                connectorName = options["connector"]
            } else {
                connectorName =  config.view.connectors[kind]
            }
            if (connectorName == undefined || connectorName == null) {
                connectorName =  config.view.connectors["rest"]
                if (connectorName == undefined || connectorName == null) {
                    connectorName = "html"
                }
                config.view.connectors[kind] = connectorName
            }
            let name: String = (connectorName + "Connector").toPascal()
            try {
                return new global[name](controller)
            } catch (e: Error) {
                throw new Error("Undefined view connector: " + name)
            }
        }


        /*
            Update the options based on the model and field being considered
         */
        private function setOptions(field: String, options: Object): Object {
            if (options == null) {
                options = {}
            }
            if (options.fieldName == null) {
                if (currentModel) {
                    options.fieldName = Reflect(currentModel).name.toCamel() + '.' + field
                } else {
                    options.fieldName = field;
                }
            }
            if (options.id == null) {
                if (currentModel && currentModel.id) {
                    options.id = field + '_' + currentModel.id
                }
            }
            options.style ||= field
            if (currentModel && currentModel.hasError(field)) {
                options.style += " -ejs-fieldError"
            }
            return options
        }

        /**
            Format the data for presentation
            @hide
         */
        function getValue(model: Object, field: String, options: Object): String {
            let value
            if (model && field) {
                value = model[field]
            }
            if (value == null || value == undefined) {
                if (options.value) {
                    value = options.value
                }
            }
            if (value == null || value == undefined) {
                value = model
                if (value) {
                    for each (let part in field.split(".")) {
                        value = value[part]
                    }
                }
                if (value == null || value == undefined) {
                    value = ""
                }
            }
            if (options.render != undefined && options.render is Function) {
                fun = options.render
                result = fun(value, model, field).toString()
                return result
            }
            if (options.formatter != undefined && options.formatter is Function) {
                return options.formatter(value).toString()
            }
            let typeName = Reflect(value).typeName

            let fmt
            if (config.view && config.view.formats) {
                fmt = config.view.formats[typeName]
            }
            if (fmt == undefined || fmt == null || fmt == "") {
                return value.toString()
            }
            switch (typeName) {
            case "Date":
                return new Date(value).format(fmt)
            case "Number":
                return fmt.format(value)
            }
            return value.toString()
        }

        /**
            Temporary helper function to format the date. 
            @param fmt Format string
            @returns a function that will return formatted string when invoked
            @stability prototype
            @hide
         */
        function date(fmt: String): Function {
            return function (data: String): String {
                return new Date(data).format(fmt)
            }
        }

        /**
            Temporary helper function to format a number as currency. 
            @param fmt Format string
            @returns a function that will return formatted string when invoked
            @stability prototype
            @hide
         */
        function currency(fmt: String): Function {
            return function (data: String): String {
                return fmt.format(data)
            }
        }

        /**
            Temporary helper function to format a number. 
            @param fmt Format string
            @returns a function that will return formatted string when invoked
            @stability prototype
            @hide
         */
        function number(fmt: String): Function {
            return function (data: String): String {
                return fmt.format(data)
            }
        }

        /*
            Mapping of helper options to HTML attributes ("" value means don't map the name)
         */
        private static const htmlOptions: Object = { 
            background: "", color: "", id: "", height: "", method: "", size: "", 
            style: "class", visible: "", width: "",
            "remote": "data-remote",
        }

        /**
            Map options to a HTML attribute string.
            @param options Optional extra options. See $getOptions for a list of the standard options.
            @returns a string containing the HTML attributes to emit.
            @option background String Background color. This is a CSS RGB color specification. For example "FF0000" for red.
            @option color String Foreground color. This is a CSS RGB color specification. For example "FF0000" for red.
            @option data String URL or action to get live data. The refresh option specifies how often to invoke
                fetch the data.
            @option id String Browser element ID for the control
            @option escape Boolean Escape the text before rendering. This converts HTML reserved tags and delimiters into
                an encoded form.
            @option height (Number|String) Height of the table. Can be a number of pixels or a percentage string. 
                Defaults to unlimited.
            @option method String HTTP method to invoke. May be: GET, POST, PUT or DELETE.
            @option refresh If set, this defines the data refresh period in milliseconds. Only valid if the data option 
                is defined.
            @option size (Number|String) Size of the element.
            @option style String CSS Style to use for the table.
            @option value Default data value to use for the control if not supplied by other means.
            @option visible Boolean Make the control visible. Defaults to true.
            @option width (Number|String) Width of the table or column. Can be a number of pixels or a percentage string. 
         */
        function getOptions(options: Object): String {
            if (!options) {
                return ''
            }
            let result: String = ""
            for (let option: String in options) {
                let mapped = htmlOptions[option]
                if (mapped || mapped == "") {
                    if (mapped == "") {
                        /* No mapping, keep the original option name */
                        mapped = option
                    }
                    result += ' ' +  mapped + '="' + options[option] + '"'

                } else if (option.startsWith("data-")) {
                    result += ' ' +  option + '="' + options[option] + '"'
                }
            }
            return result + " "
        }

/*
        private function sortFn(a: Array, ind1: Number, ind2: Number) {
            if (a < b) {
                return -1
            } else if (a > b) {
                return 1
            }
            return 0
        }

        private function sort(data: Array): Array {
            data["sortBy"] = controller.params.sort
            data["sortOrder"] = controller.params.sortOrder
            data.sort(sortFn)
            let tmp = data[0]
            data[0] = data[1]
            data[1] = tmp
            return data;
        }
*/

        /*
            Pivot the data grid. Returns a new grid, original not modified.
         */
        private function pivot(grid: Array, options: Object = {}) {
            if (!grid || grid.length == 0) return grid
            let headers = []
            let i = 0
            for (name in grid[0]) {
                headers[i++] = name
            }
            let table = []
            let row = 0
            table = []
            for each (name in headers) {
                let r = {}
                i = 1
                r[0] = name
                for (j = 0; j < grid.length; j++) {
                    r[i++] = grid[j][name]
                }
                table[row++] = r
            }
            return table
        }

        private function filter(data: Array): Array {
            data = data.clone()
            pattern = controller.params.filter.toLower()
            for (let i = 0; i < data.length; i++) {
                let found: Boolean = false
                for each (f in data[i]) {
                    if (f.toString().toLower().indexOf(pattern) >= 0) {
                        found = true
                    }
                }
                if (!found) {
                    data.remove(i, i)
                    i--
                }
            }
            return data
        }
    }

    internal dynamic class LocalModel implements Record {
        function LocalModel(fields: Object = null) {
            initialize(fields)
        }
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
