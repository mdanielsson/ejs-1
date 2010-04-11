/*
 *	master.es -- WEB Master interpreter types, functions and variables
 */

module esp.master {

	use default namespace "ejs.esp"

	var application: Object = new Object
	var session: Object = new Object

	var cookies: Object = new Object
	var files: Object = new Object
	var form: Object = new Object

//	TEMP
	var request: Object = new Object
	var headers: Object = new Object
	var server: Object = new Object
/*
	var request: Request = new Request
	var headers: Header = new Header
	var server: Server = new Server
*/


/*
	dynamic class Header {
		var HTTP_ACCEPT: String
		var HTTP_ACCEPT_CHARSET: String
		var HTTP_ACCEPT_ENCODING: String
		var HTTP_CONNECTION: String
		var HTTP_HOST: String
		var HTTP_PRAGMA: String
		var HTTP_REFERER: String
		var HTTP_USER_AGENT: String
	}


	//	THIS CLASHES WITH NEW REQUEST
	class Request {
		var AUTH_ACL: String
		var AUTH_GROUP: String
		var AUTH_TYPE: String
		var AUTH_USER: String
		var CONTENT_LENGTH: String
		var CONTENT_TYPE: String
		var QUERY_STRING: String
		var PATH_INFO: String
		var PATH_TRANSLATED: String
		var REMOTE_ADDR: String
		var REMOTE_HOST: String
		var REMOTE_PORT: Number
		var REMOTE_USER: String
		var REQUEST_METHOD: String
		var REQUEST_URI: String
		var SCRIPT_FILENAME: String
		var SCRIPT_NAME: String
	}


	class Server {
		var DOCUMENT_ROOT: String
		var GATEWAY_INTERFACE: String
		var SERVER_ADDR: String
		var SERVER_PORT: Number
		var SERVER_NAME: String
		var SERVER_PROTOCOL: String
		var SERVER_SOFTWARE: String
	}
*/


	native function destroySession(): Void


	native function createSession(timeout: Number = 0): Void


	native function redirect(url: String, code: Number = 302): Void


	native function setHeader(key: String, value: String, allowMultiple: Boolean = false): Void


	native function write(...args): Void


	function useSession(timeout: Number = 0): Void {
		createSession(timeout)
	}


	function defineLegacyVars() {

		use default namespace "public"
		headers.HTTP_ACCEPT = HTTP_ACCEPT
		headers.HTTP_ACCEPT_CHARSET = HTTP_ACCEPT_CHARSET
		headers.HTTP_ACCEPT_ENCODING = HTTP_ACCEPT_ENCODING
		headers.HTTP_CONNECTION = HTTP_CONNECTION
		headers.HTTP_HOST = HTTP_HOST
		headers.HTTP_PRAGMA = HTTP_PRAGMA
		headers.HTTP_REFERER = HTTP_REFERER
		headers.HTTP_USER_AGENT = HTTP_USER_AGENT

		request.AUTH_TYPE = AUTH_TYPE
		request.CONTENT_LENGTH = CONTENT_LENGTH
		request.CONTENT_TYPE = CONTENT_TYPE
		request.QUERY_STRING = QUERY_STRING
		request.PATH_INFO = PATH_INFO
		request.REMOTE_ADDR = REMOTE_ADDR
		request.REMOTE_HOST = REMOTE_HOST
		request.REMOTE_USER = REMOTE_USER
		request.REQUEST_METHOD = REQUEST_METHOD
		request.REQUEST_URI = REQUEST_URI
		request.SCRIPT_FILENAME = SCRIPT_FILENAME
		request.SCRIPT_NAME = SCRIPT_NAME
		request.REMOTE_PORT = REMOTE_PORT
		request.AUTH_USER = AUTH_USER
		request.AUTH_GROUP = AUTH_GROUP
		request.AUTH_ACL  = AUTH_ACL 

		if (global.PATH_TRANSLATED) {
			request.PATH_TRANSLATED = PATH_TRANSLATED
		}

		server.DOCUMENT_ROOT = DOCUMENT_ROOT
		server.GATEWAY_INTERFACE = GATEWAY_INTERFACE
		server.SERVER_ADDR = SERVER_ADDR
		server.SERVER_PORT = SERVER_PORT
		server.SERVER_NAME = SERVER_NAME
		server.SERVER_PROTOCOL = SERVER_PROTOCOL
		server.SERVER_SOFTWARE = SERVER_SOFTWARE

		//	server.UPLOAD_DIR = UPLOAD_DIR
		//	server.SERVER_ADMIN = SERVER_ADMIN
	}
}
