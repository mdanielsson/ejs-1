class Logger extends Object {

	public static const INHERIT : int	= -1
	public static const OFF : int		= 0
	public static const ERROR : int		= 1
	public static const WARN : int		= 2
	public static const INFO : int		= 3
	public static const CONFIG : int	= 4
	public static const ALL : int		= 5

	public static const NONE : int			= 0
	public static const CONSOLE : int		= 1
	public static const LOG_FILE : int		= 2
	public static const EVENT_LOG : int		= 4
	public static const MEM_LOG : int		= 8

	var _name : String
	var _parent : Logger
	var _children : Array
	var _file : String
	var _console
	var _level : int
	var _sink : int
	var _filter : Function
	
	public function get level() : int
	{
		return _level
	}

	public function Logger(name : String, parent : Logger)
	{
		_name = name
		_parent = parent
	}

	public function Logger(name : String, level : int, 
		output : int, parent : Logger)
	{
		_name = name
		_level = level
		_sink = output
		_parent = parent
	}

	public function log(level : int, msg : String, obj : Object) : void
	{
		if(_parent != null)
		{
			print("Logging: ", _name, " calling parent")
			_parent.log(level, msg, obj)
		}
		
		if(level > _level) {
			return
		}
		
		if((_sink & Logger.CONSOLE) == Logger.CONSOLE) {
			print("Logging: ", _name, " : ", msg)
			if(obj != null)
			{
				print(" ", obj.toString())
			}
		}
	}

	public function logWarn(msg : String) : void
	{
		log(Logger.WARN, msg, null)
	}

	public function setLevel(level : int)
	{
		_level = level
	}
}

/*
 *	API tests
 */
print("\nCreate a new logger")
print("var logger = new Logger(\"TestLogger1\", Logger.WARN, Logger.CONSOLE, null)")
var logger = new Logger("TestLogger1", Logger.WARN, Logger.CONSOLE, null)

print("\nLog something at WARN level")
logger.log(Logger.WARN, "First test log message", null)

print("\nCreate a new (child) logger")
print("var childLogger = new Logger(\"TestLoggerChild1\", logger)")
var childLogger = new Logger("TestLoggerChild1", logger)

print("\nLog something to the child at a lower level (INFO) then the parent")
print("  - no message should be output")
childLogger.log(Logger.INFO, "INFO log message - this should not appear")

print("\nLog something to the child at a level (WARN) equal to the parent")
childLogger.logWarn("childLogger WARN test log message")

print("\nChange the reporting level of the parent")
print("logger.setLevel(Logger.ERROR)")
logger.setLevel(Logger.ERROR)
print("New level: ", logger.level)

print("\nLog a WARN to the child - no message should be output")
childLogger.log(Logger.WARN, "WARN log message - this should not appear")

print("\nLog a ERROR to the child - this message should be output")
print("  - and so should the obj.toString() output")
var obj = "Arbitrary object for logging toString()"
childLogger.log(Logger.ERROR, "ERROR log message - this should appear", obj)


print("\n*** Tests completed ***")
