class Thread extends Object {

	var _name : String = ""
	var _pri : int = 0
	var _hasLock : Boolean = false
	var _isInterrupted : Boolean = false
	var _isStopped : Boolean = false
	var _hasStarted : Boolean = false
	var _hasExited = false
	static var threads = new Array()
		
	static public function access(thread : Thread) : Boolean
	{
		if(thread._hasExited) {
			throw "Thread has exited"
		}
		return true
	}

	static public function dump(thread : Thread) : String
	{
		print("This is dummy stack trace:1")
		print("This is dummy stack trace:2")
		print("This is dummy stack trace:3")
	}
	
	public function exit() : void
	{
		_hasExited = true
	}
	
	public function get name(): String 
	{
		return _name
	}
	
	public function get priority(): int 
	{
		return _pri
	}
	
	static public function getThreads() : Array
	{
		return threads
	}
	
	public function hasLock() : Boolean
	{
		return _hasLock
	}
	
	public function interrupt() : void
	{
		this._isInterrupted = true
	}
	
	static public function isInterrupted(thread : Thread) : Boolean
	{
		return thread._isInterrupted
	}
	
	static public function isRunning(thread : Thread) : Boolean
	{
		return !(thread._isInterrupted || thread._isStopped || thread._hasExited) 
			&& thread._hasStarted
	}
	
	public function join(thread : Thread, timeout : int) : void
	{
		this._isStopped = true
		thread._isStopped = false
	}
	
	public function pause() : void
	{
		this._isStopped = true
	}
	
	public function setName(name : String) : void
	{
		this._name = name
	}

	public function setPriority(pri : int) : void
	{
		this._pri = pri
	}

	public function sleep() : void
	{
	}

	public function start() : void
	{
		this._hasStarted = true
		this._isInterrupted = false
	}

	public function Thread(name : String, priority : int)
	{
		this._name = name
		this._pri = priority
		this._hasLock = false
		this._isInterrupted = false
		this._isStopped = false
		this._hasStarted = false
		this._hasExited = false
		threads.push(this)
	}

}

/*
 *	API tests
 */
print("var thread = new Thread(\"Test\", 2)")
var thread = new Thread("Test", 2)
print("Name = ", thread.name)
print("Priority = ", thread.priority)

print("\nthread.setName(\"Bob\")")
thread.setName("Bob")
print("Name = ", thread.name)
print("\nthread.setPriority(11)")
thread.setPriority(11)
print("Priority = ", thread.priority)

print("\nthread.start()")
thread.start()
print("\nThread.access(thread): ", Thread.access(thread))
print("\nThread.dump(thread): \n", Thread.dump(thread))

print("\nUncomment the following exit() method call to test it. An exception will be thrown.")
print("thread.exit() not called")
// thread.exit()

print("\nThread.access(thread): ", Thread.access(thread))

var threads = Thread.getThreads()
print("\nThread.getThreads()")
for(var t in threads) {
	print("   -> threads[t].name: ", threads[t].name)
}

print("\nthread.hasLock(): ", thread.hasLock())

print("\nThread.isInterrupted(thread): ", Thread.isInterrupted(thread))
print("thread.interrupt()")
thread.interrupt()
print("Thread.isInterrupted(thread): ", Thread.isInterrupted(thread))

thread.start()
print("\nvar thread2 = new Thread(\"Test2\", 2)")
var thread2 = new Thread("Test2", 2)
thread2.start()
print("Thread.isRunning(thread): ", Thread.isRunning(thread))
print("Thread.isRunning(thread2): ", Thread.isRunning(thread2))
print("thread.join(thread2)")
thread.join(thread2)
print("Thread.isRunning(thread): ", Thread.isRunning(thread))
print("Thread.isRunning(thread2): ", Thread.isRunning(thread2))

print("\nthread2.pause()")
thread2.pause()
print("Thread.isRunning(thread2): ", Thread.isRunning(thread2))

print("\nthread.sleep(): ", "does nothing")
thread.sleep()

print("\n*** Tests completed ***")
