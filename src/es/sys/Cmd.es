/*
 *  Cmd.es - Cmd class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.sys {

    /**
     *  The Cmd class supports invoking other programs on the same system. This class is prototype and will likely
     *  change in the next release.
     *  @spec ejs
     *  @stability prototype
     */
    class Cmd {

        use default namespace public

        /**
            Locate a command along the system search PATH
            @param program Program to search for
            @returns The first located occurence of the command on the PATH
         */
        static function locate(program: Path): Path {
            let sep = (Config.OS == "WIN") ? ";" : ":"
            for each (dir in App.getenv("PATH").split(sep)) {
                let path = Path(dir).join(program)
                if (path.exists && !path.isDir) {
                    return path
                }
            }
            if (Config.OS == "WIN" || Config.OS == "CYGWIN") {
                if (program.extension == "") {
                    for each (ext in ["exe", "bat", "cmd"]) {
                        let path = locate(program.joinExt(".exe"))
                        if (path) {
                            return path;
                        }
                    }
                }
            }
            return null
        }

        /**
         *  Run a command using the system command shell and wait for completion. This supports pipelines.
         *  @param cmdline Command or program to execute
         *  @returns The command output from it's standard output.
         *  @throws IOError if the command exits with non-zero status. The exception object will contain the command's
         *      standard error output. 
         */
        static function sh(cmdline: String, data: String = null): String
        {
            let sh = Cmd.locate("sh")
            return run((sh + " -c \"" + cmdline.replace(/\\/g, "\\\\") + "\"").trim('\n'), data)
        }

        /**
         *  Execute a command/program.
         *  @param cmdline Command or program to execute
         *  @param data Optional data to write to the command on it's standard input. Not implemented.
         *  @returns The command output from it's standard output.
         *  @throws IOError if the command exits with non-zero status. The exception object will contain the command's
         *      standard error output. 
         */
        static function run(cmdline: String, data: String = null): String
            System.run(cmdline)

        /**
         *  Execute a command and detach. This will not capture output nor will it wait for the command to complete
         *  @param cmdline Command or program to execute
         *  @returns The commands process ID
         */
        static function daemon(cmdline: String): Number 
            System.daemon(cmdline)

        /**
         *  Run a command and don't capture output. Output and errors go to the existing stdout
         *  @hide
         */
        static function runx(cmdline: String): String
            System.runx(cmdline)
    }

    /**
     *  Data event issued to the callback function.
     */
    # FUTURE
    class CmdDataEvent extends Event {
        /**
         *  Mask of pending events. Set to include $Read and/or $Write values.
         */
        public var eventMask: Number
    }

    /**
     *  Error event issued to the callback function if any errors occur dcmdlineng an Cmd request.
     */
    # FUTURE
    class CmdErrorEvent extends Event {
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
