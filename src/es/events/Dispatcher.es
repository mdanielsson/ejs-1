/*
    Dispatcher.es -- Event dispatcher target.
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.events {

    /**
        The Dispatcher class supports the registration of listeners who want notification of events of interest.
        WARNING: This class is prototype and it not currently supported.

        @example
            class Shape {
                public var events: Dispatcher = new Dispatcher
            }
            function callback(e: Event) {
                //  Do something
            }
            var s : Shape = new Shape
            s.events.addListener(callback)
            s.events.dispatch(new Event("Great Event"))

            // Main Program then calls
            App.serviceEvents()
        @hide
     */
    class Dispatcher {
        var endpoints: Object

        use default namespace public

        /**
            Construct a new event Dispatcher object
         */
        function Dispatcher()
            endpoints = new Object

        /**
            Add a listener function for events.
            @param callback Function to call when the event is received.
            @param eventType Event class to listen for. The listener will receive events of this event class or any 
                of its subclasses. Defaults to Event.
         */
        function addListener(callback: Function, eventType: Type = Event): Void {
            var name = Reflect(eventType).name
            var listeners : Array

            listeners = endpoints[name]
            if (listeners == undefined) {
                listeners = endpoints[name] = new Array
            } else {
                for each (var e: Endpoint in listeners) {
                    if (e.callback == callback && e.eventType == eventType) {
                        return
                    }
                }
            }
            listeners.append(new Endpoint(callback, eventType))
        }

        /**
            Dispatch an event to the registered listeners. This is called by the class owning the event dispatcher.
            @param event Event instance to send to the listeners.
         */
        function dispatch(event: Event): Void {
            var listeners : Array
            var name = Reflect(event).typeName

            listeners = endpoints[name]
            if (listeners != undefined) {
                for each (var e: Endpoint in listeners) {
                    if (event is e.eventType) {
                        e.callback(event)
                    }
                }
            }
        }

        /**
            Remove a registered listener.
            @param eventType Event class used when adding the listener.
            @param callback Listener callback function used when adding the listener.
         */
        function removeListener(callback: Function, eventType: Type = Event): Void {
            var name = Reflect(eventType).name
            var listeners: Array

            listeners = endpoints[name]
            for (let i in listeners) {
                var e: Endpoint = listeners[i]
                if (e.callback == callback && e.eventType == eventType) {
                    listeners.remove(i, i + 1)
                }
            }
        }
    }


    /*
        Listening endpoints
        @hide
     */
    internal class Endpoint {
        public var callback: Function
        public var eventType: Type
        function Endpoint(callback: Function, eventType: Type) {
            this.callback = callback
            this.eventType = eventType
        }
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
