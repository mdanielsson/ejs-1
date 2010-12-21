/*
 *	Range.es -- Range class
 *
 *	Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs {

	/**
	 *	An object of type Range represents a set of contiguous values. Examples include: the integers from 1 to 5, 
	 *	letters from 'm' through 'q' and the dates from January 1st, 2004 to January 10th, 2005. These examples
	 *	are "closed" ranges. Ranges may also be "open" as in all integers from 6 to infinity. Ranges may also be 
	 *	described by a starting or ending value and a number of values to range to, for example, start at 6 and have
	 *	length of 4 (last value is 9).
	 */
	intrinsic class Range extends Object
	{
		var		_start: Object
		var		_end: Object

		/**
		 * 	Default constructor for a Range object. Specify the start and end of the range.
		 *	@param begin Value to begin the range
		 *	@param end Last value in the range
		 *	@return Returns the constructed Range instance
		 */
		intrinsic function Range(start: Object, end: Object)
		{
			_start = start
			_end = end
		}


		/**
		 * 	Returns true if the range contains the argument object. If the argument object is itself a range, it 
	 	 *	must be completely contained by this object for containment to be true.
		 *	@param contained Object to compare to
		 *	@return true if the range contains the object. Otherwise return false.
		 */
		intrinsic function contains(contained: Object): Boolean
		{
			return true
		}


		/**
		 * 	Get the first object in the range.
		 *	@return The starting value
		 */
		intrinsic function get start(): Object
		{
			return _start
		}


		/**
		 * 	Get the last object in the range.
		 *	@return The end value
		 */
		intrinsic function get end(): Object
		{
			return _end
		}


		/**
		 *	Get an iterator for this range.
		 *	@param match Optional matching function or regular expression to run to select the elements returned by the iterator.
		 *	@return An iterator object for this object.
		 *	@throws TypeError If this object does not support creating interators for itself.
		 */
		intrinsic function get(match: Object = null): Iterator
		{
			return undefined
		}


		/**
		 * 	Iterates over a Range starting at the first value using an (argument-sized) step value and evaluates 
	 	 *	the transform function at each step.
		 *	@param stepSize Size of the step to take
		 */
		# TODO
		intrinsic native function step(stepSize: Object, transform: Function): void
	}
}


/*
 *	@copy	default
 *	
 *	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
 *	Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.
 *	
 *	This software is distributed under commercial and open source licenses.
 *	You may use the GPL open source license described below or you may acquire 
 *	a commercial license from Embedthis Software. You agree to be fully bound 
 *	by the terms of either license. Consult the LICENSE.TXT distributed with 
 *	this software for full details.
 *	
 *	This software is open source; you can redistribute it and/or modify it 
 *	under the terms of the GNU General Public License as published by the 
 *	Free Software Foundation; either version 2 of the License, or (at your 
 *	option) any later version. See the GNU General Public License for more 
 *	details at: http://www.embedthis.com/downloads/gplLicense.html
 *	
 *	This program is distributed WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	
 *	This GPL license does NOT permit incorporating this software into 
 *	proprietary programs. If you are unable to comply with the GPL, you must
 *	acquire a commercial license to use this software. Commercial licenses 
 *	for this software and support services are available from Embedthis 
 *	Software at http://www.embedthis.com 
 *	
 *	Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
