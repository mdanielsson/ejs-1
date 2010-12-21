/**
    Record.es -- Record class
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/*
    Notes:
    - Don't use getters/setters in this module. This avoids collisions with record fields. Even with namespaces, 
      this can be a conceptual problem for users.
 */

module ejs.db {
    /**
        Database record class. A record instance corresponds to a row in the database. This class provides a low level 
        Object Relational Mapping (ORM) between the database and Ejscript objects. This class provides methods to create,
        read, update and delete rows in the database. When read or initialized object properties are dynamically created 
        in the Record instance for each column in the database table. Users should subclass the Record class for each 
        database table to manage.
        @spec ejs
        @stability evolving
     */
    class Record {

        use default namespace "ejs.db"
        use namespace "ejs.db.int"
        
        /** @hide */
        static "ejs.db.int" var  _assocName: String        //  Name for use in associations. Lower case class name
        /** @hide */
        static "ejs.db.int" var  _belongsTo: Array         //  List of belonging associations
        /** @hide */
        static "ejs.db.int" var  _className: String        //  Model class name
        /** @hide */
        static "ejs.db.int" var  _columns: Object          //  List of columns in this database table
        /** @hide */
        static "ejs.db.int" var  _hasOne: Array            //  List of 1-1 containment associations
        /** @hide */
        static "ejs.db.int" var  _hasMany: Array           //  List of 1-many containment  associations

        /** @hide */
        static "ejs.db.int" var  _db: Database             //  Hosting database
        /** @hide */
        static "ejs.db.int" var  _foreignId: String        //  Camel case class name with "Id". (userCartId))
        /** @hide */
        static "ejs.db.int" var  _keyName: String          //  Name of the key column (typically "id")
        /** @hide */
        static "ejs.db.int" var  _tableName: String        //  Name of the database table. Plural, PascalCase
        /** @hide */
        static "ejs.db.int" var  _trace: Boolean           //  Trace database SQL statements
        /** @hide */
        static "ejs.db.int" var  _validations: Array

        /** @hide */
        static "ejs.db.int" var  _beforeFilters: Array     //  Filters that run before saving data
        /** @hide */
        static "ejs.db.int" var  _afterFilters: Array      //  Filters that run after saving data
        /** @hide */
        static "ejs.db.int" var  _wrapFilters: Array       //  Filters that run before and after saving data

        /** @hide */
        "ejs.db.int" var _keyValue: Object                 //  Record key column value
        /** @hide */
        "ejs.db.int" var _errors: Object                   //  Error message aggregation
        /** @hide */
        "ejs.db.int" var _cacheAssoc: Object               //  Cached association data

        /** @hide */
        static var ErrorMessages = {
            accepted: "must be accepted",
            blank: "can't be blank",
            confirmation: "doesn't match confirmation",
            empty: "can't be empty",
            invalid: "is invalid",
            missing: "is missing",
            notNumber: "is not a number",
            notUnique: "is not unique",
            taken: "already taken",
            tooLong: "is too long",
            tooShort: "is too short",
            wrongLength: "wrong length",
            wrongFormat: "wrong format",
        }

        /*
            Initialize the model. This should be called by the model as its very first call.
         */
        _keyName = "id"
        _className = Reflect(this).name
        _assocName = _className.toCamel()
        _foreignId = _className.toCamel() + _keyName.toPascal()
        _tableName = plural(_className).toPascal()

        /**
            Construct a new record instance. This is really a constructor function, because the Record class is 
            implemented by user models, no constructor will be invoked when a new user model is instantiated. 
            The record may be initialized by optionally supplying field data. However, the record will not be 
            written to the database until $save is called. To read data from the database into the record, use 
            one of the $find methods.
            @param fields An optional object set of field names and values may be supplied to initialize the record.
         */
        function Record(fields: Object = null) {
            initialize(fields)
        }

        /** @hide */
        function initialize(fields: Object = null): Void {
            if (fields) for (let field in fields) {
                this."public"::[field] = fields[field]
            }
        }

        /**
            Run filters after saving data
            @param fn Function to run
            @param options - reserved
         */
        static function afterFilter(fn, options: Object = null): Void {
            _afterFilters ||= []
            _afterFilters.append([fn, options])
        }

        /**
            Run filters before saving data
            @param fn Function to run
            @param options - reserved
         */
        static function beforeFilter(fn, options: Object = null): Void {
            _beforeFilters ||= []
            _beforeFilters.append([fn, options])
        }

        /**
            Define a belonging reference to another model class. When a model belongs to another, it has a foreign key
            reference to another class.
            @param owner Referenced model class that logically owns this model.
            @param options Optional options hash
            @option className Name of the class
            @option foreignKey Key name for the foreign key
            @option conditions SQL conditions for the relationship to be satisfied
         */
        static function belongsTo(owner, options: Object = null): Void {
            _belongsTo ||= []
            _belongsTo.append(owner)
        }

        /*
            Read a single record of kind "model" by the given "key". Data is cached for subsequent reuse.
            Read into rec[field] from table[key]
         */
        private static function cachedRead(rec: Record, field: String, model, key: String, options: Object): Object {
            rec._cacheAssoc ||= {}
            if (rec._cacheAssoc[field] == null) {
                rec._cacheAssoc[field] =  model.readRecords(key, options); 
            }
            return rec._cacheAssoc[field]
        }

        private static function checkFormat(thisObj: Record, field: String, value, options: Object): Void {
            if (! RegExp(options.format).test(value)) {
                thisObj._errors[field] = options.message ? options.message : ErrorMessages.wrongFormat
            }
        }

        private static function checkNumber(thisObj: Record, field: String, value, options): Void {
            if (! RegExp(/^[0-9]+$/).test(value)) {
                thisObj._errors[field] = options.message ? options.message : ErrorMessages.notNumber
            }
        }

        private static function checkPresent(thisObj: Record, field: String, value, options): Void {
            if (value == undefined) {
                thisObj._errors[field] = options.message ? options.message : ErrorMessages.missing
            } else if (value.length == 0 || value.trim() == "" && thisObj._errors[field] == undefined) {
                thisObj._errors[field] = ErrorMessages.blank
            }
        }

        private static function checkUnique(thisObj: Record, field: String, value, options): Void {
            let grid: Array
            if (thisObj._keyValue) {
                grid = findWhere(field + ' = "' + value + '" AND id <> ' + thisObj._keyValue)
            } else {
                grid = findWhere(field + ' = "' + value + '"')
            }
            if (grid.length > 0) {
                thisObj._errors[field] = options.message ? options.message : ErrorMessages.notUnique
            }
        }

        /*
            Map types from SQL to ejs when reading from the database
         */
        private function coerceToEjsTypes(): Void {
            for (let field: String in this) {
                let col: Column = _columns[field]
                if (col == undefined) {
                    continue
                }
                if (col.ejsType == Reflect(this[field]).type) {
                    continue
                }
                let value: String = this[field]
                switch (col.ejsType) {
                case Boolean:
                    if (value is String) {
                        this[field] = (value.trim().toLower() == "true")
                    } else if (value is Number) {
                        this[field] = (value == 1)
                    } else {
                        this[field] = value cast Boolean
                    }
                    this[field] = (this[field]) ? true : false
                    break

                case Date:
                    this[field] = new Date(value)
                    break

                case Number:
                    this[field] = this[field] cast Number
                    break
                }
            }
        }

        /*
            Create associations for a record
         */
        private static function createAssociations(rec: Record, set: Array, preload, options): Void {
            for each (let model in set) {
                if (model is Array) model = model[0]
                // print("   Create Assoc for " + _tableName + "[" + model._assocName + "] for " + model._tableName + "[" + 
                //    rec[model._foreignId] + "]")
                if (preload == true || (preload && preload.contains(model))) {
                    /*
                        Query did table join, so rec already has the data. Extract the fields for the referred model and
                        then remove from rec and replace with an association reference. 
                     */
                    let association = {}
                    if (!model._columns) model.getSchema()
                    for (let field: String in model._columns) {
                        let f: String = "_" + model._className + field.toPascal()
                        association[field] = rec[f]
                        delete rec.public::[f]
                    }
                    rec[model._assocName] = model.createRecord(association, options)

                } else {
                    rec[model._assocName] = makeLazyReader(rec, model._assocName, model, rec[model._foreignId])
                    if (!model._columns) model.getSchema()
                    for (let field: String  in model._columns) {
                        let f: String = "_" + model._className + field.toPascal()
                        if (rec[f]) {
                            delete rec.public::[f]
                        }
                    }
                }
            }
        }

        /*
            Create a new record instance and apply the row data
            Process a sql result and add properties for each field in the row
         */
        private static function createRecord(data: Object, options: Object = {}) {
            let rec: Record = new global[_className]
            rec.initialize(data)
            rec._keyValue = data[_keyName]

            let subOptions = {}
            if (options.depth) {
                subOptions.depth = options.depth
                subOptions.depth--
            }

            if (options.include) {
                createAssociations(rec, options.include, true, subOptions)
            }
            if (options.depth != 0) {
                if (_belongsTo) {
                    createAssociations(rec, _belongsTo, options.preload, subOptions)
                }
                if (_hasOne) {
                    for each (model in _hasOne) {
                        if (!rec[model._assocName]) {
                            rec[model._assocName] = makeLazyReader(rec, model._assocName, model, null,
                                {conditions: rec._foreignId + " = " + data[_keyName] + " LIMIT 1"})
                        }
                    }
                }
                if (_hasMany) {
                    for each (model in _hasMany) {
                        if (!rec[model._assocName]) {
                            rec[model._assocName] = makeLazyReader(rec, model._assocName, model, null,
                                {conditions: rec._foreignId + " = " + data[_keyName]})
                        }
                    }
                }
            }
            rec.coerceToEjsTypes()
            return rec
        }

        /**
            Set an error message. This defines an error message for the given field in a record.
            @param field Name of the field to associate with the error message
            @param msg Error message
         */
        function error(field: String, msg: String): Void {
            field ||= ""
            _errors ||= {}
            _errors[field] = msg
        }

        /**
            Find a record. Find and return a record identified by its primary key if supplied or by the specified options. 
            If more than one record matches, return the first matching record.
            @param key Key Optional key value. Set to null if selecting via the options 
            @param options Optional search option values
            @returns a model record or null if the record cannot be found.
            @throws IOError on internal SQL errors
            @option columns List of columns to retrieve
            @option conditions { field: value, ...}   or [ "SQL condition", "id == 23", ...]
            @option from Low level from clause (not fully implemented)
            @option keys [set of matching key values]
            @option order ORDER BY clause
            @option group GROUP BY clause
            @option include [Model, ...] Models to join in the query and create associations for. Always preloads.
            @option joins Low level join statement "LEFT JOIN vists on stockId = visits.id". Low level joins do not
                create association objects (or lazy loaders). The names of the joined columns are prefixed with the
                appropriate table name using camel case (tableColumn).
            @option limit LIMIT count
            @option depth Specify the depth for which to create associations for belongsTo, hasOne and hasMany relationships.
                 Depth of 1 creates associations only in the immediate fields of the result. Depth == 2 creates in the 
                 next level and so on. Defaults to one.
            @option offset OFFSET count
            @option preload [Model1, ...] Preload "belongsTo" model associations rather than creating lazy loaders. This can
                reduce the number of database queries if iterating through associations.
            @option readonly
            @option lock
         */
        static function find(key: Object, options: Object = {}): Object {
            let grid: Array = innerFind(key, 1, options)
            if (grid.length >= 1) {
                let results = createRecord(grid[0], options)
                if (options && options.debug) {
                    print("RESULTS: " + serialize(results))
                }
                return results
            } 
            return null
        }

        /**
            Find all the matching records
            @param options Optional set of options. See $find for list of possible options.
            @returns An array of model records. The array may be empty if no matching records are found
            @throws IOError on internal SQL errors
         */
        static function findAll(options: Object = {}): Array {
            let grid: Array = innerFind(null, null, options)
            // start = new Date
            for (let i = 0; i < grid.length; i++) {
                grid[i] = createRecord(grid[i], options)
            }
            // print("findAll - create records TIME: " + start.elapsed())
            if (options && options.debug) {
                print("RESULTS: " + serialize(grid))
            }
            return grid
        }

        /**
            Find the first record matching a condition. Select a record using a given SQL where clause.
            @param where SQL WHERE clause to use when selecting rows.
            @returns a model record or null if the record cannot be found.
            @throws IOError on internal SQL errors
            @example
                rec = findOneWhere("cost < 200")
         */
        static function findOneWhere(where: String): Object {
            let grid: Array = innerFind(null, 1, { conditions: [where]})
            if (grid.length >= 1) {
                return createRecord(grid[0])
            } 
            return null
        }

        /**
            Find records matching a condition. Select a set of records using a given SQL where clause
            @param where SQL WHERE clause to use when selecting rows.
            @returns An array of objects. Each object represents a matching row with fields for each column.
            @example
                list = findWhere("cost < 200")
         */
        static function findWhere(where: String, count: Number = null): Array {
            let grid: Array = innerFind(null, null, { conditions: [where]})
            for (i in grid.length) {
                grid[i] = createRecord(grid[i])
            }
            return grid
        }

        /**
            Return the column names for the table
            @returns an array containing the names of the database columns. This corresponds to the set of properties
                that will be created when a row is read using $find.
         */
        static function getColumnNames(): Array { 
            if (!_columns) getSchema()
            let result: Array = []
            for (let col: Column in _columns) {
                result.append(col)
            }
            return result
        }

        /**
            Return the column names for the record
            @returns an array containing the Pascal case names of the database columns. The names have the first letter
                capitalized. 
         */
        static function getColumnTitles(): Array { 
            if (!_columns) getSchema()
            let result: Array = []
            for (let col: Column in _columns) {
                result.append(col.toPascal())
            }
            return result
        }

        /** Get the type of a column
            @param field Name of the field to examine.
            @return A string with the data type of the column
         */
        static function getColumnType(field: String): String {
            if (!_columns) getSchema()
            return _db.sqlTypeToDataType(_columns[field].sqlType)
        }

        /**
            Get the database connection for this record class
            @returns Database instance object created via new $Database
         */
        static function getDb(): Database {
            if (!_db) {
                _db = Database.defaultDatabase
            }
            return _db
        }

        /**
            Get the errors for the record. 
            @return The error message collection for the record.  
         */
        function getErrors(): Array
            _errors

        /**
            Get the key name for this record
         */
        static function getKeyName(): String
            _keyName

        /**
            Return the number of rows in the table
         */
        static function getNumRows(): Number {
            if (!_columns) getSchema()
            let cmd: String = "SELECT COUNT(*) FROM " + _tableName + " WHERE " + _keyName + " <> '';"
            let grid: Array = _db.query(cmd, "numRows", _trace)
            return grid[0]["COUNT(*)"]
        }

        /*
            Read the table schema and return the column hash
         */
        private static function getSchema(): Void {
            if (!_db) {
                _db = Database.defaultDatabase
                if (!_db) {
                    throw new Error("Can't get schema, database connection has not yet been established")
                }
            }
            let sql: String = 'PRAGMA table_info("' + _tableName + '");'
            let grid: Array = _db.query(sql, "schema", _trace)
            _columns = {}
            for each (let row: Record in grid) {
                let name = row["name"]
                let sqlType = row["type"].toLower()
                let ejsType = mapSqlTypeToEjs(sqlType)
                _columns[name] = new Column(name, false, ejsType, sqlType)
            }
        }

        /**
            Get the associated name for this record
            @returns the database table name backing this record class. Normally this is simply a plural class name. 
         */
        static function getTableName(): String
            _tableName

        /**
            Define a containment relationship to another model class. When using "hasAndBelongsToMany" on another model, it 
            means that other models have a foreign key reference to this class and this class can "contain" many instances 
            of the other models.
            @param model Model. (not implemented).
            @param options Object hash of options.
            @option foreignKey Key name for the foreign key. (not implemented).
            @option through String Class name which mediates the many to many relationship. (not implemented).
            @option joinTable. (not implemented).
         */
        static function hasAndBelongsToMany(model: Object, options: Object = {}): Void {
            belongsTo(model, options)
            hasMany(model, options)
        }

        /**
            Check if the record has any errors.
            @return True if the record has errors.
         */
        function hasError(field: String = null): Boolean {
            if (field) {
                return (_errors && _errors[field])
            }
            if (_errors) {
                return _errors.length > 0
            }
            return false
        }

        /**
            Define a containment relationship to another model class. When using "hasMany" on another model, it means 
            that other model has a foreign key reference to this class and this class can "contain" many instances of 
            the other.
            @param model Model class that is contained by this class. 
            @param options Call options
            @option things Model object that is posessed by this. (not implemented)
            @option through String Class name which mediates the many to many relationship. (not implemented)
            @option foreignKey Key name for the foreign key. (not implemented)
         */
        static function hasMany(model: Object, options: Object = {}): Void {
            _hasMany ||= []
            _hasMany.append(model)
        }

        /**
            Define a containment relationship to another model class. When using "hasOne" on another model, 
            it means that other model has a foreign key reference to this class and this class can "contain" 
            only one instance of the other.
            @param model Model class that is contained by this class. 
            @option thing Model that is posessed by this. (not implemented).
            @option foreignKey Key name for the foreign key (not implemented).
            @option as String  (not implemented).
         */
        static function hasOne(model: Object, options: Object = null): Void {
            _hasOne ||= []
            _hasOne.append(model)
        }

        /*
            Common find implementation. See find/findAll for doc.
         */
        static private function innerFind(key: Object, limit: Number = null, options: Object = {}): Array {
            let cmd: String
            let columns: Array
            let from: String
            let conditions: String
            let where: String

            if (!_columns) getSchema()
            if (options == null) {
                options = {}
            }
            //  LEGACY 1.0.0-B2
            if (options.noassoc) {
                options.depth = 0
            }

            if (options.columns) {
                columns = options.columns
                /*
                    Qualify "id" so it won't clash when doing joins. If the "click" option is specified, must have an ID
                 */
                let index: Number = columns.indexOf("id")
                if (index >= 0) {
                    columns[index] = _tableName + ".id"
                } else if (!columns.contains(_tableName + ".id")) {
                    columns.insert(0, _tableName + ".id")
                }
            } else {
                columns =  "*"
            }

            conditions = ""
            from = ""
            where = false

            if (options.from) {
                from = options.from
            } else {
                from = _tableName
            }

            if (options.include) {
                let model
                if (options.include is Array) {
                    for each (entry in options.include) {
                        if (entry is Array) {
                            model = entry[0]
                            from += " LEFT OUTER JOIN " + model._tableName
                            from += " ON " + entry[1]
                        } else {
                            model = entry
                            from += " LEFT OUTER JOIN " + model._tableName
                        }
                    }
                } else {
                    model = options.include
                    from += " LEFT OUTER JOIN " + model._tableName
                    // conditions = " ON " + model._tableName + ".id = " + _tableName + "." + model._assocName + "Id"
                }
            }

            if (options.depth != 0) {
                if (_belongsTo) {
                    conditions = " ON "
                    for each (let owner in _belongsTo) {
                        from += " INNER JOIN " + owner._tableName
                    }
                    for each (let owner in _belongsTo) {
                        let tname: String = Reflect(owner).name
                        tname = tname[0].toLower() + tname.slice(1) + "Id"
                        conditions += _tableName + "." + tname + " = " + owner._tableName + "." + owner._keyName + " AND "
                    }
                    if (conditions == " ON ") {
                        conditions = ""
                    }
                }
            }

            if (options.joins) {
                if (conditions == "") {
                    conditions = " ON "
                }
                let parts: Array = options.joins.split(/ ON | on /)
                from += " " + parts[0]
                if (parts.length > 1) {
                    conditions += parts[1] + " AND "
                }
            }
            conditions = conditions.trim(" AND ")

            if (options.conditions) {
                let whereConditions: String = " WHERE "
                if (options.conditions is Array) {
                    for each (cond in options.conditions) {
                        whereConditions += cond + " " + " AND "
                    }
                    whereConditions = whereConditions.trim(" AND ")

                } else if (options.conditions is String) {
                    whereConditions += options.conditions + " " 

                } else if (options.conditions is Object) {
                    for (field in options.conditions) {
                        //  Remove quote from options.conditions[field]
                        whereConditions += field + " = '" + options.conditions[field] + "' " + " AND "
                    }
                }
                whereConditions = whereConditions.trim(" AND ")
                if (whereConditions != " WHERE ") {
                    where = true
                    conditions += whereConditions
                } else {
                    whereConditions = ""
                    from = from.trim(" AND ")
                }

            } else {
                from = from.trim(" AND ")
            }

            if (key || options.key) {
                if (!where) {
                    conditions += " WHERE "
                    where = true
                } else {
                    conditions += " AND "
                }
                conditions += (_tableName + "." + _keyName + " = ") + ((key) ? key : options.key)
            }

            //  Removed quote from "from"
            cmd = "SELECT " + Database.quote(columns) + " FROM " + from + conditions
            if (options.group) {
                cmd += " GROUP BY " + options.group
            }
            if (options.order) {
                cmd += " ORDER BY " + options.order
            }
            if (limit) {
                cmd += " LIMIT " + limit
            } else if (options.limit) {
                cmd += " LIMIT " + options.limit
            }
            if (options.offset) {
                cmd += " OFFSET " + options.offset
            }
            cmd += ";"

            if (_db == null) {
                throw new Error("Database connection has not yet been established")
            }

            let results: Array
            try {
                if (_trace || 1) {
                    results = _db.query(cmd, "find", _trace)
                } else {
                    results = _db.query(cmd, "find", _trace)
                }
            }
            catch (e) {
                throw e
            }
            return results
        }

        /*
            Make a getter function to lazily (on-demand) read associated records (belongsTo)
         */
        private static function makeLazyReader(rec: Record, field: String, model, key: String, 
                options: Object = {}): Function {
            // print("Make lazy reader for " + _tableName + "[" + field + "] for " + model._tableName + "[" + key + "]")
            var lazyReader: Function = function(): Object {
                // print("Run reader for " + _tableName + "[" + field + "] for " + model._tableName + "[" + key + "]")
                return cachedRead(rec, field, model, key, options)
            }
            return makeGetter(lazyReader)
        }

        private static function mapSqlTypeToEjs(sqlType: String): String {
            sqlType = sqlType.replace(/\(.*/, "")
            let ejsType: String = _db.sqlTypeToEjsType(sqlType)
            if (ejsType == undefined) {
                throw new Error("Unsupported SQL type: \"" + sqlType + "\"")
            }
            return ejsType
        }

        /*
            Prepare a value to be written to the database
         */
        private static function prepareValue(field: String, value: Object): String {
            let col: Column = _columns[field]
            if (col == undefined) {
                return undefined
            }
			if (value == undefined) {
				throw new Error("Field \"" + field + "\" is undefined")
			}
			if (value == null) {
				throw new Error("Field \"" + field + "\" is null")
			}
            switch (col.ejsType) {
            case Boolean:
                if (value is String) {
                    value = (value.toLower() == "true")
                } else if (value is Number) {
                    value = (value == 1)
                } else {
                    value = value cast Boolean
                }
                return value

            case Date:
                return "%d".format((new Date(value)).time)

            case Number:
                return value cast Number
             
            case String:
                return Database.quote(value)
            }
            return Database.quote(value.toString())
        }

        /*
            Read records for an assocation. Will return one or an array of records matching the supplied key and options.
         */
        private static function readRecords(key: String, options: Object): Object {
            let data: Array = innerFind(key, null, options)
            if (data.length > 1) {
                let result: Array = new Array
                for each (row in data) {
                    result.append(createRecord(row))
                }
                return result

            } else if (data.length == 1) {
                return createRecord(data[0])
            }
            return null
        }

        /**
            Remove records from the database
            @param keys Set of keys identifying the records to remove
         */
        static function remove(...keys): Void {
            for each (let key: Object in keys) {
                let cmd: String = "DELETE FROM " + _tableName + " WHERE " + _keyName + " = " + key + ";"
                db.query(cmd, "remove", _trace)
            }
        }

        private function runFilters(filters): Void {
            for each (filter in filters) {
                let fn = filter[0]
                let options = filter[1]
                if (options) {
                    let only = options.only
                }
                fn.call(this)
            }
        }

        /**
            Save the record to the database.
            @returns True if the record is validated and successfully written to the database
            @throws IOError Throws exception on sql errors
         */
        function save(): Boolean {
            var sql: String
            if (!_columns) getSchema()
            if (!validateRecord()) {
                return false
            }
            runFilters(_beforeFilters)
            
            if (_keyValue == null) {
                sql = "INSERT INTO " + _tableName + " ("
                for (let field: String in this) {
                    if (_columns[field]) {
                        sql += field + ", "
                    }
                }
                sql = sql.trim(', ')
                sql += ") VALUES("
                for (let field: String in this) {
                    if (_columns[field]) {
                        sql += "'" + prepareValue(field, this[field]) + "', "
                    }
                }
                sql = sql.trim(', ')
                sql += ")"

            } else {
                sql = "UPDATE " + _tableName + " SET "
                for (let field: String in this) {
                    if (_columns[field]) {
                        sql += field + " = '" + prepareValue(field, this[field]) + "', "
                    }
                }
                sql = sql.trim(', ')
                sql += " WHERE " + _keyName + " = " +  _keyValue
            }
            if (!_keyValue) {
                sql += "; SELECT last_insert_rowid();"
            } else {
                sql += ";"
            }

            let result: Array = _db.query(sql, "save", _trace)
            if (!_keyValue) {
                _keyValue = this["id"] = result[0]["last_insert_rowid()"] cast Number
            }
            runFilters(_afterFilters)
            return true
        }

        /**
            Update a record based on the supplied fields and values.
            @param fields Hash of field/value pairs to use for the record update.
            @returns True if the database is successfully updated. Returns false if validation fails. In this case,
                the record is not saved.
            @throws IOError on database SQL errors
         */
        function saveUpdate(fields: Object): Boolean {
            for (field in fields) {
                if (this[field] != undefined) {
                    this[field] = fields[field]
                }
            }
            return save()
        }

        /**
            Set the database connection for this record class
            @param database Database instance object created via new $Database
         */
        static function setDb(database: Database) {
            _db = database
        }

        /**
            Set the key name for this record
         */
        static function setKeyName(name: String): Void {
            _keyName = name
        }

        /**
            Set the associated table name for this record
            @param name Name of the database table to backup the record class.
         */
        static function setTableName(name: String): Void {
            if (_tableName != name) {
                _tableName = name
                if (_db) {
                    getSchema()
                }
            }
        }

        /**
            Run an SQL statement and return selected records.
            @param cmd SQL command to issue. Note: "SELECT" is automatically prepended and ";" is appended for you.
            @returns An array of objects. Each object represents a matching row with fields for each column.
         */
        static function sql(cmd: String, count: Number = null): Array {
            cmd = "SELECT " + cmd + ";"
            return db.query(cmd, "select", _trace)
        }

        /**
            Trace SQL statements. Control whether trace is enabled for the actual SQL statements issued against the database.
            @param on If true, display each SQL statement to the log
         */
        static function trace(on: Boolean): void
            _trace = on

        /** @hide */
        static function validateFormat(fields: Object, options: Object = null) {
            if (_validations == null) {
                _validations = []
            }
            _validations.append([checkFormat, fields, options])
        }

        /** @hide */
        static function validateNumber(fields: Object, options: Object = null) {
            if (_validations == null) {
                _validations = []
            }
            _validations.append([checkNumber, fields, options])
        }

        /** @hide */
        static function validatePresence(fields: Object, options: Object = null) {
            if (_validations == null) {
                _validations = []
            }
            _validations.append([checkPresent, fields, options])
        }

        /**
            Validate a record. This call validates all the fields in a record.
            @returns True if the record has no errors.
         */
        function validateRecord(): Boolean {
            if (!_columns) getSchema()
            _errors = {}
            if (_validations) {
                for each (let validation: String in _validations) {
                    let check = validation[0]
                    let fields = validation[1]
                    let options = validation[2]
                    if (fields is Array) {
                        for each (let field in fields) {
                            if (_errors[field]) {
                                continue
                            }
                            check(this, field, this[field], options)
                        }
                    } else {
                        check(this, fields, this[fields], options)
                    }
                }
            }
            let thisType = Reflect(this).type
            if (thisType["validate"]) {
                thisType["validate"].call(this)
            }
            if (_errors.length == 0) {
                coerceToEjsTypes()
            }
            return _errors.length == 0
        }

        /** @hide */
        static function validateUnique(fields: Object, options: Object = null)
            _validations.append([checkUnique, fields, options])

        /**
            Run filters before and after saving data
            @param fn Function to run
            @param options - reserved
         */
        static function wrapFilter(fn, options: Object = null): Void {
            _wrapFilters ||= []
            _wrapFilters.append([fn, options])
        }

        //  LEGACY DEPRECATED in 1.0.0-B2
        /** @hide */
        static function get columnNames(): Array {
            return getColumnNames()
        }
        /** @hide */
        static function get columnTitles(): Array {
            return getColumnTitles()
        }
        /** @hide */
        static function get db(): Datbase {
            return getDb()
        }
        /** @hide */
        static function get keyName(): String {
            return getKeyName()
        }
        /** @hide */
        static function get numRows(): String {
            return getNumRows()
        }
        /** @hide */
        static function get tableName(): String {
            return getTableName()
        }
        /** @hide */
        function constructor(fields: Object = null): Void {
            initialize(fields)
        }
    }


    /**
        Database column class. A database record is comprised of one or mor columns
        @hide
     */
    class Column {
        public var ejsType: Object 
        public var sqlType: Object 

        function Column(name: String, accessor: Boolean = false, ejsType: Type = null, sqlType: String = null) {
            this.ejsType = ejsType
            this.sqlType = sqlType
        }
    }

    /** @hide */
    function plural(name: String): String
        name + "s"

    /** @hide */
    function singular(name: String): String {
        var s: String = name + "s"
        if (name.endsWith("ies")) {
            return name.slice(0,-3) + "y"
        } else if (name.endsWith("es")) {
            return name.slice(0,-2)
        } else if (name.endsWith("s")) {
            return name.slice(0,-1)
        }
        return name.toPascal()
    }

    /**
        Map a type assuming it is already of the correct ejs type for the schema
        @hide
     */
    function mapType(value: Object): String {
        if (value is Date) {
            return "%d".format((new Date(value)).time)
        } else if (value is Number) {
            return "%d".format(value)
        }
        return value
    }
}

/* To ease forward compatibility */
module ejs.db.mapper {}


/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.
    
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
    vim: sw=8 ts=8 expandtab

    @end
 */
