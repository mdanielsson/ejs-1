class FileSystem extends Object {

	// Values to simulate a file and file system
	static var _basename = "test.txt"
	static var _dirName = "C:\\temp"
	static var _freeSpace = 1170238876
	static var _fileContents = "Contents of a dummy file"
	
	public static function access(path: String): Boolean
	{
		return true
	}

	public static function baseName(path: String): String
	{
		return _basename
	}

	public static function copy(fromPath: String, toPath: String): Boolean
	{
		return true
	}

	public static function dirList(path: String, enumDirs: Boolean): 
		Array 
	{
		var arr = new Array()
		arr.push("text.txt")
		arr.push("textCopy.txt")
		return arr
	}

	public static function dirName(path: String): String
	{
		return _dirName
	}

	public static function get freeSpace(): long
	{
		return _freeSpace
	}

	public static function getFileInfo(path: String): File
	{
		return null
	}

	public static function mkdir(path: String): void
	{
	}

	public static function readFile(path: String): String
	{
		return _fileContents
	}

	public static function remove(path: String): void
	{
	}

	public static function rename(from: String, to: String): void
	{
	}

	public static function rmdir(path: String): void
	{
	}

	public static function writeFile(path: String, data: String): void
	{
	}


}

public class File {
}

/*
 *	API tests
 */

print("\nTest access to a file, FileSystem.access(\"c:\\test.txt\") returns: ", 
	FileSystem.access("c:\\test.txt").toString())

print("\nBase name for a file, FileSystem.baseName(\"C:\\temp\\test.txt\") returns: ", 
	FileSystem.baseName("C:\\temp\\test.txt"))

print("\nDirectory name for a file, FileSystem.dirName(\"C:\\temp\\test.txt\") returns: ", 
	FileSystem.dirName("C:\\temp\\test.txt"))

print("\nCopy a file, FileSystem.copy(\"C:\\test.txt\", \"C:\\testCopy.txt\") returns: ", 
	FileSystem.copy("C:\\test.txt", "C:\\testCopy.txt"))

print("\nGet a directory contents, FileSystem.dirList(\"C:\", false) returns: ")
var arr = FileSystem.dirList("C:", false)
for(var i = 0; i < arr.length; i++)
{
	print("Dir contains: ", arr[i])
}

print("\nGet the amount of free space, FileSystem.freeSpace returns: ", 
	FileSystem.freeSpace)

print("\nGet file info, FileSystem.getFileInfo(\"C:\\test3.txt\") returns: ", 
	FileSystem.getFileInfo("C:\\test3.txt"))

print("\nMake a directory, FileSystem.mkdir(\"C:\\temp3\") returns: ", FileSystem.mkdir("C:\\temp3"))

print("\nRead a file, FileSystem.readFile(\"C:\\temp3\") returns: ", FileSystem.readFile("C:\\temp3"))

print("\nRemove a file, FileSystem.remove(\"C:\\temp3\") returns: ", FileSystem.remove("C:\\temp3"))

print("\nRename a file, FileSystem.rename(\"C:\\temp\\test3.txt\", \"C:\\temp\\test4.txt\") returns: ",
FileSystem.rename("C:\\temp\\test3.txt", "C:\\temp\\test4.txt"))

print("\nRemove a directory, FileSystem.rmdir(\"C:\\temp3\") returns: ", FileSystem.rmdir("C:\\temp3"))

print("\nWrite a file, FileSystem.writeFile(\"C:\\temp\\test4.txt\", \"Test contents to write to a file\")",
" returns: ", FileSystem.writeFile("C:\\temp\\test4.txt", "Test contents to write to a file"))


print("\n*** Tests completed ***")
