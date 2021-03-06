/*
 *  Boolean.es -- Boolean class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs {

    use default namespace intrinsic

    /**
     *  Boolean class. The Boolean class is used to create two immutable boolean values: "true" and "false".
     *  @stability stable
     */
    native final class Boolean {

        /**
            Boolean constructor. Construct a Boolean object and initialize its value. Since Boolean values are 
            immutable, this constructor will return a reference to either the "true" or "false" values.
            @param value. Optional value to use in creating the Boolean object. If the value is omitted or 0, -1, NaN,
                false, null, undefined or the empty string, then the object will be created and set to false.
         */
        native function Boolean(value: Boolean = false)
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
