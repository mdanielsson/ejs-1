/*
 *  Unix.es -- Unix compatibility functions
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.sys {

    use default namespace public

    /**
     *  Get the base name of a file. Returns the base name portion of a file name. The base name portion is the 
     *  trailing portion without any directory elements.
     *  @return A string containing the base name portion of the file name.
     */
    function basename(path: String): Path
        Path(path).basename
    
    /**
     *  Change the current working directory
     *  @param dir Directory String or path to change to
     */
    function chdir(dir: Object): Void
        App.chdir(dir)

    /**
     *  Set the permissions of a file or directory
     *  @param path File or directory to modify
     *  @param perms Posix style permission mask
     */
    function chmod(path: String, perms: Number): Void
        Path(path).perms = perms

    //  DEPRECATED 1.0.2
    /**
     *  Close the file and free up all associated resources.
     *  @param file Open file object previously opened via $open or $File
     *  @param graceful if true, then close the file gracefully after writing all pending data.
     *  @hide
     *  @deprecated
     */
    function close(file: File, graceful: Boolean = true): Void
        file.close(graceful)

    /**
     *  Copy a file. If the destination file already exists, the old copy will be overwritten as part of the copy operation.
     *  @param fromPath Original file to copy.
     *  @param toPath New destination file path name.
     *  @throws IOError if the copy is not successful.
     */
    function cp(fromPath: String, toPath: String): void
        Path(fromPath).copy(toPath) 

    /**
     *  Get the directory name portion of a file. The dirname name portion is the leading portion including all 
     *  directory elements and excluding the base name. On some systems, it will include a drive specifier.
     *  @return A string containing the directory name portion of the file name.
     */
    function dirname(path: String): Path
        Path(path).dirname

    /**
     *  Does a file exist. Return true if the specified file exists and can be accessed.
     *  @param path Filename path to examine.
     *  @return True if the file can be accessed
     */
    function exists(path: String): Boolean
        Path(path).exists

    /**
     *  Get the file extension portion of the file name. The file extension is the portion after the last "."
     *  in the path.
     *  @param path Filename path to examine
     *  @return String containing the file extension. It does not include "." as the first character.
     */
    function extension(path: String): String
        Path(path).extension

    /**
     *  Is a file a directory. Return true if the specified path exists and is a directory
     *  @param path Directory path to examine.
     *  @return True if the file can be accessed
     */
    function isDir(path: String): Boolean
        Path(path).isDir

    //  DEPRECATED 1.0.2
    /**
     *  Kill a process
     *  @param pid Process ID to kill
     *  @param signal Signal number to issue to kill the process
     *  @hide
     *  @deprecated
     */
    function kill(pid: Number, signal: Number = 2): Void {
        if (Config.OS == "WIN" || Config.OS == "CYGWIN") {
            Cmd.run("/bin/kill -f -" + signal + " " + pid)
        } else {
            Cmd.run("/bin/kill -" + signal + " " + pid)
        }
    }

    /**
     *  Get a list of files in a directory. The returned array contains the base file name portion only.
     *  @param path Directory path to enumerate.
     *  @param enumDirs If set to true, then dirList will include sub-directories in the returned list of files.
     *  @return An Array of strings containing the filenames in the directory.
     */
    function ls(path: String = ".", enumDirs: Boolean = false): Array
        Path(path).files(enumDirs)

    /**
     *  Find matching files. Files are listed in a depth first order.
     *  @param path Starting path from which to find matching files.
     *  @param glob Glob style Pattern that files must match. This is similar to a ls() style pattern.
     *  @param recurse Set to true to examine sub-directories. 
     *  @return Return a list of matching files
     */
    function find(path: Object, glob: String, recurse: Boolean = true): Array {
        let result = []
        if (path is Array) {
            let paths = path
            for each (path in paths) {
                result += Path(path).find(glob, recurse)
            }
        } else {
            result += Path(path).find(glob, recurse)
        }
        return result
    }

    /**
     *  Make a new directory. Makes a new directory and all required intervening directories. If the directory 
     *  already exists, the function returns without throwing an exception.
     *  @param path Filename path to use.
     *  @param permissions Optional posix permissions mask number. e.g. 0664.
     *  @throws IOError if the directory cannot be created.
     */
    function mkdir(path: String, permissions: Number = 0755): void
        Path(path).makeDir({permissions: permissions})
    
    /**
     *  Rename a file. If the new file name exists it is removed before the rename.
     *  @param fromFile Original file name.
     *  @param toFile New file name.
     *  @throws IOError if the original file does not exist or cannot be renamed.
     */
    function mv(fromFile: String, toFile: String): void
        Path(fromFile).rename(toFile)

    //  DEPRECATED 1.0.2
    /**
     *  Open or create a file. 
     *  @param path Filename path to open
     *  @param mode optional file access mode with values values from: Read, Write, Append, Create, Open, Truncate. 
     *      Defaults to Read.
     *  @param permissions optional permissions. Defaults to App.permissions.
     *  @return a File object which implements the Stream interface
     *  @throws IOError if the path or file cannot be opened or created.
     *  @hide
     *  @deprecated
     */
    function open(path: String, mode: String = "r", permissions: Number = App.permissions): File {
        print("IN OPEN")
        return new File(path, { mode: mode, permissions: permissions})
    }

    /**
     *  Get the current working directory
     *  @return A Path containing the current working directory
     */
    function pwd(): Path
        App.dir

    //  DEPRECATED 1.0.2
    /**
     *  Read data bytes from a file and return a byte array containing the data.
     *  @param file Open file object previously opened via $open or $File
     *  @param count Number of bytes to read
     *  @return A byte array containing the read data
     *  @throws IOError if the file could not be read.
     *  @hide
     *  @deprecated
     */
    function read(file: File, count: Number): ByteArray
        file.read(count)

    /**
     *  Remove a file from the file system.
     *  @param path Filename path to delete.
     *  @param quiet Don't complain if the file does not exist.
     *  @throws IOError if the file exists and cannot be removed.
     */
    function rm(path: Path): void {
        if (path.isDir) {
            throw new ArgError(path.toString() + " is a directory")
        } 
        Path(path).remove()
    }

    /**
     *  Removes a directory. This can remove a directory and its contents.  
     *  @param path Filename path to remove.
     *  @param contents If true, remove the directory contents including files and sub-directories.
     *  @throws IOError if the directory exists and cannot be removed.
     */
    function rmdir(path: Path, contents: Boolean = false): void {
        if (contents) {
            Path(path).removeAll()
        } else {
            Path(path).remove()
        }
    }

    /**
     *  Create a temporary file. Creates a new, uniquely named temporary file.
     *  @param directory Directory in which to create the temp file.
     *  @returns a closed File object after creating an empty temporary file.
     */
    function tempname(directory: String = null): File
        Path(directory).makeTemp()

    //  DEPRECATED 1.0.2
    /**
     *  Write data to the file. If the stream is in sync mode, the write call blocks until the underlying stream or 
     *  endpoint absorbes all the data. If in async-mode, the call accepts whatever data can be accepted immediately 
     *  and returns a count of the elements that have been written.
     *  @param file Open file object previously opened via $open or $File
     *  @param items The data argument can be ByteArrays, strings or Numbers. All other types will call serialize
     *  first before writing. Note that numbers will not be written in a cross platform manner. If that is required, use
     *  the BinaryStream class to write Numbers.
     *  @returns the number of bytes written.  
     *  @throws IOError if the file could not be written.
     *  @hide
     *  @deprecated
     */
    function write(file: File, ...items): Number
        file.write(items)
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
