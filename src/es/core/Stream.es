/*
 *  Stream.es -- Stream class. Base interface implemented by Streams.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs {

    use default namespace intrinsic

    /**
     *  Stream objects represent bi-directional streams of data that pass data elements between an endpoint known 
     *  as a source or sink and a consumer / producer. In between, intermediate streams may be used as filters. 
     *  Example endpoints are the File, Socket, String and Http classes. The TextStream is an example of a filter 
     *  stream. The data elements passed by streams may be any series of objects including: bytes, lines of text, 
     *  integers or objects. Streams may buffer the incoming data or not. Streams may offer sync and/or async modes 
     *  of operation.
     *  @spec ejs
     *  @spec evolving
     */
    interface Stream {

        use default namespace public

        /**
         *  Close the input stream and free up all associated resources.
         *  WARNING: This API will have the graceful parameter removed in a future release. Call $flush manually if
         *  you require graceful closes.
         *  @param graceful if true, then close the socket gracefully after writing all pending data.
         *  @prototype
         */
        function close(graceful: Boolean = false): Void

        /**
         *  Flush the stream and all stacked streams and underlying data source/sinks.
         *  WARNING: This API will have the graceful parameter removed in a future release. All flushes will then
         *  be graceful.
         *  @param graceful if true, then write all pending data.
         *  @prototype
         */
        function flush(graceful: Boolean = true): Void 

        /**
         *  Read a block of data from the stream. Read the required number of bytes from the stream into the 
         *      supplied byte array at the given offset. 
         *  @param buffer Destination byte array for read data.
         *  @param offset Offset in the byte array to place the data. If the offset is -1, then data is
         *      appended to the buffer write $position which is then updated. 
         *  @param count Number of bytes to read. If -1, read as much as the buffer will hold up to the entire 
         *      stream if the buffer is of sufficient size or is growable.
         *  @returns a count of the bytes actually read.
         *  @throws IOError if an I/O error occurs.
         */
        function read(buffer: ByteArray, offset: Number = 0, count: Number = -1): Number 

        /**
         *  Write data to the stream. If in sync mode, the write call blocks until the underlying stream or 
         *  endpoint absorbes all the data. If in async-mode, the call accepts whatever data can be accepted 
         *  immediately and returns a count of the elements that have been written.
         *  @param data Data to write. 
         *  @returns The total number of elements that were written.
         *  @throws IOError if there is an I/O error.
         */
        function write(... data): Number
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
