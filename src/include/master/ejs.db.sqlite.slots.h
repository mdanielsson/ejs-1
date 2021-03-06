/*
 * ejs.db.sqlite.slots.h -- Property slot definitions for the "ejs.db.sqlite" module
 *
 * This file is generated by ec
 *
 * Slot definitions. Version 1.1.2.
 */

#ifndef _h_SLOTS_EjsDbSqliteSlots
#define _h_SLOTS_EjsDbSqliteSlots 1


/**
 * Slots for the "ejs.db.sqlite" module 
 */


/**
 *   Class property slots for the "global" class 
 */

/**
 * Instance slots for "global" type 
 */


/**
 *   Class property slots for the "Sqlite" class 
 */
#define ES_ejs_db_Sqlite__origin                                       6
#define ES_ejs_db_Sqlite_Sqlite                                        6
#define ES_ejs_db_Sqlite__initializer__Sqlite_initializer              7
#define ES_ejs_db_Sqlite_DataTypeToSqlType                             8
#define ES_ejs_db_Sqlite_SqlTypeToDataType                             9
#define ES_ejs_db_Sqlite_SqlTypeToEjsType                              10
#define ES_ejs_db_Sqlite_addColumn                                     11
#define ES_ejs_db_Sqlite_addIndex                                      12
#define ES_ejs_db_Sqlite_changeColumn                                  13
#define ES_ejs_db_Sqlite_close                                         14
#define ES_ejs_db_Sqlite_commit                                        15
#define ES_ejs_db_Sqlite_connect                                       16
#define ES_ejs_db_Sqlite_createDatabase                                17
#define ES_ejs_db_Sqlite_createTable                                   18
#define ES_ejs_db_Sqlite_dataTypeToSqlType                             19
#define ES_ejs_db_Sqlite_destroyDatabase                               20
#define ES_ejs_db_Sqlite_destroyTable                                  21
#define ES_ejs_db_Sqlite_endTransaction                                22
#define ES_ejs_db_Sqlite_getColumns                                    23
#define ES_ejs_db_Sqlite_getNumRows                                    24
#define ES_ejs_db_Sqlite_getTables                                     25
#define ES_ejs_db_Sqlite_removeColumns                                 26
#define ES_ejs_db_Sqlite_removeIndex                                   27
#define ES_ejs_db_Sqlite_renameColumn                                  28
#define ES_ejs_db_Sqlite_renameTable                                   29
#define ES_ejs_db_Sqlite_rollback                                      30
#define ES_ejs_db_Sqlite_query                                         31
#define ES_ejs_db_Sqlite_sql                                           32
#define ES_ejs_db_Sqlite_sqlTypeToDataType                             33
#define ES_ejs_db_Sqlite_sqlTypeToEjsType                              34
#define ES_ejs_db_Sqlite_startTransaction                              35
#define ES_ejs_db_Sqlite_NUM_CLASS_PROP                                36

/**
 * Instance slots for "Sqlite" type 
 */
#define ES_ejs_db_Sqlite_NUM_INSTANCE_PROP                             0

/**
 * 
 *    Local slots for methods in type Sqlite 
 */
#define ES_ejs_db_Sqlite_Sqlite_connectionString                       0
#define ES_ejs_db_Sqlite_addColumn_table                               0
#define ES_ejs_db_Sqlite_addColumn_column                              1
#define ES_ejs_db_Sqlite_addColumn_datatype                            2
#define ES_ejs_db_Sqlite_addColumn_options                             3
#define ES_ejs_db_Sqlite_addIndex_table                                0
#define ES_ejs_db_Sqlite_addIndex_column                               1
#define ES_ejs_db_Sqlite_addIndex_index                                2
#define ES_ejs_db_Sqlite_changeColumn_table                            0
#define ES_ejs_db_Sqlite_changeColumn_column                           1
#define ES_ejs_db_Sqlite_changeColumn_datatype                         2
#define ES_ejs_db_Sqlite_changeColumn_options                          3
#define ES_ejs_db_Sqlite_connect_connectionString                      0
#define ES_ejs_db_Sqlite_createDatabase_name                           0
#define ES_ejs_db_Sqlite_createDatabase_options                        1
#define ES_ejs_db_Sqlite_createTable_table                             0
#define ES_ejs_db_Sqlite_createTable_columns                           1
#define ES_ejs_db_Sqlite_createTable_cmd                               2
#define ES_ejs_db_Sqlite_createTable__hoisted_3_colspec                3
#define ES_ejs_db_Sqlite_createTable__hoisted_4_spec                   4
#define ES_ejs_db_Sqlite_createTable__hoisted_5_column                 5
#define ES_ejs_db_Sqlite_createTable__hoisted_6_datatype               6
#define ES_ejs_db_Sqlite_dataTypeToSqlType_dataType                    0
#define ES_ejs_db_Sqlite_destroyDatabase_name                          0
#define ES_ejs_db_Sqlite_destroyTable_table                            0
#define ES_ejs_db_Sqlite_getColumns_table                              0
#define ES_ejs_db_Sqlite_getColumns_names                              1
#define ES_ejs_db_Sqlite_getColumns__hoisted_2_row                     2
#define ES_ejs_db_Sqlite_getColumns__hoisted_3_name                    3
#define ES_ejs_db_Sqlite_getNumRows_table                              0
#define ES_ejs_db_Sqlite_getNumRows_cmd                                1
#define ES_ejs_db_Sqlite_getNumRows_grid                               2
#define ES_ejs_db_Sqlite_getTables_cmd                                 0
#define ES_ejs_db_Sqlite_getTables_grid                                1
#define ES_ejs_db_Sqlite_getTables_result                              2
#define ES_ejs_db_Sqlite_getTables__hoisted_3_row                      3
#define ES_ejs_db_Sqlite_getTables__hoisted_4_name                     4
#define ES_ejs_db_Sqlite_removeColumns_table                           0
#define ES_ejs_db_Sqlite_removeColumns_columns                         1
#define ES_ejs_db_Sqlite_removeColumns__hoisted_2_column               2
#define ES_ejs_db_Sqlite_removeColumns__hoisted_3_row                  3
#define ES_ejs_db_Sqlite_removeColumns__hoisted_4_name                 4
#define ES_ejs_db_Sqlite_removeColumns__hoisted_5_k                    5
#define ES_ejs_db_Sqlite_removeIndex_table                             0
#define ES_ejs_db_Sqlite_removeIndex_index                             1
#define ES_ejs_db_Sqlite_renameColumn_table                            0
#define ES_ejs_db_Sqlite_renameColumn_oldColumn                        1
#define ES_ejs_db_Sqlite_renameColumn_newColumn                        2
#define ES_ejs_db_Sqlite_renameTable_oldTable                          0
#define ES_ejs_db_Sqlite_renameTable_newTable                          1
#define ES_ejs_db_Sqlite_query_cmd                                     0
#define ES_ejs_db_Sqlite_query_tag                                     1
#define ES_ejs_db_Sqlite_query_trace                                   2
#define ES_ejs_db_Sqlite_sql_cmd                                       0
#define ES_ejs_db_Sqlite_sqlTypeToDataType_sqlType                     0
#define ES_ejs_db_Sqlite_sqlTypeToEjsType_sqlType                      0

#define _ES_CHECKSUM_ejs_db_sqlite 74622

#endif
