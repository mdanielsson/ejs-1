class Acl extends Object {

	var _owner : Group
	var _name : String
	var _aces : Array
	
	public function Acl(caller : Group, name : String)
	{
		_owner = caller
		_name = name 
		_aces = new Array()
	}
	
	public function add(caller : Object, newAce : Ace) : void
	{
		if(caller != _owner) {
			print("*** Error: someone other then the owner attempted to add an Ace")
			return // Should throw an exception
		}
		_aces.push(newAce)
	}
	
	public function check(accessor : Object, permissions : Permission) : Boolean
	{
		var ret = false
		for(var i in _aces) {
			var val = _aces[i].check(accessor, permissions)
			if(val == -1) {
				return false
			}
			if(val == 1)
			{
				ret = true
			}
		}
		return ret
	}

	public function getPermissions(group : Group) : Array
	{
		var ret = new Array()
		
		for(var i in _aces)
		{
			if(_aces[i].isGroup(group))
			{
				ret.push(_aces[i].permissions())
			}
		}
		return ret
	}

	public function remove(caller : Object, ace : Ace) : void
	{ 
		if(caller != _owner) {
			print("*** Error: someone other then the owner attempted to remove an Ace")
			return // Should throw an exception
		}
		_aces.pop()
	}

	public function toString() : String
	{
		var ret = "\n\nAcl, Name: " + _name + " Owner: " + _owner.toString() 
		for(var i in _aces) {
			ret += _aces[i].toString()
		}
		return ret
	}
}

class Group extends Object {

	var _members : Array
	
	public function add(newMember : Object) : void
	{
		if(newMember == null) 
		{
			return
		}
		
		_members.push(newMember)
	}

	public function Group(initialMember : Object)
	{
		_members = new Array()
		if(initialMember != null) 
		{
			_members.push(initialMember)
		}
	}

	public function contains(obj : Object) : Boolean
	{
		for(var i in _members) 
		{
			if(_members[i] == obj)
			{
				return true
			}
		}

		return false
	}

	public function remove(removeMember : Object) : void
	{ 
		_members.pop()
	}
	
	public function toString() : String
	{
		var ret = "\n   Group: "
		for(var i in _members) 
		{
			ret += _members[i].toString() + "  "
		}
		return ret
	}
}

class Ace extends Object {

	var _group : Group
	var _permissions : Permission
	var _negative : Boolean
	
	public function Ace(group : Group, permissions : Permission, negative :
		Boolean)
	{
		_group = group;
		_permissions = permissions
		_negative = negative 
	}
	
	// Return value of 0 means no match, -1 means no permission, +1 means has permission.
	public function check(accessor : Object, permissions : Permission) : int
	{
		if(_group.contains(accessor) == false) {
			return 0
		}
		if(_permissions.has(permissions, _negative) == true) {
			return 1
		}

		return -1
	}

	public function isGroup(group : Group) : Boolean
	{
		if(_group == group) {
			return true
		}
		return false
	}
	
	public function get permissions() : Permission
	{
		return _permissions
	}
	
	public function replacePermissions(permissions : Permission) : void
	{
		_permissions = permissions
	}

	public function setNegative() : void
	{
		_negative = true
	}

	public function toString() : String
	{
		var ret = "\nAce : group: " + _group.toString()
		ret += _permissions.toString()
		return ret
	}
}

class Permission extends Object {

	public static const READ : int			= 1
	public static const WRITE : int			= 2
	public static const OTHER : int			= 4
	public static const OWNER : int			= 8
	public static const EXECUTE : int		= 16
	
	var _name : String
	var _permissions : int
	
	public function Permission(name : String, permissions : int)
	{ 
		_name = name;
		_permissions = permissions
	}

	public function has(perm : Permission, negative : Boolean) : Boolean
	{
		if(negative == false)
		{
			if((perm._permissions & _permissions) == perm._permissions)
			{
				return true
			}
		} else {			
			if((perm._permissions & ~_permissions) == perm._permissions)
			{
				return true
			}
		}
		return false
	}
	
	public function setPermissions(permissions : int) : void
	{
		_permissions = permissions
	}

	public function toString() : String
	{
		var ret = "\n     Permission, Name: " + _name + " settings: " + _permissions.toString()
		return ret
	}
}

/*
 *	API tests
 */
 
print("\nCreate a group")
print("Group g1 = new Group(\"Bob Smith\")")
var g1 = new Group("Bob Smith")

print("\nCreate an Acl with that group as the owner")
print("var Acl = new Acl(g1, \"new Acl\")")
var acl = new Acl(g1, "new Acl")

print("\nCreate a Permission")
print("var perm = new Permission(\"First_permission\", Permission.READ | Permission.WRITE)")
var perm = new Permission("First_permission", Permission.READ | Permission.WRITE)

print("\nCreate an Ace with Bob Smith as the owner and those permissions")
print("var ace = new Ace(g1, perm, false)")
var ace = new Ace(g1, perm, false)

print("\nAdd the ace to the acl")
print("acl.add(g1, ace)")
acl.add(g1, ace)
print(acl.toString())

print("\nAdd Jimmy Jones to the group")
print("g1.add(\"Jimmy Jones\")")
g1.add("Jimmy Jones")
print(acl.toString())

print("\nCreate another group")
print("Group g1 = new Group(\"Linda Brown\")")
var g2 = new Group("Linda Brown")

print("\nCreate an Ace with Linda Brown as the owner and add it to the Acl")
print("var ace2 = new Ace(g2, perm, false)")
var ace2 = new Ace(g2, perm, false)
print("acl.add(g2, ace2)")
acl.add(g2, ace2)
print(acl.toString())

print("\nOwner adds the Ace")
acl.add(g1, ace2)
print(acl.toString())

print("\nCheck if Linda has execute permission")
var perm2 = new Permission("", Permission.EXECUTE)
print("acl.check(\"Linda Brown\", perm2)")
print("Permission? ", acl.check("Linda Brown", perm2))

print("\nCheck if Linda has read permission")
var perm3 = new Permission("", Permission.READ)
print("acl.check(\"Linda Brown\", perm3)")
print("Permission? ", acl.check("Linda Brown", perm3))

print("\nMake Linda's Ace negative")
print("ace2.setNegative()")
ace2.setNegative()
print("acl.check(\"Linda Brown\", perm3)")
print("Permission? ", acl.check("Linda Brown", perm3))

print("\nRemove Linda from this group")
print("g2.remove(\"Linda Brown\")")
g2.remove("Linda Brown")
print("acl.check(\"Linda Brown\", perm3)")
print("Permission? ", acl.check("Linda Brown", perm3))

print("\nRemove the Linda Brown Ace")
print("acl.remove(g1, ace2)")
acl.remove(g1, ace2)
print("acl toString: ", acl.toString())

print("\nAdd a permission, then get and show all the permissions in this Acl")
var ace2 = new Ace(g1, perm2, false)
acl.add(g1, ace2)

print("var perms = acl.getPermissions(g1)")
var perms = acl.getPermissions(g1)
for(var i in perms)
{
	print(perms[i].toString())
}

print("\nChange the permissions on the Aces")
print("perm.setPermissions(Permission.READ)")
perm.setPermissions(Permission.READ)

print("ace2.replacePermissions(new Permission(\"New_permission\", Permission.OTHER))")
ace2.replacePermissions(new Permission("New_permission", Permission.OTHER))
print("var perms = acl.getPermissions(g1)")
var perms = acl.getPermissions(g1)
for(var i in perms)
{
	print(perms[i].toString())
}


print("\n*** Tests completed ***")
