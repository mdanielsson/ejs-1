/*
 *  App.es -- Application configuration and control. (Really controlling the interpreter's environment)
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.sys {

    /**
     *  Application configuration state. The App class is a singleton class object. The App class is accessed via
     *  the App global type object. It provides  methods to interrogate and control the applications environment including
     *  the current working directory, application command line arguments, path to the application's executable and
     *  input and output streams.
     *  @spec ejs
     *  @stability evolving
     */
    native class App {

        use default namespace public

        /**
         *  Application command line arguments. Set to an array containing each of the arguments. If the ejs command is 
         *      invoked as "ejs script arg1 arg2", then args[0] will be "script", args[1] will be "arg1" etc.
         */
        native static function get args(): Array

        /**
         *  The application's current directory
         */
        native static function get dir(): Path

        /**
         *  Change the application's Working directory
         *  @param value The path to the new working directory
         */
        native static function chdir(value: Object): Void

        /**
         *  The directory containing the application executable
         */
        native static function get exeDir(): Path

        /**
         *  The application executable path
         */
        native static function get exePath(): Path

        /**
         *  The application's standard error file stream
         */
        native static function get errorStream(): Stream

        /**
            Stop the program and exit.
            @param status The optional exit code to provide the environment. If running inside the ejs command program,
                the status is used as process exit status.
         */
        native static function exit(status: Number = 0): void

        /**
         *  Get an environment variable.
         *  @param name The name of the environment variable to retrieve.
         *  @return The value of the environment variable or null if not found.
         */
        native static function getenv(name: String): String

        /**
         *  The application's standard input file stream
         */
        native static function get inputStream(): Stream

        /**
         *  Application name.  Set to a single word, lower case name for the application.
         */
        static function get name(): String
            Config.Product

        /**
         *  Control whether an application will exit when global scripts have completed. Setting this to true will cause
         *  the application to continue servicing events until the $exit method is explicitly called. The default 
         *  application setting of noexit is false.
         *  @param exit If true, the application will exit when the last script completes.
         */
        native static function noexit(exit: Boolean = true): void

        /**
         *  The application's standard output file stream
         */
        native static function get outputStream(): Stream

        /**
         *  Update an environment variable.
         *  @param name The name of the environment variable to retrieve.
         *  @param value The new value to define for the variable.
         */
        native static function putenv(name: String, value: String): Void

        /** 
            The current module search path . Set to a delimited searchPath string. Warning: This will be changed to an
            array of paths in a future release.
            @stability prototype.
         */
        native static function get searchPath(): String

        /** 
            @duplicate App.searchPath
            Setting a search path will preserve certain system dependant paths that must be present.
            @param path Search path
         */
        native static function set searchPath(path: String): Void

        /**
         *  Service events
         *  @param count Count of events to service. Defaults to unlimited.
         *  @param timeout Timeout to block waiting for an event in milliseconds before returning. If an event occurs, the
         *      call returns immediately.
         */
        native static function serviceEvents(count: Number = -1, timeout: Number = -1): Void

        /**
         *  Set an environment variable.
         *  @param env The name of the environment variable to set.
         *  @param value The new value.
         *  @return True if the environment variable was successfully set.
         */
        # FUTURE
        native static function setEnv(name: String, value: String): Boolean

        /**
         *  Sleep the application for the given number of milliseconds
         *  @param delay Time in milliseconds to sleep. Set to -1 to sleep forever.
         */
        native static function sleep(delay: Number = -1): Void

        /**
         *  Application title name. Multi-word, Camel Case name for the application suitable for display.
         */
        static static function get title(): String
            Config.Title

        /**
         *  Application version string. Set to a version string of the format Major.Minor.Patch-Build. For example: 1.1.2-3.
         */
        static static function get version(): String
            Config.Version
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
