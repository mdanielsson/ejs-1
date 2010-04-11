/*
 *	Thread.es -- Thread class
 *
 *	Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs {

	/**
	 *	A Thread represents a single path of code execution that can run in parallel with other Thread objects. 
	 *	Threads are pre-emptive. They can be created, run, interrupted, paused (sleep), prioritized and destroyed. 
	 *	Various helper methods provide information about threads such as their number, name, priority, state, etc.
	 */
	# FUTURE
	intrinsic native class Thread extends Object {

		/**
		 *	Test if a thread can have its state (e.g. made interrupted) changed by code in this thread.
		 *	@param target The thread to test.
		 *	@return True if this thread can modify the state of the target thread.
		 */
		intrinsic native static function access(target : Thread): boolean 


		/**
		 *	Put all the stack trace from a thread into a string.
		 *	@param target The thread to dump.
		 *	@return A string containing the stack trace
		 */
		intrinsic native static function dump(target : Thread): String


		/**
		 *	Exit and destroy the currently running thread. If the thread has already exited this method has no effect.
		 */
		intrinsic native function exit(): void


		/**
		 *	Obtain a reference to the currently running thread.
		 *	@return A thread reference
		 */
		intrinsic native static function getCurrent(): Thread 


		/**
		 *	Get the name of this thread.
		 *	@return A string with the name
		 */
		intrinsic native function get name(): String 


		/**
		 *	Get the priority of this thread.
		 *	@return The integer priority
		 */
		intrinsic native function get priority(): int 


		/**
		 *	Get references to all of the threads that are either running or interrupted or paused.
		 *	@return An unsorted array of thread references
		 */
		intrinsic native static function getThreads(): Array 


		/**
		 *	Determine if the currently running thread has a lock on an object.
		 *	@param target The object to test.
		 *	@return True if the thread has a lock
		 */
		intrinsic native function hasLock(target : Object): boolean


		/**
		 *	Interrupts the currently running thread. If the thread is already
		 *	interrupted this method has no effect.
		 *	@throws ThreadError If the target thread has already exited
		 */
		intrinsic native function interrupt(): void


		/**
		 *	Tests a thread to see if it is interrupted.
		 *	@param target The thread to test.
		 *	@return True if the thread is interrupted
		 */
		intrinsic native static function isInterrupted(target : Thread): boolean


		/**
		 *	Tests a thread to see if it is running.
		 *	@param target The thread to test.
		 *	@return True if the thread is running
		 */
		intrinsic native static function isRunning(target : Thread): boolean


		/**
		 *	Stops the currently running thread and starts execution on the argument thread.
		 *	@param target The thread to join
		 *	@param timeout The number of milliseconds to run the thread, -1 means run forever.
		 *	@throws ThreadError If the target thread is interrupted or has exited
		 */
		intrinsic native function join(target : Thread, timeout : int): void


		/**
		 *	Starts a thread running.
		 *	@throws ThreadError If the target thread is has exited
		 */
		intrinsic native function start(): void


		/**
		 *	Sets the name of the thread.
		 *	@param name New name for the thread
		 */
		intrinsic native function setName(name : String): void


		/**
		 *	Sets the priority of the thread.
		 *	@param priority Integer priority for the thread, higher numbers are higher priorities.
		 *	@throws ArgError If the priority is outside the legal limits.
		 */
		intrinsic native function setPriority(priority : int): void


		/**
		 *	Cause the currently executing thread sleeps for some milliseconds.
		 *	@param ms Number of milliseconds to sleep
		 *	@throws ThreadError If the thread is interrupted while it is sleeping.
		 */
		intrinsic native function sleep(ms : int): void


		/**
		 *	Thread constructor, the name and priority can be (optionally) set. The name defaults to an empty 
		 *	string; the priority defaults to 0.
		 *	@param name The (optional) name for this thread.
		 *	@param priority The (optional) priority.
		 */
		intrinsic native function Thread(name : String = "", priority : int = 0)


		/**
		 *	Pauses the currently executing thread and allows other threads to run.
		 *	@throws ThreadError If the target thread is interrupted or has exited
		 */
		intrinsic native function pause(): void
	}
}


/*
 *	@copy	default
 *	
 *	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
 *	Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
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
 *	@end
 */
