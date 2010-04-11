/*
 *  Global.es -- Global variables, namespaces and functions.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/*
 *  Notes:
 *      native means supplied as a builtin (native C/Java)
 *      intrinsic implies ReadOnly, DontDelete, DontEnum
 */

module ejs {

    use strict

    /** @hide */
    public var ECMA: Boolean = false

    /**
     *  The public namespace used to make entities visible accross modules.
     */
    public namespace public

    /**
     *  The internal namespace used to make entities visible within a single module only.
     */
    public namespace internal

    /**
     *  The intrinsic namespace used for entities that are part of and intrinsic to, the Ejscript platform.
     */
    public namespace intrinsic

    /**
     *  The iterator namespace used to defined iterators.
     */
    public namespace iterator

    /**
     *  The CONFIG namespace used to defined conditional compilation directives.
     */
    public namespace CONFIG

    use default namespace intrinsic
    use namespace iterator

    use namespace "ejs.sys"

    /** 
        Conditional compilation constant. Used to disable compilation of certain elements.
        @hide
     */  
    const TODO: Boolean = false

    /** 
     *  Conditional compilation constant. Used to disable compilation of certain elements.
     */  
    const FUTURE: Boolean = false

    /** 
     *  Conditional compilation constant. Used to disable compilation of certain elements.
     */  
    const ASC: Boolean = false

    /** 
     *  Conditional compilation constant. Used to enable the compilation of elements only for creating the API documentation.
     */  
    const DOC_ONLY: Boolean = false

    /** 
     *  Conditional compilation constant. Used to deprecate elements.
     */  
    const DEPRECATED: Boolean = false

    /** 
     *  Conditional compilation constant. Used to deprecate elements.
     */  
    const REGEXP: Boolean = true

    /**
     *  Alias for the Boolean type
     */
    native const boolean: Type = Boolean

    /**
     *  Alias for the Number type
     */
    native const double: Type = Number

    /**
     *  Alias for the Number type
     *  @spec ejs
     */
    native const num: Type = Number

    /**
     *  Alias for the String type
     */
    native const string: Type = String

    /**
     *  Boolean false value.
     */
    native const false: Boolean

    /**
     *  Global variable space reference. The global variable references an object which is the global variable space. 
     *  This is useful when guaranteed access to a global variable is required. e.g. global.someName.
     */
    native var global: Object

    /**
     *  Null value. The null value is returned when testing a nullable variable that has not yet had a 
     *  value assigned or one that has had null explicitly assigned.
     */
    native const null: Null

    /**
     *  Infinity value.
     */
    native const Infinity: Number

    /**
     *  Negative infinity value.
     */
    native const NegativeInfinity: Number

    /**
     *  Invalid numeric value. If the numeric type is set to an integral type, the value is zero.
     */
    native const NaN: Number

    /**
     *  StopIteration class. Iterators throw the StopIteration class instance to signal the end of iteration in for/in loops.
     *  @spec ejs
     */
    iterator native final class StopIteration {}

    /**
     *  True value.
     */
    native const true: Boolean

    /**
     *  Undefined variable value. The undefined value is returned when testing
     *  for a property that has not been defined. 
     */
    native const undefined: Void

    /**
     *  void type value. 
     *  This is an alias for Void.
     *  @spec ejs
     */
    native const void: Type = Void

    /**
     *  Assert a condition is true. This call tests if a condition is true by testing to see if the supplied 
     *  expression is true. If the expression is false, the interpreter will throw an exception.
     *  @param condition JavaScript expression evaluating or castable to a Boolean result.
     *  @return True if the condition is.
     *  @spec ejs
     */
    native function assert(condition: Boolean): Boolean

    /**
     *  Convenient way to trap to the debugger
     */
    native function breakpoint(): Void

    /**
     *  Replace the base type of a type with an exact clone. 
     *  @param klass Class in which to replace the base class.
     *  @spec ejs
     */
    native function cloneBase(klass: Type): Void

    /**
     *  Convert a string into an object. This will parse a string that has been encoded via serialize. It may contain
     *  nested objects and arrays. This is a static method.
     *  @param obj The string containing the object encoding.
     *  @return The fully constructed object or undefined if it could not be reconstructed.
     *  @throws IOError If the object could not be reconstructed from the string.
     *  @spec ejs
     */
    native function deserialize(obj: String): Object

    /**
     *  Reverse www-url encoding on a string
     *  @param str URL encoded string
     *  @returns a decoded string
     */
    native function decodeURI(str: String): String

    /**
     *  Dump the contents of objects. Used for debugging, this routine serializes the objects and prints to the standard
     *  output.
     *  @param args Variable number of arguments of any type
     */
    function dump(...args): Void {
        for each (var e: Object in args) {
            print(serialize(e))
        }
    }

    /**
     *  Write to the standard error. This call writes the arguments to the standard error with a new line appended. 
     *  It evaluates the arguments, converts the result to strings and prints the result to the standard error. 
     *  Arguments are converted to strings by calling their toSource method.
     *  @param args Data to write
     *  @spec ejs
     */
    native function error(...args): void

    /**
     *  HTML escape a string. This quotes characters which would otherwise be interpreted as HTML directives.
     *  @param str String to html escape
     *  @returns a HTML escaped string
     *  @spec ejs
     */
    native function escape(str: String): String

    /**
     *  Encode a string using  www-url encoding
     *  @param str URL encoded string
     *  @returns an encoded string
     *  @spec ejs
     */
    native function encodeURI(str: String): String

    /**
     *  Computed an MD5 sum of a string
     *  This function is prototype and may be renamed in a future release.
     *  @param str String on which to compute a checksum
     *  @returns An MD5 checksum
     *  @spec ejs
     */
    native function md5(str: String): String

    /**
     *  Evaluate a script. Not present in ejsvm.
     *  @param script Script to evaluate
     *  @returns the the script expression value.
     */
    native function eval(script: String): Object

    /**
     *  Format the current call stack. Used for debugging and when creating exception objects.
     *  @spec ejs
     */
    native function formatStack(): String

    /**
     *  Get the object's Unique hash id. All objects have a unique object hash. 
     *  @return This property accessor returns a long containing the object's unique hash identifier. 
     */ 
    native function hashcode(o: Object): Number

    /**
     *  Read from the standard input. This call reads a line of input from the standard input
     *  @return A string containing the input. Returns null on EOF.
     */
    native function input(): String

    /**
     *  Load a script or module
     *  @param file path name to load. File will be interpreted relative to EJSPATH if it is not found as an absolute or
     *      relative file name.
     */
    native function load(file: String): Void

    /**
     *  Print the arguments to the standard output with a new line appended. This call evaluates the arguments, 
     *  converts the result to strings and prints the result to the standard output. Arguments are converted to 
     *  strings by calling their toString method.
     *  @param args Variables to print
     *  @spec ejs
     */
    native function output(...args): void

    /**
     *  Print the arguments to the standard output with a new line appended. This call evaluates the arguments, 
     *  converts the result to strings and prints the result to the standard output. Arguments are converted to 
     *  strings by calling their toString method. This method invokes $output as its implementation. 
     *  @param args Variables to print
     *  @spec ejs
     */
    native function print(...args): void

    /**
     *  Print variables for debugging.
     *  @param args Variables to print
     *  @spec ejs
     */
    native function printv(...args): void

    /**
        Parse a string and convert to a primitive type
        @param str String to parse
        @param preferredType Preferred type to use if the input can be parsed multiple ways.
     */
    native function parse(str: String, preferredType: Type = null): Object

    /**
     *  Encode an object as a string. This function returns a literal string for the object and all its properties. 
     *  If @maxDepth is sufficiently large (or zero for infinite depth), each property will be processed recursively 
     *  until all properties are rendered.  NOTE: the maxDepth, all and base properties are not yet supported.
     *  @param obj Object to serialize
     *  @param maxDepth The depth to recurse when converting properties to literals. If set to zero, the depth is infinite.
     *  @param all Encode non-enumerable and class fixture properties and functions.
     *  @param base Encode base class properties.
     *  @return This function returns an object literal that can be used to reinstantiate an object.
     *  @throws TypeError If the object could not be converted to a string.
     *  @spec ejs
     */ 
    native function serialize(obj: Object, maxDepth: Number = 0, all: Boolean = false, base: Boolean = false): String

    /** @hide  temp only */
    function printHash(name: String, o: Object): Void
        print("%20s %X" % [name, hashcode(o)])

    /** @hide  */
    function instanceOf(obj: Object, target: Object): Boolean
        obj is target
}

/*
    Added to ease forward compatibility
 */
module ejs.unix {
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
