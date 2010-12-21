/**
 *  Sqlite.es -- SQLite Database class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.db.sqlite {

    /**
     *  SQLite database support
     *  @spec ejs
     *  @stability evolving
     */
    "ejs.db" class Sqlite {

        /*
         *  Map independent types to SQL types
         */
        static const DataTypeToSqlType: Object = {
            "binary":       "blob",
            "boolean":      "tinyint",
            "date":         "date",
            "datetime":     "datetime",
            "decimal":      "decimal",
            "float":        "float",
            "integer":      "int",
            "number":       "decimal",
            "string":       "varchar",
            "text":         "text",
            "time":         "time",
            "timestamp":    "datetime",
        }

        /*
         *  Map independent types to SQL types
         */
        static const SqlTypeToDataType: Object = {
            "blob":         "binary",
            "tinyint":      "boolean",
            "date":         "date",
            "datetime":     "datetime",
            "decimal":      "decimal",
            "float":        "float",
            "int":          "integer",
            "varchar":      "string",
            "text":         "text",
            "time":         "time",
        }

        /*
         *  Map SQL types to Ejscript native types
         */
        static const SqlTypeToEjsType: Object = {
            "blob":         String,
            "date":         Date,
            "datetime":     Date,
            "decimal":      Number,
            "int":          Number,
            "integer":      Number,
            "float":        Number,
            "time":         Date,
            "tinyint":      Boolean,
            "text":         String,
            "varchar":      String,
        }

        /*
            Map Ejscript native types back to SQL types
            INCOMPLETE and INCORRECT
         
        static const EjsToDataType: Object = {
            "string":       "varchar",
            "number":       "decimal",
            "date":         "datetime",
            "bytearray":    "Blob",
            "boolean":      "tinyint",
        }
         */

        use default namespace public

        /**
            Initialize a database connection using the supplied database connection string
            @param connectionString Connection string stipulating how to connect to the database. The only supported 
            format is:
                <ul>
                    <li>filename</li>
                </ul>
                Where filename is the path to the database. 
         */
        native "ejs.db" function Sqlite(connectionString: String)

        /** @duplicate ejs.db::Database.addColumn */
        function addColumn(table: String, column: String, datatype: String, options: Object = null): Void {
            datatype = DataTypeToSqlType[datatype.toLower()]
            if (datatype == undefined) {
                throw "Bad Ejscript column type: " + datatype
            }
            query("ALTER TABLE " + table + " ADD " + column + " " + datatype)
        }

        /** @duplicate ejs.db::Database.addIndex */
        function addIndex(table: String, column: String, index: String): Void
            query("CREATE INDEX " + index + " ON " + table + " (" + column + ");")

        /** 
            @duplicate ejs.db::Database.changeColumn 
            @hide
            SQLite cannot change or rename columns.
         */
        function changeColumn(table: String, column: String, datatype: String, options: Object = null): Void {
            datatype = datatype.toLower()
            if (DataTypeToSqlType[datatype] == undefined) {
                throw "Bad column type: " + datatype
            }
            /* query("ALTER TABLE " + table + " CHANGE " + column + " " + datatype) */
            throw "SQLite does not support column changes"
        }

        /** @duplicate ejs.db::Database.close */
        native function close(): Void

        /** @duplicate ejs.db::Database.commit  Not implemented */
        function commit(): Void {}

        /** @duplicate ejs.db::Database.connect 
            Not yet implemented.
            @hide
         */
        native function connect(connectionString: String): Void

        /** @duplicate ejs.db::Database.createDatabase */
        function createDatabase(name: String, options: Object = null): Void {
            /* Nothing to do for sqlite */
        }

        /** @duplicate ejs.db::Database.createTable */
        function createTable(table: String, columns: Array = null): Void {
            let cmd: String

            query("DROP TABLE IF EXISTS " + table + ";")
            query("CREATE TABLE " + table + "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL);")

            if (columns) {
                for each (let colspec: String in columns) {
                    let spec: Array = colspec.split(":")
                    if (spec.length != 2) {
                        throw "Bad column spec: " + spec
                    }
                    let column: String = spec[0]
                    let datatype: String = spec[1]
                    addColumn(table, column.trim(), datatype.trim())
                }
            }
        }

        /** @duplicate ejs.db::Database.dataTypeToSqlType */
        static function dataTypeToSqlType(dataType:String): String
            DataTypeToSqlType[dataType]

        /** @duplicate ejs.db::Database.destroyDatabase */
        function destroyDatabase(name: String): Void
            Path(name).remove()

        /** @duplicate ejs.db::Database.destroyTable */
        function destroyTable(table: String): Void
            query("DROP TABLE IF EXISTS " + table + ";")

        /** @duplicate ejs.db::Database.endTransaction */
        function endTransaction(): Void {}

        /** @duplicate ejs.db::Database.getColumns */
        function getColumns(table: String): Array {
            grid = query('PRAGMA table_info("' + table + '");')
            let names = []
            for each (let row in grid) {
                let name: String = row["name"]
                names.append(name)
            }
            return names
        }

        /**
         *  @duplicate ejs.db::Database.getNumRows
         */
        function getNumRows(table: String): Number {
            let cmd: String = "SELECT COUNT(*) FROM " + table + ";"
            let grid: Array = query(cmd, "numRows")
            return grid[0]["COUNT(*)"] cast Number
        }

        /** @duplicate ejs.db::Database.getTables */
        function getTables(): Array {
            let cmd: String = "SELECT name from sqlite_master WHERE type = 'table' order by NAME;"
            let grid: Array = query(cmd)
            let result: Array = new Array
            for each (let row: Object in grid) {
                let name: String = row["name"]
                if (!name.contains("sqlite_") && !name.contains("_Ejs")) {
                    result.append(row["name"])
                }
            }
            return result
        }

        /** @duplicate ejs.db::Database.removeColumns */
        function removeColumns(table: String, columns: Array): Void {
            /*
             *  This is a dumb SQLite work around because it doesn't have drop column
             */
            backup = "_backup_" + table
            keep = getColumns(table)
            for each (column in columns) {
                if ((index = keep.indexOf(column)) < 0) {
                    throw "Column \"" + column + "\" does not exist in " + table
                } 
                keep.remove(index)
            }

            schema = 'PRAGMA table_info("' + table + '");'
            grid = query(schema)
            types = {}
            for each (let row in grid) {
                let name: String = row["name"]
                types[name] = row["type"]
            }

            columnSpec = []
            for each (k in keep) {
                if (k == "id") {
                    columnSpec.append(k + " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL")
                } else {
                    columnSpec.append(k + " " + types[k])
                }
            }

            cmd = "BEGIN TRANSACTION;
                CREATE TEMPORARY TABLE " + backup + "(" + columnSpec + ");
                INSERT INTO " + backup + " SELECT " + keep + " FROM " + table + ";
                DROP TABLE " + table + ";
                CREATE TABLE " + table + "(" + columnSpec + ");
                INSERT INTO " + table + " SELECT " + keep + " FROM " + backup + ";
                DROP TABLE " + backup + ";
                COMMIT;"
            query(cmd)
        }

        /** @duplicate ejs.db::Database.removeIndex */
        function removeIndex(table: String, index: String): Void
            query("DROP INDEX " + index + ";")

        /** 
            @duplicate ejs.db::Database.renameColumn 
            @hide
            SQLite does not support renaming columns
         */
        function renameColumn(table: String, oldColumn: String, newColumn: String): Void {
            throw "SQLite does not support renaming columns"
            // query("ALTER TABLE " + table + " RENAME " + oldColumn + " TO " + newColumn + ";")
        }

        /** @duplicate ejs.db::Database.renameTable */
        function renameTable(oldTable: String, newTable: String): Void
            query("ALTER TABLE " + oldTable + " RENAME TO " + newTable + ";")

        /** @duplicate ejs.db::Database.rollback 
            @hide
         */
        function rollback(): Void {}

        /** @duplicate ejs.db::Database.query */
        function query(cmd: String, tag: String = "SQL", trace: Boolean = false): Array {
            if (trace) {
                print(tag + ": " + cmd)
            }
            return sql(cmd)
        }

        /** @duplicate ejs.db::Database.sql */
        native function sql(cmd: String): Array

        /** @duplicate ejs.db::Database.sqlTypeToDataType */
        static function sqlTypeToDataType(sqlType: String): String
            SqlTypeToDataType[sqlType]

        /** @duplicate ejs.db::Database.sqlTypeToEjsType */
        static function sqlTypeToEjsType(sqlType: String): Type
            SqlTypeToEjsType[sqlType]

        /** @duplicate ejs.db::Database.startTransaction */
        function startTransaction(): Void {}

    }
}


/*
 *  @copy   default
 *  
 *  Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.
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
 *  Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=8 ts=8 expandtab

    @end
 */
