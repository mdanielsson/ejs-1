/*
    Number.es - Number class

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs {

    use default namespace intrinsic

    /**
        The Number type is used by all numeric values in Ejscript. Depending on how Ejscript is configured, the underlying
        number representation may be based on either an int, long, int64 or double data type. If the underlying type is
        integral (not double) then some of these routines may not be relevant.
        @stability evolving
     */
    native final class Number {

        use default namespace public

        /**
            Number constructor.
            @param value Value to use in creating the Number object. If the value cannot be converted to a number, 
                the value will ba NaN (or 0 if using integer numerics).
         */
        native function Number(value: Object = null)

        /**
            Return the maximim value this number type can assume. Alias for MaxValue.
            An object of the appropriate number with its value set to the maximum value allowed.
         */
        static const MAX_VALUE: Number = MaxValue

        /**
            Return the minimum value this number type can assume. Alias for MinValue.
            An object of the appropriate number with its value set to the minimum value allowed.
         */
        static const MIN_VALUE: Number = MinValue

        /**
            Not a Number. This is the result of an arithmetic expression which has no value.
         */
        static const NaN : Number = NaN

        /**
            Return a unique value which is less than or equal then any value which the number can assume. 
            @return A number with its value set to -Infinity. If the numeric type is integral, then return zero.
         */
        static const NEGATIVE_INFINITY: Number = NegativeInfinity

        /**
            Return a unique value which is greater then any value which the number can assume. 
            @return A number with its value set to Infinity. If the numeric type is integral, then return MaxValue.
         */
        static const POSITIVE_INFINITY: Number = Infinity

        /**
            Return the maximim value this number type can assume.
            @return A number with its value set to the maximum value allowed.
            @spec ejs
         */
        native static const MaxValue: Number

        /**
            Return the minimum value this number type can assume.
            @return A number with its value set to the minimum value allowed.
            @spec ejs
         */
        native static const MinValue: Number

        /**
            The absolute value of a number (which is equal to its magnitude).
            @spec ejs
         */
        function get abs(): Number
            Math.abs(this)

        /**
            The smallest integral number that is greater or equal to the number value. 
            @spec ejs
         */
        function get ceil(): Number 
            Math.ceil(this)

        /**
            The largest integral number that is smaller than the number value.
            @spec ejs
         */
        function get floor(): Number
            Math.floor(this)

        /**
            Is the number Infinity or -Infinity. Set to true or false.
            @spec ejs
         */
        native function get isFinite(): Boolean

        /**
            Is the number is equal to the NaN value. If the numeric type is integral, this will always return false.
            @spec ejs
         */
        native function get isNaN(): Boolean

        /**
            Compute the integral number that is closest to this number. ie. round up or down to the closest integer.
            @spec ejs
         */
        function get round(): Number
            Math.round(this)

        /**
            Returns the number formatted as a string in scientific notation with one digit before the decimal point 
            and the argument number of digits after it.
            @param fractionDigits The number of digits in the fraction.
            @return A string representing the number.
         */
        native function toExponential(fractionDigits: Number = 0): String

        /**
            Returns the number formatted as a string with the specified number of digits after the decimal point.
            @param fractionDigits The number of digits in the fraction.
            @return A string representing the number 
         */
        native function toFixed(fractionDigits: Number = 0): String

        /**
            Returns the number formatted as a string in either fixed or exponential notation with argument number of digits.
            @param numDigits The number of digits in the result. If omitted, the entire number is returned.
            @return A string
         */
        native function toPrecision(numDigits: Number = MAX_VALUE): String

        /**
            Byte sized integral number. Numbers are rounded and truncated as necessary.
            @spec ejs
         */
        function get byte(): Number
            integral(8)

        /**
            Convert this number to an integral value of the specified number of bits. Floating point numbers are 
                converted to integral values using truncation.
            @size Size in bits of the value
            @return An integral number
            @spec ejs
         */
        native function integral(size: Number = 32): Number

        /**
            Return an iterator that can be used to iterate a given number of times. This is used in for/in statements.
            @return an iterator
            @example
                for (i in 5) 
                    print(i)
            @spec ejs
         */
        override iterator native function get(): Iterator

        /**
            Return an iterator that can be used to iterate a given number of times. This is used in for/each statements.
            @return an iterator
            @example
                for each (i in 5) 
                    print(i)
            @spec ejs
         */
        override iterator native function getValues(): Iterator

        /**
            Returns the greater of the number and the arguments.
            @param other Other numbers number to compare with
            @return A number
            @spec ejs
         */
        function max(...other): Number {
            let result = this
            for each (n in other) {
                n = n cast Number
                if (n > result) {
                    result = n
                }
            }
            return result
        }

        /**
            Returns the lessor of the number and the arguments.
            @param other Numbers to compare with
            @return A number
            @spec ejs
         */
        function min(...other): Number {
            let result = this
            for each (n in other) {
                n = n cast Number
                if (n < result) {
                    result = n
                }
            }
            return result
        }

        /**
            Returns a number which is equal to this number raised to the power of the argument.
            @param nth Nth power to be raised to
            @return A number
            @spec ejs
         */
        function power(nth: Number): Number
            Math.pow(this, nth)

        /**
            This function converts the number to a string representation.
            @param radix Radix to use for the conversion. Defaults to 10. Non-default radixes are currently not supported.
            @returns a string representation of the number.
         */ 
        override native function toString(radix: Number = 10): String
    }
}

/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
    
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
    vim: sw=4 ts=4 expandtab

    @end
 */
