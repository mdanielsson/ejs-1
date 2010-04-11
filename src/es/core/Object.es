/*
 *  Object.es -- Object class. Base class for all types.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of this file.
 */
module ejs {

    use default namespace intrinsic

    /**
     *  The Object Class is the root class from which all objects are based. It provides a foundation set of functions 
     *  and properties which are available to all objects. It provides for: copying objects, evaluating equality of 
     *  objects, providing information about base classes, serialization and deserialization and iteration. 
        @stability evolving
     */
    dynamic native class Object implements Iterable {

        use default namespace public

        /**
         *  Clone the array and all its elements.
         *  @param deep If true, do a deep copy where all object references are also copied, and so on, recursively.
         *  @spec ejs
         */
        native function clone(deep: Boolean = true) : Array

        /**
         *  Get an iterator for this object to be used by "for (v in obj)"
         *  @return An iterator object.
         *  @spec ejs
         */
        iterator native function get(): Iterator

        /**
         *  Get an iterator for this object to be used by "for each (v in obj)"
         *  @return An iterator object.
         *  @spec ejs
         */
        iterator native function getValues(): Iterator

        /**
         *  The length of the object. For Objects, length() will be set to the number of properties. For Arrays, it will
         *  be set to the the number of elements. Other types will set length to the most natural representation of the
         *  size or length of the object.
         */
        native function get length(): Number 

        /**
         *  Convert an object to an equivalent JSON encoding.
         *  @return This function returns an object literal string.
         *  @throws TypeError If the object could not be converted to a string.
         */ 
        native function toJSON(): String

        /**
         *  This function converts an object to a string representation. Types typically override this to provide 
         *  the best string representation.
         *  @returns a string representation of the object. For Objects "[object className]" will be returned, 
         *  where className is set to the name of the class on which the object was based.
         */ 
        native function toString(): String
    }
}


/*
 *  @copy   default
 *  
 *  Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
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
 *  @end
 */
