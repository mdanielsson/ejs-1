/*
 *  System.es - System class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.sys {
    /**
     *  System is a utility class providing methods to interact with the operating system.
     *  @spec ejs
     *  @stability prototype
     */
    native class System {

        use default namespace public

        public static const Bufsize: Number = 1024

        /**
         *  The fully qualified system hostname
         */
        native static function get hostname(): String

        /**
         *  Execute a command/program.
         *  @param cmd Command or program to execute
         *  @return a text stream connected to the programs standard output.
         *  @throws IOError if the command exits with non-zero status. 
         */
        native static function run(cmd: String): String

        /**
            Run a program without capturing stdout.
            @hide
         */
        native static function runx(cmd: String): Void

        /** @hide */
        native static function daemon(cmd: String): Number

        /**
            Run a command using the system command shell. This allows pipelines and also works better cross platform on
            Windows Cygwin.
            @hide
         */
        static function sh(args): String {
            let sh = locate("sh") 
            return System.run(sh + " -c \"" + args.replace(/\\/g, "\\\\") + "\"").trim('\n')
        }

        /**  TEMP deprecated @hide */
        static function cmd(args): String
            sh(args)

        /**  TEMP @hide */
        native static function exec(args): String
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
    vim: sw=4 ts=4 expandtab

    @end
 */
