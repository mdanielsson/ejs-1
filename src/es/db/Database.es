/**
 *  Database.es -- Database class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/*
 *  Design notes:
 *  - Don't create static vars so this class can be fully stateless and can be placed into the master interpreter.
 */

module ejs.db {

    /**
     *  SQL Database support. The Database class provides an interface over other database adapter classes such as 
     *  SQLite or MySQL. Not all the functionality expressed by this API may be implemented by a specific database adapter.
     *  @spec ejs
     *  @stability evolving
     */
    class Database {

        private var _adapter: Object
        private var _connection: String
        private var _name: String
        private var _traceAll: Boolean

        use default namespace public

        /**
         *  Initialize a database connection using the supplied database connection string
            @param adapter Database adapter to use. E.g. "sqlite"
         *  @param connectionString Connection string stipulating how to connect to the database. The format is one of the 
         *  following forms:
         *      <ul>
         *          <li>adapter://host/database/username/password</li>
         *          <li>filename</li>
         *      </ul>
         *      Where adapter specifies the kind of database. Sqlite is currently the only supported adapter.
         *      For sqlite connection strings, the abbreviated form is permitted where a filename is supplied and the 
         *      connection string is assumed to be: <pre>sqlite://localhost/filename</pre>
         */
        function Database(adapter: String, connectionString: String) {
            if (adapter == "sqlite3") adapter = "sqlite"
            try {
                _name = basename(connectionString)
                _connection = connectionString
                let adapterClass = adapter.toPascal()
                if (global."ejs.db"::[adapterClass] == undefined) {
                    load("ejs.db." + adapter + ".mod")
                }
                _adapter = new global."ejs.db"::[adapterClass](connectionString)
            } catch (e) {
                print(e)
                throw "Can't find database connector for " + adapter
            }
        }

        /**
         *  Add a column to a table.
         *  @param table Name of the table
         *  @param column Name of the column to add
         *  @param datatype Database independant type of the column. Valid types are: binary, boolean, date,
         *      datetime, decimal, float, integer, number, string, text, time and timestamp.
         *  @param options Optional parameters
         */
        function addColumn(table: String, column: String, datatype: String, options: Object = null): Void
            _adapter.addColumn(table, column, datatype, options)

        /**
         *  Add an index on a column
         *  @param table Name of the table
         *  @param column Name of the column to add
         *  @param index Name of the index
         */
        function addIndex(table: String, column: String, index: String): Void
            _adapter.addIndex(table, column, index)

        /**
         *  Change a column
         *  @param table Name of the table holding the column
         *  @param column Name of the column to change
         *  @param datatype Database independant type of the column. Valid types are: binary, boolean, date,
         *      datetime, decimal, float, integer, number, string, text, time and timestamp.
         *  @param options Optional parameters
         */
        function changeColumn(table: String, column: String, datatype: String, options: Object = null): Void
            _adapter.changeColumn(table, column, datatype, options)

        /**
         *  Close the database connection. Database connections should be closed when no longer needed rather than waiting
         *  for the garbage collector to automatically close the connection when disposing the database instance.
         */
        function close(): Void
            _adapter.close()

        /**
         *  Commit a database transaction
         */
        function commit(): Void
            _adapter.commit()

        /**
         *  Reconnect to the database using a new connection string. Not yet implemented.
         *  @param connectionString See Database() for information about connection string formats.
         *  @hide
         */
        function connect(connectionString: String): Void
            _adapter.connect(connectionString)

        /**
         *  The database connection string
         */
        function get connection(): String {
            return _connection
        }

        /**
         *  Create a new database
         *  @param name Name of the database
         *  @options Optional parameters
         */
        function createDatabase(name: String, options: Object = null): Void
            _adapter.createDatabase(name, options)

        /**
         *  Create a new table
         *  @param table Name of the table
         *  @param columns Array of column descriptor tuples consisting of name:datatype
         *  @options Optional parameters
         */
        function createTable(table: String, columns: Array = null): Void
            _adapter.createTable(table, columns)

        /**
         *  Map the database independant data type to a database dependant SQL data type
         *  @param dataType Data type to map
         *  @returns The corresponding SQL database type
         */
        function dataTypeToSqlType(dataType:String): String
            _adapter.dataTypeToSqlType(dataType)

        /**
         *  The default database for the application.
         */
        static function get defaultDatabase(): Database
            global."ejs.db"::"defaultDb"

        /**
         *  Set the default database for the application.
         *  @param db the default database to define
         */
        static function set defaultDatabase(db: Database): Void {
            /*
             *  Do this rather than using a Database static var so Database can go into the master interpreter
             */
            global."ejs.db"::"defaultDb" = db
        }

        /**
         *  Destroy a database
         *  @param name Name of the database to remove
         */
        function destroyDatabase(name: String): Void
            _adapter.destroyDatabase(name)

        /**
         *  Destroy a table
         *  @param table Name of the table to destroy
         */
        function destroyTable(table: String): Void
            _adapter.destroyTable(table)

        /**
         *  End a transaction
         */
        function endTransaction(): Void
            _adapter.endTransaction()

        /**
         *  Get column information 
         *  @param table Name of the table to examine
         *  @return An array of column data. This is database specific content and will vary depending on the
         *      database connector in use.
         */
        function getColumns(table: String): Array
            _adapter.getColumns(table)

        /**
         *  Return list of tables in a database
         *  @returns an array containing list of table names present in the currently opened database.
         */
        function getTables(): Array
            _adapter.getTables()

        /**
         *  Return the number of rows in a table
         *  @returns the count of rows in a table in the currently opened database.
         */
        function getNumRows(table: String): Number
            _adapter.getNumRows()

        /**
         *  The database name defined via the connection string or constructor.
         */
        function get name(): String
            _name

        /**
         *  Execute a SQL command on the database.
         *  @param cmd SQL command string
            @param tag Debug tag to use when logging the command
            @param trace Set to true to eanble logging this command.
         *  @returns An array of row results where each row is represented by an Object hash containing the 
         *      column names and values
         */
        function query(cmd: String, tag: String = "SQL", trace: Boolean = false): Array {
            if (_traceAll || trace) {
                print(tag + ": " + cmd)
            }
            return _adapter.sql(cmd)
        }

        /**
         *  Remove columns from a table
         *  @param table Name of the table to modify
         *  @param columns Array of column names to remove
         */
        function removeColumns(table: String, columns: Array): Void
            _adapter.removeColumns(table, columns)

        /**
         *  Remove an index
         *  @param table Name of the table to modify
         *  @param index Name of the index to remove
         */
        function removeIndex(table: String, index: String): Void
            _adapter.removeIndex(table, index)

        /**
         *  Rename a column
         *  @param table Name of the table to modify
         *  @param oldColumn Old column name
         *  @param newColumn New column name
         */
        function renameColumn(table: String, oldColumn: String, newColumn: String): Void
            _adapter.renameColumn(table, oldColumn, newColumn)

        /**
         *  Rename a table
         *  @param oldTable Old table name
         *  @param newTable New table name
         */
        function renameTable(oldTable: String, newTable: String): Void
            _adapter.renameTable(oldTable, newTable)

        /**
         *  Rollback an uncommited database transaction. Not supported.
         *  @hide
         */
        function rollback(): Void
            _adapter.rollback()

        /**
         *  Execute a SQL command on the database. This is a low level SQL command interface that bypasses logging.
         *      Use @query instead.
            @param cmd SQL command to issue. Note: "SELECT" is automatically prepended and ";" is appended for you.
         *  @returns An array of row results where each row is represented by an Object hash containing the column 
         *      names and values
         */
        function sql(cmd: String): Array
            _adapter.sql(cmd)

        /**
         *  Map the SQL type to a database independant data type
         *  @param sqlType SQL Data type to map
         *  @returns The corresponding database independant type
         */
        function sqlTypeToDataType(sqlType: String): String
            _adapter.sqlTypeToDataType(sqlType)

        /**
         *  Map the SQL type to an Ejscript type class
         *  @param sqlType SQL Data type to map
         *  @returns The corresponding type class
         */
        function sqlTypeToEjsType(sqlType: String): Type
            _adapter.sqlTypeToEjsType(sqlType)

        /**
         *  Start a new database transaction
         */
        function startTransaction(): Void
            _adapter.startTransaction()

        /**
         *  Trace all SQL statements on this database. Control whether trace is enabled for all SQL statements 
         *  issued against the database.
         *  @param on If true, display each SQL statement to the log
         */
        function trace(on: Boolean): void
            _traceAll = on

        /**
         *  Execute a database transaction
         *  @param code Function to run inside a database transaction
         */
        function transaction(code: Function): Void {
            startTransaction()
            try {
                code()
            } catch (e: Error) {
                rollback();
            } finally {
                endTransaction()
            }
        }

        /**
            Quote ", ', --, ;
            @hide
         */
        static function quote(str: String): String  {
            // str.replace(/'/g, "''").replace(/[#;\x00\x1a\r\n",;\\-]/g, "\\$0")
            // return str.replace(/'/g, "''").replace(/[#;",;\\-]/g, "\\$0")
            // return str.replace(/'/g, "''").replace(/[#";\\]/g, "\\$0")
            // return str.replace(/'/g, "''").replace(/[;\\]/g, "\\$0")
            return str.replace(/'/g, "''")
        }
    }
}


/*
 *  @copy   default
 *  
 *  Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
 *  
 *  This software is distributed under commercial and open source licenses.
 *  You may use the GPL open source license described below or you may acquire 
 *  a commercial license from Embedthis Software. You agree to be fully bound 
 *  by the terms of either license. Consult the LICENSE.TXT distributed with 
 *  this software for full details.
 *  
 *  This software is open source; you can redistribute it and/or modify it 
 *  under the terms of the GNU General Public License as published by the 
 *  Free Software Foundation; either version 2 of the License, or (at your 
 *  option) any later version. See the GNU General Public License for more 
 *  details at: http://www.embedthis.com/downloads/gplLicense.html
 *  
 *  This program is distributed WITHOUT ANY WARRANTY; without even the 
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  
 *  This GPL license does NOT permit incorporating this software into 
 *  proprietary programs. If you are unable to comply with the GPL, you must
 *  acquire a commercial license to use this software. Commercial licenses 
 *  for this software and support services are available from Embedthis 
 *  Software at http://www.embedthis.com 
 *  
 *  @end
 */
