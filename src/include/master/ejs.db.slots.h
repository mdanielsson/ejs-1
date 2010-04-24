/*
 * ejs.db.slots.h -- Property slot definitions for the "ejs.db" module
 *
 * This file is generated by ec
 *
 * Slot definitions. Version 1.0.0.
 */

#ifndef _h_SLOTS_EjsDbSlots
#define _h_SLOTS_EjsDbSlots 1


/**
 * Slots for the "ejs.db" module 
 */


/**
 *   Class property slots for the "global" class 
 */

/**
 * Instance slots for "global" type 
 */

/**
 * 
 *    Local slots for methods in type global 
 */


/**
 *   Class property slots for the "Database" class 
 */
#define ES_ejs_db_Database__origin                                     6
#define ES_ejs_db_Database_Database                                    6
#define ES_ejs_db_Database_addColumn                                   7
#define ES_ejs_db_Database_addIndex                                    8
#define ES_ejs_db_Database_changeColumn                                9
#define ES_ejs_db_Database_close                                       10
#define ES_ejs_db_Database_commit                                      11
#define ES_ejs_db_Database_connect                                     12
#define ES_ejs_db_Database_connection                                  13
#define ES_ejs_db_Database_createDatabase                              14
#define ES_ejs_db_Database_createTable                                 15
#define ES_ejs_db_Database_dataTypeToSqlType                           16
#define ES_ejs_db_Database_defaultDatabase                             17
#define ES_ejs_db_Database_set_defaultDatabase                         18
#define ES_ejs_db_Database_destroyDatabase                             19
#define ES_ejs_db_Database_destroyTable                                20
#define ES_ejs_db_Database_endTransaction                              21
#define ES_ejs_db_Database_getColumns                                  22
#define ES_ejs_db_Database_getTables                                   23
#define ES_ejs_db_Database_getNumRows                                  24
#define ES_ejs_db_Database_name                                        25
#define ES_ejs_db_Database_query                                       26
#define ES_ejs_db_Database_removeColumns                               27
#define ES_ejs_db_Database_removeIndex                                 28
#define ES_ejs_db_Database_renameColumn                                29
#define ES_ejs_db_Database_renameTable                                 30
#define ES_ejs_db_Database_rollback                                    31
#define ES_ejs_db_Database_sql                                         32
#define ES_ejs_db_Database_sqlTypeToDataType                           33
#define ES_ejs_db_Database_sqlTypeToEjsType                            34
#define ES_ejs_db_Database_startTransaction                            35
#define ES_ejs_db_Database_trace                                       36
#define ES_ejs_db_Database_transaction                                 37
#define ES_ejs_db_Database_quote                                       38
#define ES_ejs_db_Database_NUM_CLASS_PROP                              39

/**
 * Instance slots for "Database" type 
 */
#define ES_ejs_db_Database__adapter                                    0
#define ES_ejs_db_Database__connection                                 1
#define ES_ejs_db_Database__name                                       2
#define ES_ejs_db_Database__traceAll                                   3
#define ES_ejs_db_Database_NUM_INSTANCE_PROP                           4

/**
 * 
 *    Local slots for methods in type Database 
 */
#define ES_ejs_db_Database_Database_adapter                            0
#define ES_ejs_db_Database_Database_connectionString                   1
#define ES_ejs_db_Database_Database_adapterClass                       2
#define ES_ejs_db_Database_addColumn_table                             0
#define ES_ejs_db_Database_addColumn_column                            1
#define ES_ejs_db_Database_addColumn_datatype                          2
#define ES_ejs_db_Database_addColumn_options                           3
#define ES_ejs_db_Database_addIndex_table                              0
#define ES_ejs_db_Database_addIndex_column                             1
#define ES_ejs_db_Database_addIndex_index                              2
#define ES_ejs_db_Database_changeColumn_table                          0
#define ES_ejs_db_Database_changeColumn_column                         1
#define ES_ejs_db_Database_changeColumn_datatype                       2
#define ES_ejs_db_Database_changeColumn_options                        3
#define ES_ejs_db_Database_connect_connectionString                    0
#define ES_ejs_db_Database_createDatabase_name                         0
#define ES_ejs_db_Database_createDatabase_options                      1
#define ES_ejs_db_Database_createTable_table                           0
#define ES_ejs_db_Database_createTable_columns                         1
#define ES_ejs_db_Database_dataTypeToSqlType_dataType                  0
#define ES_ejs_db_Database_set_defaultDatabase_db                      0
#define ES_ejs_db_Database_destroyDatabase_name                        0
#define ES_ejs_db_Database_destroyTable_table                          0
#define ES_ejs_db_Database_getColumns_table                            0
#define ES_ejs_db_Database_getNumRows_table                            0
#define ES_ejs_db_Database_query_cmd                                   0
#define ES_ejs_db_Database_query_tag                                   1
#define ES_ejs_db_Database_query_trace                                 2
#define ES_ejs_db_Database_removeColumns_table                         0
#define ES_ejs_db_Database_removeColumns_columns                       1
#define ES_ejs_db_Database_removeIndex_table                           0
#define ES_ejs_db_Database_removeIndex_index                           1
#define ES_ejs_db_Database_renameColumn_table                          0
#define ES_ejs_db_Database_renameColumn_oldColumn                      1
#define ES_ejs_db_Database_renameColumn_newColumn                      2
#define ES_ejs_db_Database_renameTable_oldTable                        0
#define ES_ejs_db_Database_renameTable_newTable                        1
#define ES_ejs_db_Database_sql_cmd                                     0
#define ES_ejs_db_Database_sqlTypeToDataType_sqlType                   0
#define ES_ejs_db_Database_sqlTypeToEjsType_sqlType                    0
#define ES_ejs_db_Database_trace_on                                    0
#define ES_ejs_db_Database_transaction_code                            0
#define ES_ejs_db_Database_transaction__hoisted_1_e                    1
#define ES_ejs_db_Database_quote_str                                   0


/**
 *   Class property slots for the "DatabaseConnector" class 
 */
#define ES_ejs_db_DatabaseConnector__origin                            0
#define ES_ejs_db_DatabaseConnector_addColumn                          0
#define ES_ejs_db_DatabaseConnector_addIndex                           1
#define ES_ejs_db_DatabaseConnector_changeColumn                       2
#define ES_ejs_db_DatabaseConnector_close                              3
#define ES_ejs_db_DatabaseConnector_commit                             4
#define ES_ejs_db_DatabaseConnector_connect                            5
#define ES_ejs_db_DatabaseConnector_createDatabase                     6
#define ES_ejs_db_DatabaseConnector_createTable                        7
#define ES_ejs_db_DatabaseConnector_dataTypeToSqlType                  8
#define ES_ejs_db_DatabaseConnector_destroyDatabase                    9
#define ES_ejs_db_DatabaseConnector_destroyTable                       10
#define ES_ejs_db_DatabaseConnector_getColumns                         11
#define ES_ejs_db_DatabaseConnector_getTables                          12
#define ES_ejs_db_DatabaseConnector_removeColumns                      13
#define ES_ejs_db_DatabaseConnector_removeIndex                        14
#define ES_ejs_db_DatabaseConnector_renameColumn                       15
#define ES_ejs_db_DatabaseConnector_renameTable                        16
#define ES_ejs_db_DatabaseConnector_rollback                           17
#define ES_ejs_db_DatabaseConnector_sql                                18
#define ES_ejs_db_DatabaseConnector_sqlTypeToDataType                  19
#define ES_ejs_db_DatabaseConnector_sqlTypeToEjsType                   20
#define ES_ejs_db_DatabaseConnector_startTransaction                   21
#define ES_ejs_db_DatabaseConnector_NUM_CLASS_PROP                     22

/**
 * Instance slots for "DatabaseConnector" type 
 */
#define ES_ejs_db_DatabaseConnector_NUM_INSTANCE_PROP                  0

/**
 * 
 *    Local slots for methods in type DatabaseConnector 
 */
#define ES_ejs_db_DatabaseConnector_addColumn_table                    0
#define ES_ejs_db_DatabaseConnector_addColumn_column                   1
#define ES_ejs_db_DatabaseConnector_addColumn_datatype                 2
#define ES_ejs_db_DatabaseConnector_addColumn_options                  3
#define ES_ejs_db_DatabaseConnector_addIndex_table                     0
#define ES_ejs_db_DatabaseConnector_addIndex_column                    1
#define ES_ejs_db_DatabaseConnector_addIndex_index                     2
#define ES_ejs_db_DatabaseConnector_changeColumn_table                 0
#define ES_ejs_db_DatabaseConnector_changeColumn_column                1
#define ES_ejs_db_DatabaseConnector_changeColumn_datatype              2
#define ES_ejs_db_DatabaseConnector_changeColumn_options               3
#define ES_ejs_db_DatabaseConnector_connect_connectionString           0
#define ES_ejs_db_DatabaseConnector_createDatabase_name                0
#define ES_ejs_db_DatabaseConnector_createDatabase_options             1
#define ES_ejs_db_DatabaseConnector_createTable_table                  0
#define ES_ejs_db_DatabaseConnector_createTable_columns                1
#define ES_ejs_db_DatabaseConnector_dataTypeToSqlType_dataType         0
#define ES_ejs_db_DatabaseConnector_destroyDatabase_name               0
#define ES_ejs_db_DatabaseConnector_destroyTable_table                 0
#define ES_ejs_db_DatabaseConnector_getColumns_table                   0
#define ES_ejs_db_DatabaseConnector_removeColumns_table                0
#define ES_ejs_db_DatabaseConnector_removeColumns_columns              1
#define ES_ejs_db_DatabaseConnector_removeIndex_table                  0
#define ES_ejs_db_DatabaseConnector_removeIndex_index                  1
#define ES_ejs_db_DatabaseConnector_renameColumn_table                 0
#define ES_ejs_db_DatabaseConnector_renameColumn_oldColumn             1
#define ES_ejs_db_DatabaseConnector_renameColumn_newColumn             2
#define ES_ejs_db_DatabaseConnector_renameTable_oldTable               0
#define ES_ejs_db_DatabaseConnector_renameTable_newTable               1
#define ES_ejs_db_DatabaseConnector_sql_cmd                            0
#define ES_ejs_db_DatabaseConnector_sqlTypeToDataType_sqlType          0
#define ES_ejs_db_DatabaseConnector_sqlTypeToEjsType_sqlType           0


/**
 *   Class property slots for the "Record" class 
 */
#define ES_ejs_db_Record__origin                                       6
#define ES_ejs_db_Record_Record                                        6
#define ES_ejs_db_Record__initializer__Record_initializer              7
#define ES_ejs_db_Record_ejs_db_int__assocName                         8
#define ES_ejs_db_Record_ejs_db_int__belongsTo                         9
#define ES_ejs_db_Record_ejs_db_int__className                         10
#define ES_ejs_db_Record_ejs_db_int__columns                           11
#define ES_ejs_db_Record_ejs_db_int__hasOne                            12
#define ES_ejs_db_Record_ejs_db_int__hasMany                           13
#define ES_ejs_db_Record_ejs_db_int__db                                14
#define ES_ejs_db_Record_ejs_db_int__foreignId                         15
#define ES_ejs_db_Record_ejs_db_int__keyName                           16
#define ES_ejs_db_Record_ejs_db_int__tableName                         17
#define ES_ejs_db_Record_ejs_db_int__trace                             18
#define ES_ejs_db_Record_ejs_db_int__validations                       19
#define ES_ejs_db_Record_ejs_db_int__beforeFilters                     20
#define ES_ejs_db_Record_ejs_db_int__afterFilters                      21
#define ES_ejs_db_Record_ejs_db_int__wrapFilters                       22
#define ES_ejs_db_Record_ejs_db_ErrorMessages                          23
#define ES_ejs_db_Record_ejs_db_initialize                             24
#define ES_ejs_db_Record_ejs_db_afterFilter                            25
#define ES_ejs_db_Record_ejs_db_beforeFilter                           26
#define ES_ejs_db_Record_ejs_db_belongsTo                              27
#define ES_ejs_db_Record_cachedRead                                    28
#define ES_ejs_db_Record_checkFormat                                   29
#define ES_ejs_db_Record_checkNumber                                   30
#define ES_ejs_db_Record_checkPresent                                  31
#define ES_ejs_db_Record_checkUnique                                   32
#define ES_ejs_db_Record_coerceToEjsTypes                              33
#define ES_ejs_db_Record_createAssociations                            34
#define ES_ejs_db_Record_createRecord                                  35
#define ES_ejs_db_Record_ejs_db_error                                  36
#define ES_ejs_db_Record_ejs_db_find                                   37
#define ES_ejs_db_Record_ejs_db_findAll                                38
#define ES_ejs_db_Record_ejs_db_findOneWhere                           39
#define ES_ejs_db_Record_ejs_db_findWhere                              40
#define ES_ejs_db_Record_ejs_db_getColumnNames                         41
#define ES_ejs_db_Record_ejs_db_getColumnTitles                        42
#define ES_ejs_db_Record_ejs_db_getColumnType                          43
#define ES_ejs_db_Record_ejs_db_getDb                                  44
#define ES_ejs_db_Record_ejs_db_getErrors                              45
#define ES_ejs_db_Record_ejs_db_getKeyName                             46
#define ES_ejs_db_Record_ejs_db_getNumRows                             47
#define ES_ejs_db_Record_getSchema                                     48
#define ES_ejs_db_Record_ejs_db_getTableName                           49
#define ES_ejs_db_Record_ejs_db_hasAndBelongsToMany                    50
#define ES_ejs_db_Record_ejs_db_hasError                               51
#define ES_ejs_db_Record_ejs_db_hasMany                                52
#define ES_ejs_db_Record_ejs_db_hasOne                                 53
#define ES_ejs_db_Record_innerFind                                     54
#define ES_ejs_db_Record_makeLazyReader                                55
#define ES_ejs_db_Record_mapSqlTypeToEjs                               56
#define ES_ejs_db_Record_prepareValue                                  57
#define ES_ejs_db_Record_readRecords                                   58
#define ES_ejs_db_Record_ejs_db_remove                                 59
#define ES_ejs_db_Record_runFilters                                    60
#define ES_ejs_db_Record_ejs_db_save                                   61
#define ES_ejs_db_Record_ejs_db_saveUpdate                             62
#define ES_ejs_db_Record_ejs_db_setDb                                  63
#define ES_ejs_db_Record_ejs_db_setKeyName                             64
#define ES_ejs_db_Record_ejs_db_setTableName                           65
#define ES_ejs_db_Record_ejs_db_sql                                    66
#define ES_ejs_db_Record_ejs_db_trace                                  67
#define ES_ejs_db_Record_ejs_db_validateFormat                         68
#define ES_ejs_db_Record_ejs_db_validateNumber                         69
#define ES_ejs_db_Record_ejs_db_validatePresence                       70
#define ES_ejs_db_Record_ejs_db_validateRecord                         71
#define ES_ejs_db_Record_ejs_db_validateUnique                         72
#define ES_ejs_db_Record_ejs_db_wrapFilter                             73
#define ES_ejs_db_Record_ejs_db_columnNames                            74
#define ES_ejs_db_Record_ejs_db_columnTitles                           75
#define ES_ejs_db_Record_ejs_db_db                                     76
#define ES_ejs_db_Record_ejs_db_keyName                                77
#define ES_ejs_db_Record_ejs_db_numRows                                78
#define ES_ejs_db_Record_ejs_db_tableName                              79
#define ES_ejs_db_Record_ejs_db_constructor                            80
#define ES_ejs_db_Record_NUM_CLASS_PROP                                81

/**
 * Instance slots for "Record" type 
 */
#define ES_ejs_db_Record_ejs_db_int__keyValue                          0
#define ES_ejs_db_Record_ejs_db_int__errors                            1
#define ES_ejs_db_Record_ejs_db_int__cacheAssoc                        2
#define ES_ejs_db_Record_NUM_INSTANCE_PROP                             3

/**
 * 
 *    Local slots for methods in type Record 
 */
#define ES_ejs_db_Record_Record_fields                                 0
#define ES_ejs_db_Record_ejs_db_initialize_fields                      0
#define ES_ejs_db_Record_ejs_db_initialize__hoisted_1_field            1
#define ES_ejs_db_Record_ejs_db_afterFilter_fn                         0
#define ES_ejs_db_Record_ejs_db_afterFilter_options                    1
#define ES_ejs_db_Record_ejs_db_beforeFilter_fn                        0
#define ES_ejs_db_Record_ejs_db_beforeFilter_options                   1
#define ES_ejs_db_Record_ejs_db_belongsTo_owner                        0
#define ES_ejs_db_Record_ejs_db_belongsTo_options                      1
#define ES_ejs_db_Record_cachedRead_rec                                0
#define ES_ejs_db_Record_cachedRead_field                              1
#define ES_ejs_db_Record_cachedRead_model                              2
#define ES_ejs_db_Record_cachedRead_key                                3
#define ES_ejs_db_Record_cachedRead_options                            4
#define ES_ejs_db_Record_checkFormat_thisObj                           0
#define ES_ejs_db_Record_checkFormat_field                             1
#define ES_ejs_db_Record_checkFormat_value                             2
#define ES_ejs_db_Record_checkFormat_options                           3
#define ES_ejs_db_Record_checkNumber_thisObj                           0
#define ES_ejs_db_Record_checkNumber_field                             1
#define ES_ejs_db_Record_checkNumber_value                             2
#define ES_ejs_db_Record_checkNumber_options                           3
#define ES_ejs_db_Record_checkPresent_thisObj                          0
#define ES_ejs_db_Record_checkPresent_field                            1
#define ES_ejs_db_Record_checkPresent_value                            2
#define ES_ejs_db_Record_checkPresent_options                          3
#define ES_ejs_db_Record_checkUnique_thisObj                           0
#define ES_ejs_db_Record_checkUnique_field                             1
#define ES_ejs_db_Record_checkUnique_value                             2
#define ES_ejs_db_Record_checkUnique_options                           3
#define ES_ejs_db_Record_checkUnique_grid                              4
#define ES_ejs_db_Record_coerceToEjsTypes__hoisted_0_field             0
#define ES_ejs_db_Record_coerceToEjsTypes__hoisted_1_col               1
#define ES_ejs_db_Record_coerceToEjsTypes__hoisted_2_value             2
#define ES_ejs_db_Record_createAssociations_rec                        0
#define ES_ejs_db_Record_createAssociations_set                        1
#define ES_ejs_db_Record_createAssociations_preload                    2
#define ES_ejs_db_Record_createAssociations_options                    3
#define ES_ejs_db_Record_createAssociations__hoisted_4_model           4
#define ES_ejs_db_Record_createAssociations__hoisted_5_association     5
#define ES_ejs_db_Record_createAssociations__hoisted_6_field           6
#define ES_ejs_db_Record_createAssociations__hoisted_7_f               7
#define ES_ejs_db_Record_createAssociations__hoisted_8_field           8
#define ES_ejs_db_Record_createAssociations__hoisted_9_f               9
#define ES_ejs_db_Record_createRecord_data                             0
#define ES_ejs_db_Record_createRecord_options                          1
#define ES_ejs_db_Record_createRecord_rec                              2
#define ES_ejs_db_Record_createRecord_subOptions                       3
#define ES_ejs_db_Record_createRecord__hoisted_4_model                 4
#define ES_ejs_db_Record_createRecord__hoisted_5_model                 5
#define ES_ejs_db_Record_ejs_db_error_field                            0
#define ES_ejs_db_Record_ejs_db_error_msg                              1
#define ES_ejs_db_Record_ejs_db_find_key                               0
#define ES_ejs_db_Record_ejs_db_find_options                           1
#define ES_ejs_db_Record_ejs_db_find_grid                              2
#define ES_ejs_db_Record_ejs_db_find__hoisted_3_results                3
#define ES_ejs_db_Record_ejs_db_findAll_options                        0
#define ES_ejs_db_Record_ejs_db_findAll_grid                           1
#define ES_ejs_db_Record_ejs_db_findAll__hoisted_2_i                   2
#define ES_ejs_db_Record_ejs_db_findOneWhere_where                     0
#define ES_ejs_db_Record_ejs_db_findOneWhere_grid                      1
#define ES_ejs_db_Record_ejs_db_findWhere_where                        0
#define ES_ejs_db_Record_ejs_db_findWhere_count                        1
#define ES_ejs_db_Record_ejs_db_findWhere_grid                         2
#define ES_ejs_db_Record_ejs_db_findWhere__hoisted_3_i                 3
#define ES_ejs_db_Record_ejs_db_getColumnNames_result                  0
#define ES_ejs_db_Record_ejs_db_getColumnNames__hoisted_1_col          1
#define ES_ejs_db_Record_ejs_db_getColumnTitles_result                 0
#define ES_ejs_db_Record_ejs_db_getColumnTitles__hoisted_1_col         1
#define ES_ejs_db_Record_ejs_db_getColumnType_field                    0
#define ES_ejs_db_Record_ejs_db_getNumRows_cmd                         0
#define ES_ejs_db_Record_ejs_db_getNumRows_grid                        1
#define ES_ejs_db_Record_getSchema_sql                                 0
#define ES_ejs_db_Record_getSchema_grid                                1
#define ES_ejs_db_Record_getSchema__hoisted_2_row                      2
#define ES_ejs_db_Record_getSchema__hoisted_3_name                     3
#define ES_ejs_db_Record_getSchema__hoisted_4_sqlType                  4
#define ES_ejs_db_Record_getSchema__hoisted_5_ejsType                  5
#define ES_ejs_db_Record_ejs_db_hasAndBelongsToMany_model              0
#define ES_ejs_db_Record_ejs_db_hasAndBelongsToMany_options            1
#define ES_ejs_db_Record_ejs_db_hasError_field                         0
#define ES_ejs_db_Record_ejs_db_hasMany_model                          0
#define ES_ejs_db_Record_ejs_db_hasMany_options                        1
#define ES_ejs_db_Record_ejs_db_hasOne_model                           0
#define ES_ejs_db_Record_ejs_db_hasOne_options                         1
#define ES_ejs_db_Record_innerFind_key                                 0
#define ES_ejs_db_Record_innerFind_limit                               1
#define ES_ejs_db_Record_innerFind_options                             2
#define ES_ejs_db_Record_innerFind_cmd                                 3
#define ES_ejs_db_Record_innerFind_columns                             4
#define ES_ejs_db_Record_innerFind_from                                5
#define ES_ejs_db_Record_innerFind_conditions                          6
#define ES_ejs_db_Record_innerFind_where                               7
#define ES_ejs_db_Record_innerFind_results                             8
#define ES_ejs_db_Record_innerFind__hoisted_9_index                    9
#define ES_ejs_db_Record_innerFind__hoisted_10_model                   10
#define ES_ejs_db_Record_innerFind__hoisted_11_entry                   11
#define ES_ejs_db_Record_innerFind__hoisted_12_owner                   12
#define ES_ejs_db_Record_innerFind__hoisted_13_owner                   13
#define ES_ejs_db_Record_innerFind__hoisted_14_tname                   14
#define ES_ejs_db_Record_innerFind__hoisted_15_parts                   15
#define ES_ejs_db_Record_innerFind__hoisted_16_whereConditions         16
#define ES_ejs_db_Record_innerFind__hoisted_17_cond                    17
#define ES_ejs_db_Record_innerFind__hoisted_18_field                   18
#define ES_ejs_db_Record_innerFind__hoisted_19_e                       19
#define ES_ejs_db_Record_makeLazyReader_rec                            0
#define ES_ejs_db_Record_makeLazyReader_field                          1
#define ES_ejs_db_Record_makeLazyReader_model                          2
#define ES_ejs_db_Record_makeLazyReader_key                            3
#define ES_ejs_db_Record_makeLazyReader_options                        4
#define ES_ejs_db_Record_makeLazyReader___fun_3931__                   5
#define ES_ejs_db_Record_makeLazyReader_lazyReader                     6
#define ES_ejs_db_Record_mapSqlTypeToEjs_sqlType                       0
#define ES_ejs_db_Record_mapSqlTypeToEjs_ejsType                       1
#define ES_ejs_db_Record_prepareValue_field                            0
#define ES_ejs_db_Record_prepareValue_value                            1
#define ES_ejs_db_Record_prepareValue_col                              2
#define ES_ejs_db_Record_readRecords_key                               0
#define ES_ejs_db_Record_readRecords_options                           1
#define ES_ejs_db_Record_readRecords_data                              2
#define ES_ejs_db_Record_readRecords__hoisted_3_result                 3
#define ES_ejs_db_Record_readRecords__hoisted_4_row                    4
#define ES_ejs_db_Record_ejs_db_remove_keys                            0
#define ES_ejs_db_Record_ejs_db_remove__hoisted_1_key                  1
#define ES_ejs_db_Record_ejs_db_remove__hoisted_2_cmd                  2
#define ES_ejs_db_Record_runFilters_filters                            0
#define ES_ejs_db_Record_runFilters__hoisted_1_filter                  1
#define ES_ejs_db_Record_runFilters__hoisted_2_fn                      2
#define ES_ejs_db_Record_runFilters__hoisted_3_options                 3
#define ES_ejs_db_Record_runFilters__hoisted_4_only                    4
#define ES_ejs_db_Record_ejs_db_save_sql                               0
#define ES_ejs_db_Record_ejs_db_save_result                            1
#define ES_ejs_db_Record_ejs_db_save__hoisted_2_field                  2
#define ES_ejs_db_Record_ejs_db_save__hoisted_3_field                  3
#define ES_ejs_db_Record_ejs_db_save__hoisted_4_field                  4
#define ES_ejs_db_Record_ejs_db_saveUpdate_fields                      0
#define ES_ejs_db_Record_ejs_db_saveUpdate__hoisted_1_field            1
#define ES_ejs_db_Record_ejs_db_setDb_database                         0
#define ES_ejs_db_Record_ejs_db_setKeyName_name                        0
#define ES_ejs_db_Record_ejs_db_setTableName_name                      0
#define ES_ejs_db_Record_ejs_db_sql_cmd                                0
#define ES_ejs_db_Record_ejs_db_sql_count                              1
#define ES_ejs_db_Record_ejs_db_trace_on                               0
#define ES_ejs_db_Record_ejs_db_validateFormat_fields                  0
#define ES_ejs_db_Record_ejs_db_validateFormat_options                 1
#define ES_ejs_db_Record_ejs_db_validateNumber_fields                  0
#define ES_ejs_db_Record_ejs_db_validateNumber_options                 1
#define ES_ejs_db_Record_ejs_db_validatePresence_fields                0
#define ES_ejs_db_Record_ejs_db_validatePresence_options               1
#define ES_ejs_db_Record_ejs_db_validateRecord_thisType                0
#define ES_ejs_db_Record_ejs_db_validateRecord__hoisted_1_validation   1
#define ES_ejs_db_Record_ejs_db_validateRecord__hoisted_2_check        2
#define ES_ejs_db_Record_ejs_db_validateRecord__hoisted_3_fields       3
#define ES_ejs_db_Record_ejs_db_validateRecord__hoisted_4_options      4
#define ES_ejs_db_Record_ejs_db_validateRecord__hoisted_5_field        5
#define ES_ejs_db_Record_ejs_db_validateUnique_fields                  0
#define ES_ejs_db_Record_ejs_db_validateUnique_options                 1
#define ES_ejs_db_Record_ejs_db_wrapFilter_fn                          0
#define ES_ejs_db_Record_ejs_db_wrapFilter_options                     1
#define ES_ejs_db_Record_ejs_db_constructor_fields                     0


/**
 *   Class property slots for the "Column" class 
 */
#define ES_ejs_db_Column__origin                                       6
#define ES_ejs_db_Column_Column                                        6
#define ES_ejs_db_Column_NUM_CLASS_PROP                                7

/**
 * Instance slots for "Column" type 
 */
#define ES_ejs_db_Column_ejsType                                       0
#define ES_ejs_db_Column_sqlType                                       1
#define ES_ejs_db_Column_NUM_INSTANCE_PROP                             2

/**
 * 
 *    Local slots for methods in type Column 
 */
#define ES_ejs_db_Column_Column_name                                   0
#define ES_ejs_db_Column_Column_accessor                               1
#define ES_ejs_db_Column_Column_ejsType                                2
#define ES_ejs_db_Column_Column_sqlType                                3

#define _ES_CHECKSUM_ejs_db 307497

#endif
