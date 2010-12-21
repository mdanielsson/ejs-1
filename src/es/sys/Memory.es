/*
 *  Memory.es -- Memory statistics
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.sys {

    /**
     *  Singleton class to monitor and report on memory allocation and usage.
     *  @spec ejs
     *  @stability evolving
     */
    native class Memory {

        use default namespace public

        /**
         *  Total heap memory currently allocated by the application in bytes. This includes memory currently in use and 
         *  also memory that has been freed but is still retained by the application for future use. It does not include 
         *  code, static data or stack memory. If you require these, use the $resident call.
         */
        native static function get allocated(): Number

        /**
         *  Memory redline callback. When the memory redline limit is exceeded, the callback will be invoked. 
         *  If no callback is defined and the redline limit is exceeded, a MemoryError exception is thrown. This callback
         *  enables the application detect low memory conditions before they become critical and to recover by freeing 
         *  memory or to gracefully exit. The callback is invoked with the following signature:
         *      function callback(size: Number, total: Number): Void
         *  @param fn Callback function to invoke when the redline limit is exceeded. While the callback is active
         *      subsequent invocations of the callback are suppressed.
         */
        native static function set callback(fn: Function): Void

        /** @hide */
        native static function get callback(): Void

        /**
            Maximum amount of heap memory the application may use in bytes. 
            This defines the upper limit for heap memory usage 
            by the entire hosting application. If this limit is reached, subsequent memory allocations will fail and 
            a $MemoryError exception will be thrown. Setting it to zero will allow unlimited memory allocations up 
            to the system imposed maximum. If $redline is defined and non-zero, the redline callback will be invoked 
            when the $redline is exceeded.
         */
        native static function get maximum(): Number

        /**
            @duplicate Memory.maximum
         *  @param value New maximum value in bytes
         */
        native static function set maximum(value: Number): Void

        /**
         *  Peak memory ever used by the application in bytes. This statistic is the maximum value ever attained by 
         *  $allocated. 
         */
        native static function get peak(): Number
        
        /**
            Memory redline value in bytes. When the memory redline limit is exceeded, the redline $callback will be invoked. 
            If no callback is defined, a MemoryError exception is thrown. The redline limit enables the application detect 
            low memory conditions before they become critical and to recover by freeing memory or to gracefully exit. 
            Note: the redline applies to the entire hosting application.
         */
        native static function get redline(): Number

        /**
         *  @duplicate Memory.redline
         *  @param value New memory redline limit in bytes
         */
        native static function set redline(value: Number): Void

        /**
         *  Application's current resident set in bytes. This is the total memory used to host the application and includes 
         *  all the the application code, data and heap. It is measured by the O/S.
         */
        native static function get resident(): Number

        /**
         *  Peak stack size ever used by the application in bytes. 
         */
        native static function get stack(): Number
        
        /**
         *  System RAM. This is the total amount of RAM installed in the system in bytes
         */
        native static function get system(): Number
        
        /**
         *  Prints memory statistics to stdout. This is primarily used during development for performance measurement.
         */
        native static function stats(): void
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
