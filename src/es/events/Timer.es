/*
 *  Timer.es -- Timer Services
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.events {

    /**
     *  WARNING: This class is prototype and will be changed in the next release
     *  Timers manage the execution of functions at some point in the future. Timers run repeatedly until stopped by 
     *  calling the stop() method. Timers are scheduled with a granularity of 1 millisecond. However, many systems are not 
     *  capable of supporting this granularity and make only best efforts to schedule events at the desired time.
     *  To use timers, the application must call Dispatcher.serviceEvents.
     *  @Example
     *      function callback(e: Event) {
     *      }
     *      new Timer(200, callback)
     *  or
     *      new Timer(200, function (e) { print(e); })
     *  @hide
     */
    native class Timer {

        use default namespace public

        /**
         *  Constructor for Timer. The timer is will not be called until @start is called.
         *  @param callback Function to invoke when the timer is due.
         *  @param period Time period in milliseconds between invocations of the callback
         *  @param drift Set the timers drift setting. See @drift.
         */
        native function Timer(period: Number, callback: Function, drift: Boolean = true)

        /**
         *  The timer drift setting.
         *  If drift is false, reschedule the timer so that the time period between callback start times does not drift 
         *  and is best-efforts equal to the timer reschedule period. The timer subsystem will delay other low priority
         *  events or timers, with drift equal to true, if necessary to ensure non-drifting timers are scheduled exactly. 
         *  Setting drift to true will schedule the timer so that the time between the end of the callback and the 
         *  start of the next callback invocation is equal to the period. 
         */
        native function get drift(): Boolean

        /**
         *  @duplicate Timer.drift
         *  @param enable If true, allow the timer to drift
         */
        native function set drift(enable: Boolean): Void

        /**
         *  The timer interval period in milliseconds.
         */
        native function get period(): Number

        /**
         *  Set the timer period and reschedule the timer.
         *  @param period New time in milliseconds between timer invocations.
         */
        native function set period(period: Number): Void

        /**
         *  Restart a stopped timer. Once running, the callback function will be invoked every @period milliseconds 
         *  according to the @drift setting. If the timer is already stopped, this function has no effect
         */
        native function restart(): Void

        /**
         *  Stop a timer running. Once stopped a timer can be restarted by calling @start.
         */
        native function stop(): Void
    }


    /**
     *  WARNING: This class is prototype and will be changed in the next release
     *  Timer event
     *  @hide
     */
    class TimerEvent extends Event { }
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
