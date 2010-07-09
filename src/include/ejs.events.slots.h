/*
 * ejs.events.slots.h -- Property slot definitions for the "ejs.events" module
 *
 * This file is generated by ec
 *
 * Slot definitions. Version 1.0.3.
 */

#ifndef _h_SLOTS_EjsEventsSlots
#define _h_SLOTS_EjsEventsSlots 1


/**
 * Slots for the "ejs.events" module 
 */


/**
 *   Class property slots for the "global" class 
 */

/**
 * Instance slots for "global" type 
 */

/**
 * 
 *    Local slots for methods in type global 
 */


/**
 *   Class property slots for the "Event" class 
 */
#define ES_ejs_events_Event__origin                                    6
#define ES_ejs_events_Event_Event                                      6
#define ES_ejs_events_Event__initializer__Event_initializer            7
#define ES_ejs_events_Event_PRI_LOW                                    8
#define ES_ejs_events_Event_PRI_NORMAL                                 9
#define ES_ejs_events_Event_PRI_HIGH                                   10
#define ES_ejs_events_Event_NUM_CLASS_PROP                             11

/**
 * Instance slots for "Event" type 
 */
#define ES_ejs_events_Event_bubbles                                    0
#define ES_ejs_events_Event_data                                       1
#define ES_ejs_events_Event_timestamp                                  2
#define ES_ejs_events_Event_priority                                   3
#define ES_ejs_events_Event_NUM_INSTANCE_PROP                          4

/**
 * 
 *    Local slots for methods in type Event 
 */
#define ES_ejs_events_Event_Event_data                                 0
#define ES_ejs_events_Event_Event_bubbles                              1
#define ES_ejs_events_Event_Event_priority                             2


/**
 *   Class property slots for the "ErrorEvent" class 
 */
#define ES_ejs_events_ErrorEvent__origin                               11
#define ES_ejs_events_ErrorEvent_ErrorEvent                            11
#define ES_ejs_events_ErrorEvent__initializer__ErrorEvent_initializer  12
#define ES_ejs_events_ErrorEvent_NUM_CLASS_PROP                        13

/**
 * Instance slots for "ErrorEvent" type 
 */
#define ES_ejs_events_ErrorEvent_message                               4
#define ES_ejs_events_ErrorEvent_filename                              5
#define ES_ejs_events_ErrorEvent_lineno                                6
#define ES_ejs_events_ErrorEvent_stack                                 7
#define ES_ejs_events_ErrorEvent_NUM_INSTANCE_PROP                     8


/**
 *   Class property slots for the "Dispatcher" class 
 */
#define ES_ejs_events_Dispatcher__origin                               6
#define ES_ejs_events_Dispatcher_Dispatcher                            6
#define ES_ejs_events_Dispatcher_addListener                           7
#define ES_ejs_events_Dispatcher_dispatch                              8
#define ES_ejs_events_Dispatcher_removeListener                        9
#define ES_ejs_events_Dispatcher_NUM_CLASS_PROP                        10

/**
 * Instance slots for "Dispatcher" type 
 */
#define ES_ejs_events_Dispatcher_endpoints                             0
#define ES_ejs_events_Dispatcher_NUM_INSTANCE_PROP                     1

/**
 * 
 *    Local slots for methods in type Dispatcher 
 */
#define ES_ejs_events_Dispatcher_addListener_callback                  0
#define ES_ejs_events_Dispatcher_addListener_eventType                 1
#define ES_ejs_events_Dispatcher_addListener_name                      2
#define ES_ejs_events_Dispatcher_addListener_listeners                 3
#define ES_ejs_events_Dispatcher_addListener_e                         4
#define ES_ejs_events_Dispatcher_dispatch_event                        0
#define ES_ejs_events_Dispatcher_dispatch_listeners                    1
#define ES_ejs_events_Dispatcher_dispatch_name                         2
#define ES_ejs_events_Dispatcher_dispatch_e                            3
#define ES_ejs_events_Dispatcher_removeListener_callback               0
#define ES_ejs_events_Dispatcher_removeListener_eventType              1
#define ES_ejs_events_Dispatcher_removeListener_name                   2
#define ES_ejs_events_Dispatcher_removeListener_listeners              3
#define ES_ejs_events_Dispatcher_removeListener_e                      4
#define ES_ejs_events_Dispatcher_removeListener__hoisted_5_i           5


/**
 *   Class property slots for the "Endpoint" class 
 */
#define ES_Endpoint__origin                                            6
#define ES_Endpoint_Endpoint                                           6
#define ES_Endpoint_NUM_CLASS_PROP                                     7

/**
 * Instance slots for "Endpoint" type 
 */
#define ES_Endpoint_callback                                           0
#define ES_Endpoint_eventType                                          1
#define ES_Endpoint_NUM_INSTANCE_PROP                                  2

/**
 * 
 *    Local slots for methods in type Endpoint 
 */
#define ES_Endpoint_Endpoint_callback                                  0
#define ES_Endpoint_Endpoint_eventType                                 1


/**
 *   Class property slots for the "Timer" class 
 */
#define ES_ejs_events_Timer__origin                                    6
#define ES_ejs_events_Timer_Timer                                      6
#define ES_ejs_events_Timer_drift                                      7
#define ES_ejs_events_Timer_set_drift                                  8
#define ES_ejs_events_Timer_period                                     9
#define ES_ejs_events_Timer_set_period                                 10
#define ES_ejs_events_Timer_restart                                    11
#define ES_ejs_events_Timer_stop                                       12
#define ES_ejs_events_Timer_NUM_CLASS_PROP                             13

/**
 * Instance slots for "Timer" type 
 */
#define ES_ejs_events_Timer_NUM_INSTANCE_PROP                          0

/**
 * 
 *    Local slots for methods in type Timer 
 */
#define ES_ejs_events_Timer_Timer_period                               0
#define ES_ejs_events_Timer_Timer_callback                             1
#define ES_ejs_events_Timer_Timer_drift                                2
#define ES_ejs_events_Timer_set_drift_enable                           0
#define ES_ejs_events_Timer_set_period_period                          0


/**
 *   Class property slots for the "TimerEvent" class 
 */
#define ES_ejs_events_TimerEvent__origin                               11
#define ES_ejs_events_TimerEvent_TimerEvent                            11
#define ES_ejs_events_TimerEvent__initializer__TimerEvent_initializer  12
#define ES_ejs_events_TimerEvent_NUM_CLASS_PROP                        13

/**
 * Instance slots for "TimerEvent" type 
 */
#define ES_ejs_events_TimerEvent_NUM_INSTANCE_PROP                     4

#define _ES_CHECKSUM_ejs_events 54233

#endif
