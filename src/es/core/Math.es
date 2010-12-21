/*
    Math.es -- Math class 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs {

    use default namespace intrinsic

    /**
        The Math class provides a set of static methods for performing common arithmetic, exponential and 
        trigonometric functions. It also provides commonly used constants such as PI. See also the Number class.
        Depending on the method and the supplied argument, return values may be real numbers, NaN (not a number) 
        or positive or negative infinity.
        @stability evolving
     */
    class Math extends Object {

        use default namespace public

        /**
            Base of natural logarithms (Euler's number).
         */
        static const E: Number = 2.7182818284590452354

        /**
            Natural log of 2.
         */
        static const LN2: Number = 0.6931471805599453

        /**
            Natural log of 10.
         */
        static const LN10: Number = 2.302585092994046

        /**
            Base 2 log of e.
         */
        static const LOG2E: Number = 1.4426950408889634

        /**
            Base 10 log of e.
         */
        static const LOG10E: Number = 0.4342944819032518

        /**
            The ratio of the circumference to the diameter of a circle.
         */
        static const PI: Number = 3.1415926535897932

        /**
            Reciprocal of the square root of 2.
         */
        static const SQRT1_2: Number = 0.7071067811865476

        /**
            Square root of 2.
         */
        static const SQRT2: Number = 1.4142135623730951

        /**
            Returns the absolute value of a number (which is equal to its magnitude).
            @param value Number value to examine
            @return the absolute value.
         */
        native static function abs(value: Number): Number 

        /**
            Calculates the arc cosine of an angle (in radians).
            @param angle In radians 
            @return The arc cosine of the argument 
         */
        native static function acos(angle: Number): Number 

        /**
            Calculates the arc sine of an angle (in radians).
            @param oper The operand.
            @return The arc sine of the argument 
         */
        native static function asin(oper: Number): Number 

        /**
            Calculates the arc tangent of an angle (in radians).
            @param oper The operand.
            @return The arc tanget of the argument 
         */
        native static function atan(oper: Number): Number 

        /**
            Calculates the arc tangent of the quotient of its arguments
            @param x the x operand.
            @param y the y operand.
            @return The arc tanget of the argument 
         */
        native static function atan2(y: Number, x: Number): Number 

        /**
            Return the smallest integer greater then this number.
            @return The ceiling
         */
        native static function ceil(oper: Number): Number

        /**
            Calculates the cosine of an angle (in radians).
            @param angle In radians 
            @return The cosine of the argument 
         */
        native static function cos(angle: Number): Number 
        
        /**
            Calculate E to the power of the argument
         */
        native static function exp(power: Number): Number 

        /**
            Returns the largest integer smaller then the argument.
            @param oper The operand.
            @return The floor
         */
        native static function floor(oper: Number): Number

        /**
            Calculates the log (base 10) of a number.
            @param oper The operand.
            @return The natural log of the argument
            @return The base 10 log of the argument
            @spec ejs
         */
        native static function log10(oper: Number): Number 
        
        /**
            Calculates the natural log (ln) of a number.
            @param oper The operand.
            @return The natural log of the argument
         */
        native static function log(oper: Number): Number 

        /**
            Returns the greater of the number or the argument.
            @param x First number to compare
            @param y Second number to compare
            @return A number
         */
        native static function max(x: Number, y: Number): Number

        /**
            Returns the lessor of the number or the argument.
            @param x First number to compare
            @param y Second number to compare
            @return A number
         */
        native static function min(x: Number, y: Number): Number

        /**
            Returns a number which is equal to this number raised to the power of the argument.
            @param num The number to raise to the power
            @param pow The exponent to raise @num to
            @return A number
         */
        native static function pow(num: Number, pow: Number): Number

        /**
            Generates a random number (a Number) inclusively between 0.0 and 1.0.
            @return A random number
         */
        native static function random(): Number 

        /**
            Round this number down to the closes integral value.
            @param num Number to round
            @return A rounded number
         */
        native static function round(num: Number): Number

        /**
            Calculates the sine of an angle (in radians).
            @param angle In radians 
            @return The sine of the argument 
         */
        native static function sin(angle: Number): Number 
        
        /**
            Calculates the square root of a number.
            @param oper The operand.
            @return The square root of the argument
         */
        native static function sqrt(oper: Number): Number 

        /**
            Calculates the tangent of an angle (in radians).
            @param angle In radians 
            @return The tangent of the argument 
         */
        native static function tan(angle: Number): Number 
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
