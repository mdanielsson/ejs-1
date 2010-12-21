/**
 *  Iterator.es -- Iteration support via the Iterable interface and Iterator class. 
 *
 *  This provides a high performance native iterator for native classes. 
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs {

    /**
     *  Iterable is an internal interface used by native types to provide iterators for use in for/in statements.
        @hide
        @stability evolving
     */
    iterator interface Iterable {
        use default namespace iterator

        /**
         *  Get an Iterator for use with for/in
         *  @return An Iterator
         */
        function get(): Iterator

        /**
         *  Get an Iterator for use with for each in
         *  @return An Iterator
         */
        function getValues(): Iterator
    }

    /**
     *  Iterator is a helper class to implement iterators.
     *  @hide
     */
    iterator native final class Iterator {

        use default namespace public

        /**
         *  Return the next element in the object.
         *  @return An object
         *  @throws StopIteration
         */
        native function next(): Object
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
