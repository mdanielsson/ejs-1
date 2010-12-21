/*
    Path.es --  Path class. Path's represent files in a file system. The file may or may not exist.  

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.io {

    /**
        Path class. Paths are filenames and may or may not represent physical files in a file system. That is, the 
        corresponding file or directory for the Path  may or may not exist. Once created, Paths are immutable and their
        path value cannot be changed.
        @spec ejs
        @stability evolving
     */
    native class Path {

        use default namespace public

        /**
            Create a new Path object and set the path's location.
            @param path Name of the path to associate with this path object.
         */
        native function Path(path: String)

        /**
            An absolute path equivalent for the current path. The path is normalized.
         */
        native function get absolute(): Path

        /**
            Get when the file was last accessed. Set to null if the file does not exist.
         */
        native function get accessed(): Date 

        /**
            Base of portion of the path. The base portion is the trailing portion without any directory elements.
         */
        native function get basename(): Path
        
        /**
            Path components. This is the path converted to an absolute path and then broken into components for each
            directory level. It is set to an array object with an element for each segment of the path. The first 
                element represents the file system root and will be the empty string or drive spec on a windows 
                like systems. Array.join("/") can be used to put components back into a complete path.
         */
        native function get components(): Array
  
        /**
            Test if the path name contains a substring
            @param pattern String pattern to search for
            @return Boolean True if found.
         */
        function contains(pattern: String): Boolean
            name.contains(pattern)

        /**
            Copy a file
            @param target New file location
            @param options Object hash
            @options permissions Set to a numeric Posix permissions mask. Not implemented.
         */
        native function copy(target: Object, options: Object = null): Void

        /**
            When was the file was created. Set to null if the file does not exist.
         */
        native function get created(): Date 

        /**
            The directory portion of a file. The directory portion is the leading portion including all 
            directory elements and excluding the base name. On some systems, it will include a drive specifier.
         */
        native function get dirname(): Path

        /**
            Return true if the path ends with the given suffix
            @param suffix String suffix to compare with the path.
            @return true if the path does begin with the suffix
         */
        function endsWith(suffix: String): Boolean
            this.toString().endsWith(suffix)

        /**
            Does a file exist for this path.
         */
        native function get exists(): Boolean 

        /**
            File extension portion of the path. The file extension is the portion after (and not including) the last ".".
         */
        native function get extension(): String 

        /**
            Find matching files. Files are listed in depth first order.
            @param glob Glob style Pattern that files must match. This is similar to a ls() style pattern.
            @param recurse Set to true to examine sub-directories. 
            @return Return a list of matching files
         */
        function find(glob: String = "*", recurse: Boolean = true): Array {
            function recursiveFind(path: Path, pattern: RegExp, level: Number): Array {
                let result: Array = []
                if (path.isDir && (recurse || level == 0)) {
                    for each (f in path.files(true)) {
                        let got: Array = recursiveFind(f, pattern, level + 1)
                        for each (i in got) {
                            result.append(i)
                        }
                    }
                }
                if (Config.OS == "WIN") {
                    if (path.basename.toString().toLower().match(pattern)) {
                        result.append(path)
                    }
                } else {
                    if (path.basename.toString().match(pattern)) {
                        result.append(path)
                    }
                }
                return result
            }
            if (Config.OS == "WIN") {
                glob = glob.toLower()
            }
            pattern = RegExp("^" + glob.replace(/\./g, "\\.").replace(/\*/g, ".*") + "$")
            return recursiveFind(this, pattern, 0)
        }

        /**
            Get a list of files in a directory. The returned array contains the base path portion only, relative to the
            path itself. This routine does not recurse into sub-directories. To get a list of files in sub-directories,
            use find().
            @param enumDirs If set to true, then dirList will include sub-directories in the returned list of files.
            @return An Array of Path objects for each file in the directory.
         */
        native function files(enumDirs: Boolean = false): Array 

        /**
            Get iterate over any files contained under this path (assuming it is a directory) "for (v in files)". 
                This operates the same as getValues on a Path object.
            @return An iterator object.
            @example:
                for (f in Path("."))
         */
        override iterator native function get(): Iterator

        /**
            Get an iterator for this file to be used by "for each (v in obj)". Return 
                This operates the same as $get on a Path object.
            @return An iterator object.
            @example
                for each (f in Path("."))
         */
        override iterator native function getValues(): Iterator

        /**
            Does the file path have a drive spec (C:) in it's name. Only relevant on Windows like systems.
         */
        native function get hasDrive(): Boolean 

        /**
            Is the path absolute, i.e. Beginning at the file system's root.
         */
        native function get isAbsolute(): Boolean

        /**
            Is the path a directory
         */
        native function get isDir(): Boolean

        /**
            Is the path is a symbolic link. Not available on some platforms (such as Windows and VxWorks).
         */
        native function get isLink(): Boolean

        /**
            Is the path a regular file
         */
        native function get isRegular(): Boolean

        /**
            Is the path is relative and not absolute.
         */
        native function get isRelative(): Boolean

        /**
            Join paths. Joins one more more paths together. If the other path is absolute, return it. Otherwise
            add a separator, and continue joining. The separator is chosen to match the first separator found in 
            either path. If none found, the default file system path separator is used.
            @return A new joined, normalized Path object.
         */
        native function join(...other): Path

        /**
            Join an extension to a path. If the base name of the path already has an extension, this call does nothing.
            @return A path with extension.
         */
        native function joinExt(ext: String): Path

        /**
            The length of the path name in bytes. Note: this is not the file size. For that, use Path.size
         */
        override native function get length(): Number 

        /**
            The target pointed to if this path is a symbolic link. Not available on some platforms such as Windows and 
            VxWorks. If the path is not a symbolic link, it is set to null.
         */
        native function get linkTarget(): Path

        /**
            Make a new directory and all required intervening directories. If the directory already exists, 
                the function returns without throwing an exception.
            @param options
            @options permissions Set to a numeric Posix permissions mask
            @options owner String representing the file owner (Not implemented)
            @options group String representing the file group (Not implemented)
            @throws IOError if the directory cannot be created.
         */
        native function makeDir(options: Object = null): Void

        /**
            Create a link to a file. Links are not supported on all platforms (such as Windows and VxWORKS).
            @param existing Path to an existing file to link to.
            @param hard Set to true to create a hard link. Otherwise the default is to create a symbolic link.
         */
        native function makeLink(existing: Path, hard: Boolean = false): Void

        /**
            Create a temporary file in the path directory. Creates a new, uniquely named temporary file.
            @returns a new Path object for the temp file.
         */
        native function makeTemp(): Path

        /**
            Get a path after mapping the path directory separator
            @param separator Path directory separator to use
            @return a new Path after mapping separators
         */
        native function map(separator: String): Path

        /** 
            Get the mime type for a path or extension string. The path's extension is used to lookup the corresponding
            mime type.
            @returns The mime type string
         */
        native function get mimeType(): String

        /**
            When the file was created or last modified. Set to null the file does not exist.
         */
        native function get modified(): Date 

        /**
            Name of the Path as a string. This is the same as toString().
         */
        native function get name(): String 

        /**
            Natural (native) respresentation of the path. This uses the default O/S file system path separator, 
            this is "\" on windows and "/" on unix and is normalized.
         */
        native function get natural(): Path 

        /**
            Normalized representation of the path. This removes "segment/.." and "./" components. Separators are made 
            uniform by converting all separators to be the same as the first separator found in the path. Note: the 
            value is not converted to an absolute path.
         */
        native function get normalize(): Path

        /**
            Open a path and return a File object.
            @params options
            @options mode optional file access mode string. Use "r" for read, "w" for write, "a" for append to existing
                content, "+" never truncate.
            @options permissions optional Posix permissions number mask. Defaults to 0664.
            @options owner String representing the file owner (Not implemented)
            @options group String representing the file group (Not implemented)
            @return a File object which implements the Stream interface.
            @throws IOError if the path or file cannot be created.
         */
        function open(options: Object = null): File
            new File(this, options)

        /**
            Open a file and return a TextStream object.
            @params options Optional options object
            @options mode optional file access mode string. Use "r" for read, "w" for write, "a" for append to existing
                content, "+" never truncate.
            @options encoding Text encoding format
            @options permissions optional Posix permissions number mask. Defaults to 0664.
            @options owner String representing the file owner (Not implemented)
            @options group String representing the file group (Not implemented)
            @return a TextStream object which implements the Stream interface.
            @throws IOError if the path or file cannot be opened or created.
         */
        function openTextStream(options: Object = null): TextStream
            new TextStream(open(options))

        /**
            Open a file and return a BinaryStream object.
            @params options Optional options object
            @param mode optional file access mode with values ored from: Read, Write, Append, Create, Open, Truncate. 
                Defaults to Read.
            @options permissions optional Posix permissions number mask. Defaults to 0664.
            @options owner String representing the file owner (Not implemented)
            @options group String representing the file group (Not implemented)
            @return a BinaryStream object which implements the Stream interface.
            @throws IOError if the path or file cannot be opened or created.
         */
        function openBinaryStream(options: Object = null): BinaryStream
            new BinaryStream(open(options))

        /**
            The parent directory of path. This may be an absolute path if there are no parent directories in the path.
         */
        native function get parent(): Path

        /**
            The file permissions of a path. This number contains the Posix style permissions value or null if the file 
            does not exist. NOTE: this is not a string representation of an octal posix mask. 
         */
        native function get perms(): Number

        /** */
        native function set perms(perms: Number): Void

        /**
            The path in a portable (like Unix) representation. This uses "/" separators. The value is is normalized and 
            the separators are mapped to "/".
         */
        native function get portable(): Path 

        /**
            Read the file contents as a ByteArray. This method opens the file, reads the contents, closes the file and
                returns a new ByteArray with the contents.
            @return A ByteArray
            @throws IOError if the file cannot be read
            @example:
                var b: ByteArray = Path("/tmp/a.txt").readBytes()
         */
        function readBytes(): ByteArray {
            let file: File = File(this).open()
            result = file.readBytes()
            file.close()
            return result
        }

        /**
            Read the file contents as an array of lines. Each line is a string. This method opens the file, reads the 
                contents and closes the file.
            @return An array of strings.
            @throws IOError if the file cannot be read
            @example:
                for each (line in Path("/tmp/a.txt").readLines())
         */
        function readLines(): Array {
            let stream: TextStream = TextStream(open(this))
            result = stream.readLines()
            stream.close()
            return result
        }

        /**
            Read the file contents as a string. This method opens the file, reads the contents and closes the file.
            @return A string.
            @throws IOError if the file cannot be read
            @example:
                data = Path("/tmp/a.txt").readString()
         */
        function readString(): String {
            let file: File = open(this)
            result = file.readString()
            file.close()
            return result
        }

        /**
            Read the file contents as an XML object.  This method opens the file, reads the contents and closes the file.
            @return An XML object
            @throws IOError if the file cannot be read
         */
        function readXML(): XML {
            let file: File = open(this)
            let data = file.readString()
            file.close()
            if (data == null) {
                return null
            }
            return new XML(data)
        }

        /**
            That path in a form relative to the application's current working directory. The value is normalized.
         */
        native function get relative(): Path

        /**
            Delete the file associated with the Path object. If this is a directory without contents it will be removed.
            @throws IOError if the file exists and could not be deleted.
         */
        native function remove(): Void 

        /**
            Removes a directory and its contents
            If the path is a directory, this call will remove all subdirectories and their contents and finally the
            directory itself. If the directory does not exist, this call does not error and does nothing.
            @throws IOError if the directory exists and cannot be removed.
         */
        function removeAll(): Void {
            for each (f in find('*')) {
                f.remove()
            }
            remove()
        }

        /**
            Rename a file. If the new path exists it is removed before the rename.
            @param target New name of the path. Can be either a Path or String.
            @throws IOError if the original file does not exist or cannot be renamed.
         */
        native function rename(target: Object): Void
        
        /**
            Replace the path extension and return a new path.
            @return A new path with joined extension.
         */
        function replaceExt(ext: String): Path
            this.trimExt().joinExt(ext)

        /**
            Resolve paths in the neighborhood of this path. Resolve operates like join, except that it joins the 
            given paths to the directory portion of the current ("this") path. For example: if "path" is set to
            "/usr/bin/ejs/bin", then path.resolve("lib") will return "/usr/lib/ejs/lib". i.e. it will return the
            sibling directory "lib".

            Resolve operations by accumulating a virtual current directory, starting with "this" path. It then
            successively joins the given paths. If the next path is an absolute path, it is used unmodified. 
            The effect is to find the given paths with a virtual current directory set to the directory containing 
            the prior path.

            Resolve is useful for creating paths in the region of the current path and gracefully handles both 
            absolute and relative path segments.
            @return A new Path object that is resolved against the prior path. 
         */
        native function resolve(...otherPaths): Path

        /**
            Compare two paths test if they represent the same file
            @param other Other path to compare with
            @return True if the paths represent the same underlying filename
         */
        native function same(other: Object): Boolean

        /**
            The path separator for this path. This will return the first valid path separator used by the path
            or the default file system path separator if the path has no separators. On Windows, a path may contain
            "/" and "\" separators.  This will be set to a string containing the first separator found in the path.
            Will typically be either "/" or "/\\" depending on the path, platform and file system.
            Use $natural or $portable to create a new path with different path separators.
         */
        native function get separator(): String

        /**
            Size of the file associated with this Path object. Set to number of bytes in the file or -1 if the size
             determination failed.
         */
        native function get size(): Number 

        /**
            Return true if the path starts with the given prefix
            @param prefix String prefix to compare with the path.
            @return true if the path does begin with the prefix
         */
        function startsWith(prefix: String): Boolean
            this.toString().startsWith(prefix) 

        /**
            Convert the path to a JSON string. 
            @return a JSON string representing the path.
         */
        native override function toJSON(): String

        /**
            Convert the path to lower case
            @return a new Path mapped to lower case
         */
        function toLower(): Path
            new Path(name.toString().toLower())

        /**
            Return the path as a string. The path is not translated.
            @return a string representing the path.
         */
        native override function toString(): String

        /**
            Trim a pattern from the end of the path
            NOTE: this does a case-sensitive match
            @return a Path containing the trimmed path name
         */
        function trimEnd(pat: String): Path {
            if (name.endsWith(pat)) {
                loc = name.lastIndexOf(pat)
                if (loc >= 0) {
                    return new Path(name.slice(0, loc))
                }
            }
            return this
        }

        /**
            Trim a file extension from a path
            @return a Path with no extension
         */
        native function trimExt(): Path

        /**
            Trim a pattern from the start of the path
            NOTE: this does a case-sensitive match
            @return a Path containing the trimmed path name
         */
        function trimStart(pat: String): Path {
            if (name.startsWith(pat)) {
                return new Path(name.slice(pat.length))
            }
            return this
        }

        /**
            Reduce the size of the file by truncating it. 
            @param size The new size of the file. If the truncate argument is greater than or equal to the 
                current file size nothing happens.
            @throws IOError if the truncate failed.
         */
        native function truncate(size: Number): Void

        /**
            Write the file contents. This method opens the file, writes the contents and closes the file.
            @param args The data to write to the file. Data is serialized in before writing. Note that numbers will not 
                be written in a cross platform manner. If that is required, use the BinaryStream class to write Numbers.
            @throws IOError if the data cannot be written
         */
        function write(...args): Void {
            var file: File = new File(this, { mode: "w", permissions: 0644 })
            try {
                for each (item in args) {
                    if (item is String) {
                        file.write(item)
                    } else {
                        file.write(serialize(item))
                    }
                }
            } 
            catch (es) {
                throw new IOError("Can't write to file")
            }
            finally {
                file.close()
            }
        }
    }
}

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
