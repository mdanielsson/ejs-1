/*
 *  Reflect.es - Reflection API and class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs {

    use default namespace intrinsic

    /**
     *  Simple reflection class. This class is prototype and likely to change.
     *  @spec ejs
     *  @stability evolving 
     *  @example
     *      name      = Reflect(obj).name
     *      type      = Reflect(obj).type
     *      typeName  = Reflect(obj).typeName
     */
    native final class Reflect {
        use default namespace public

        native private var obj: Object

        /**
         *  Create a new reflection object.
         *  @param obj to reflect upon
         */
        native function Reflect(obj: Object)

        /**
         *  The name of the object
         */
        native function get name(): String

        /**
         *  The type of the object
         */
        native function get type(): Type

        /**
         *  The name of the type of the object
         */
        native function get typeName(): String
    }

    /**
     *  Return the name of a type. This is a fixed version of the standard "typeof" operator. It returns the real
     *  Ejscript underlying type. 
     *  This is implemented as a wrapper around Reflect(o).typeName.
     *  @param o Object or value to examine. 
     *  @return A string type name. If the object to examine is a type object, then return the name of the base type.
     *      If the object is Object, then return null.
     *  @spec ejs
     */
    function typeOf(o): String
        Reflect(o).typeName
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
