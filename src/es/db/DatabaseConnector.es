/*
 *  DatabaseConnector.es -- Database Connector interface
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.db {

    /**
     *  Database interface connector. This interface is implemented by database connectors such as SQLite.
     *  @spec ejs
     *  @stability evolving
     */
    interface DatabaseConnector {

        use default namespace public

        // function DatabaseConnector(connectionString: String)

        /** @duplicate ejs.db::Database.addColumn */
        function addColumn(table: String, column: String, datatype: String, options: Object = null): Void

        /** @duplicate ejs.db::Database.addIndex */
        function addIndex(table: String, column: String, index: String): Void

        /** @duplicate ejs.db::Database.changeColumn */
        function changeColumn(table: String, column: String, datatype: String, options: Object = null): Void

        /** @duplicate ejs.db::Database.close */
        function close(): Void

        /** @duplicate ejs.db::Database.commit */
        function commit(): Void

        /** @duplicate ejs.db::Database.connect 
            Not yet implemented
            @hide
         */
        function connect(connectionString: String): Void

        /** @duplicate ejs.db::Database.createDatabase */
        function createDatabase(name: String, options: Object = null): Void

        /** @duplicate ejs.db::Database.createTable */
        function createTable(table: String, columns: Array = null): Void

        /** @duplicate ejs.db::Database.dataTypeToSqlType */
        function dataTypeToSqlType(dataType:String): String

        /** @duplicate ejs.db::Database.destroyDatabase */
        function destroyDatabase(name: String): Void

        /** @duplicate ejs.db::Database.destroyTable */
        function destroyTable(table: String): Void

        /** @duplicate ejs.db::Database.getColumns */
        function getColumns(table: String): Array

        /** @duplicate ejs.db::Database.getTables */
        function getTables(): Array

        /** @duplicate ejs.db::Database.removeColumns */
        function removeColumns(table: String, columns: Array): Void 

        /** @duplicate ejs.db::Database.removeIndex */
        function removeIndex(table: String, index: String): Void

        /** @duplicate ejs.db::Database.renameColumn */
        function renameColumn(table: String, oldColumn: String, newColumn: String): Void

        /** @duplicate ejs.db::Database.renameTable */
        function renameTable(oldTable: String, newTable: String): Void

        /** @duplicate ejs.db::Database.rollback 
            @hide
         */
        function rollback(): Void

        /** @duplicate ejs.db::Database.sql */
        function sql(cmd: String): Array

        /** @duplicate ejs.db::Database.sqlTypeToDataType */
        function sqlTypeToDataType(sqlType: String): String

        /** @duplicate ejs.db::Database.sqlTypeToEjsType */
        function sqlTypeToEjsType(sqlType: String): String

        /** @duplicate ejs.db::Database.startTransaction */
        function startTransaction(): Void
    }
}


/*
 *  @copy   default
 *  
 *  Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
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
    vim: sw=4 ts=4 expandtab

    @end
 */
