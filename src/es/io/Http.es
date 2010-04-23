/**
    Http.es -- HTTP client side communications
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

module ejs.io {

    /**
        The Http object represents a Hypertext Transfer Protocol version 1.1 client connection. It is used to issue 
        HTTP requests and capture responses. It supports the HTTP/1.1 standard including methods for GET, POST, 
        PUT, DELETE, OPTIONS, and TRACE. It also supports Keep-Alive and SSL connections. 
        @spec ejs
        @stability prototype
     */
    native class Http implements Stream {

        use default namespace public

        /** 
          HTTP Continue Status (100)
         */     
        static const Continue : Number = 100

        /** 
            HTTP Success Status (200) 
         */     
        static const Ok : Number = 200

        /** 
            HTTP Created Status (201) 
         */     
        static const Created : Number = 201

        /** 
            HTTP Accepted Status (202) 
         */     
        static const Accepted : Number = 202

        /** 
            HTTP Non-Authoritative Information Status (203) 
         */     
        static const NotAuthoritative : Number = 203

        /** 
            HTTP No Content Status (204)  
         */     
        static const NoContent : Number = 204

        /** 
            HTTP Reset Content Status (205) 
         */     
        static const Reset : Number = 205

        /** 
            HTTP Partial Content Status (206) 
         */     
        static const PartialContent : Number = 206

        /** 
            HTTP Multiple Choices Status (300) 
         */     
        static const MultipleChoice : Number = 300

        /** 
            HTTP Moved Permanently Status (301) 
         */     
        static const MovedPermanently : Number = 301

        /** 
            HTTP Found but Moved Temporily Status (302) 
         */     
        static const MovedTemporarily : Number = 302

        /** 
            HTTP See Other Status (303) 
         */     
        static const SeeOther : Number = 303

        /** 
            HTTP Not Modified Status (304)     
         */
        static const NotModified : Number = 304

        /** 
            HTTP Use Proxy Status (305) 
         */     
        static const UseProxy : Number = 305

        /** 
            HTTP Bad Request Status(400) 
         */     
        static const BadRequest : Number = 400

        /** 
            HTTP Unauthorized Status (401) 
         */     
        static const Unauthorized : Number = 401

        /** 
            HTTP Payment Required Status (402) 
         */     
        static const PaymentRequired : Number = 402

        /** 
            HTTP Forbidden Status (403)  
         */     
        static const Forbidden : Number = 403

        /** 
            HTTP Not Found Status (404) 
         */     
        static const NotFound : Number = 404

        /** 
            HTTP Method Not Allowed Status (405) 
         */     
        static const BadMethod : Number = 405

        /** 
            HTTP Not Acceptable Status (406) 
         */     
        static const NotAcceptable : Number = 406

        /** 
            HTTP ProxyAuthentication Required Status (407) 
         */     
        static const ProxyAuthRequired : Number = 407

        /** 
            HTTP Request Timeout Status (408) 
         */     
        static const RequestTimeout : Number = 408

        /** 
            HTTP Conflict Status (409) 
         */     
        static const Conflict : Number = 409

        /** 
            HTTP Gone Status (410) 
         */     
        static const Gone : Number = 410

        /** 
            HTTP Length Required Status (411) 
         */     
        static const LengthRequired : Number = 411
        
        /** 
            HTTP Precondition Failed Status (412) 
         */     
        static const PrecondFailed : Number = 412

        /** 
            HTTP Request Entity Too Large Status (413) 
         */     
        static const EntityTooLarge : Number = 413

        /** 
            HTTP Request URI Too Long Status (414)  
         */     
        static const UriTooLong : Number = 414

        /** 
            HTTP Unsupported Media Type (415) 
         */     
        static const UnsupportedMedia : Number = 415

        /** 
            HTTP Requested Range Not Satisfiable (416) 
         */     
        static const BadRange : Number = 416

        /** 
            HTTP Server Error Status (500) 
         */     
        static const ServerError : Number = 500

        /** 
            HTTP Not Implemented Status (501) 
         */     
        static const NotImplemented : Number = 501

        /** 
            HTTP Bad Gateway Status (502) 
         */     
        static const BadGateway : Number   = 502

        /** 
            HTTP Service Unavailable Status (503) 
         */     
        static const ServiceUnavailable : Number = 503

        /** 
            HTTP Gateway Timeout Status (504) 
         */     
        static const GatewayTimeout : Number = 504

        /** 
            HTTP Http Version Not Supported Status (505) 
         */     
        static const VersionNotSupported: Number = 505

        /**
            Callback event mask for readable events
         */
        static const Read: Number = 2

        /**
            Callback event mask for writeable events
         */
        static const Write: Number = 4

        private var _response: String

        /**
            Create an Http object. The object is initialized with the URI
            @param uri The (optional) URI to initialize with.
            @throws IOError if the URI is malformed.
         */
        native function Http(uri: String = null)

        /**
            Add a request header. Must be done before the Http request is issued. 
            @param key The header keyword for the request, e.g. "accept".
            @param value The value to associate with the header, e.g. "yes"
            @param overwrite If true, overwrite existing headers of the same key name.
         */
        native function addHeader(key: String, value: String, overwrite: Boolean = true): Void

        /**
            The number of response data bytes that are currently available for reading.
            This API is not supported and is provided only for compliance with the Stream interface.
            @hide
         */
        native function get available(): Number 

        /**
            Return whether chunked transfer encoding will be applied to the body data of the request. Chunked encoding
            is used when an explicit request content length is unknown at the time the request headers must be emitted.
            Chunked encoding is automatically enabled if $post, $put or $upload is called and a contentLength has not been 
            previously set. This call is deprecated.
            @hide
         */
        native function get chunked(): Boolean

        /**
            Control whether chunked transfer encoding will be applied to the body data of the request. Chunked encoding
            is used when an explicit request content length is unknown at the time the request headers must be emitted.
            Chunked encoding is automatically enabled if $post, $put or $upload is called and a contentLength has not been 
            previously set. This call is deprecated.
            @param value Boolean value. Set to true to enable chunked transfers.
            @hide
         */
        native function set chunked(value: Boolean): Void

        /**
            Close the connection to the server and free up all associated resources. 
            This closes any open network connection and resets the http object to be ready for another connection. 
            Connections should be explicitly closed rather than relying on the garbage collector to dispose of the 
            Http object and automatically close the connection.
            @param graceful if true, then close the socket gracefully after writing all pending data.
            @stability prototype
         */
        native function close(graceful: Boolean = true): Void 

        /**
            Issue a HTTP request for the current method and uri. The HTTP method should be defined via the $method 
            property and uri via the $uri property. This routine is typically not used. Rather $get, $head, $post or $put
            are used instead.
            @param uri New uri to use. This overrides any previously defined uri for the Http object.
            @param data Data objects to send with the request. Data is written raw and is not encoded or converted. 
                However, the routine intelligently handles arrays such that, each element of the array will be written. 
            @throws IOError if the request cannot be issued to the remote server.
         */
        native function connect(uri: String = null, ...data): Void

        /**
            Filename of the certificate file for this HTTP object. The certificate is only used if secure communications 
            are in use.
            @hide
         */
        native function get certificate(): Path

        /**
            @duplicate Http.certificate
            @param certFile The name of the certificate file.
            @throws IOError if the file does not exist.
         */
        native function set certificate(certFile: Path): Void

        /**
            Http response status code.  Set to an integer value or -1 if the Http response code is not known. e.g. 200.  
         */
        native function get code(): Number

        /**
            Http response message from the Http response status line, e.g. OK. This is the additional text added to the
            first line in the Http protocol response.
         */
        native function get codeString(): String

        /**
            Content encoding used for the response. Set to the value provided by the Http Content-Type response header or
            or null if not known.
            @hide
         */
        native function get contentEncoding(): String

        /**
            Request/Response body length. Reading this property, will return the response content body length. The value
            will be the response body length in bytes or -1 if no body or if the body length is not known.
            Setting this value will set the outgoing request body length. This will set the request Content-Length 
            header value and is used when using $write to manually send the requeset body data.
            WARNING: This API will change in the next release. It will become a sole getter. Use addHeader to define
            a Content-Length header if you require defining a request body content length.
            @stability prototype
         */
        native function get contentLength(): Number

        /**
            @duplicate Http.contentLength
            @param length The length of body data that will be sent with the request.
         */
        native function set contentLength(length: Number): Void

        /**  DEPRECATED @hide */
        function get bodyLength(): Void
            contentLength

        /**  DEPRECATED @hide */
        function set bodyLength(value: Number): Void {
            contentLength = value
        }

        /**
            Response content type derrived from the response Http Content-Type header.
         */
        native function get contentType(): String

        /**
            When the response was generated. Response date derrived from the response Http Date header.
         */
        native function get date(): Date

        /**
            Issue a DELETE request for the current uri. This function will block until the the request completes or 
            fails, unless a callback function has been defined. If a callback has been specified, this function 
            will initiate the request and return immediately.
            @param uri The uri to delete. This overrides any previously defined uri for the Http object.
            @param data Data objects to send with the request. Data is written raw and is not encoded or converted. 
                However, the routine intelligently handles arrays such that, each element of the array will be written. 
            @throws IOError if the request cannot be issued to the remote server.
         */
        native function del(uri: String = null, ...data): Void

        /**
            When the response content expires. This is derrived from the response Http Expires header.
         */
        native function get expires(): Date

        /**
            Flush any buffered write data
            @param graceful If true, then write all pending.
         */
        native function flush(graceful: Boolean = true): Void

        /**
            Will response redirects be automatically followed by this Http object. If true, the Http class will 
            automatically reissue requests if redirected by the server.
         */
        native function get followRedirects(): Boolean

        /**
            Eanble or disable following redirects from the connection remove server. Default is true.
            @param flag Set to true to follow redirects.
         */
        native function set followRedirects(flag: Boolean): Void

        /**
            Issue a POST request with form data the current uri. This function will block until the the request 
            completes or fails, unless a callback function has been defined. If a callback has been specified, 
            this function will initiate the request and return immediately.
            @param uri Optional request uri. If non-null, this overrides any previously defined uri for the Http object.
            @param postData Optional object hash of key value pairs to use as the post data. These are www-url-encoded and
                the content mime type is set to "application/x-www-form-urlencoded".
            @throws IOError if the request cannot be issued to the remote server.
         */
        native function form(uri: String, postData: Object): Void

        /**
            Issue a GET request for the current uri. This call initiates a GET request. It does not wait for the
            request to complete. Once initiated, one of the $read or response routines  may be used to receive the 
            response data.
            @param uri The uri to get. This overrides any previously defined uri for the Http object.
            @param data Data objects to send with the request. Data is written raw and is not encoded or converted. 
                However, the routine intelligently handles arrays such that, each element of the array will be written. 
            @throws IOError if the request cannot be issued to the remote server.
         */
        native function get(uri: String = null, ...data): Void

        /**
            Issue a HEAD request for the current uri. This call initiates a HEAD request. Once initiated, $read and $write
            may be issued to send and receive data.
            @param uri The request uri. This overrides any previously defined uri for the Http object.
            @throws IOError if the request cannot be issued to the remote server.
         */
        native function head(uri: String = null): Void

        /**
            Get the value of any response header field.
            @return The header field value as a string or null if not known.
         */
        native function header(key: String): String

        /**
            Response headers. Use header() to retrieve a single header value.
         */
        native function get headers(): Array

        /**
            Is the connection is utilizing SSL.
         */
        native function get isSecure(): Boolean

        /**
            Private key file for this Https object.
            NOT currently supported.
            @return The file name.
            @hide
         */
        native function get key(): Path

        /**
            @duplicate Http.key
            @param keyFile The name of the key file.
            @throws IOError if the file does not exist.
            @hide
         */
        native function set key(keyFile: Path): Void

        /**
            When the response content was last modified. Set to the the value of the response Http Last-Modified header.
            Set to null if not known.
         */
        native function get lastModified(): Date

        /**
            Http request method for this Http object.
         */
        native function get method(): String

        /**
            Set or reset the Http object's request method. Default method is GET.
            @param name The method name as a string.
            @throws IOError if the request is not GET, POST, HEAD, OPTIONS, PUT, DELETE or TRACE.
         */
        native function set method(name: String)

        /**
            Get the mime type for a path or extension string.
            NOTE: this routine will migrate to the Url class in the future.
            @param path Path or extension string to examine.
            @returns The mime type string
         */
        native static function mimeType(path: String): String

        /**
            Issue an OPTIONS request for the current uri. Use $readString to retrieve the response.
            @param uri New uri to use. This overrides any previously defined uri for the Http object.
            @throws IOError if the request cannot be issued to the remote server.
         */
        native function options(uri: String = null): Void

        /**
            Initiate a POST request for the current uri. Posted data is NOT URL encoded. If you want to post data to a 
            form, consider using the $form method instead which automatically URL encodes the data. Post data may be 
            supplied may alternatively be supplied via $write. If a contentLength has not been previously defined for this
            request, chunked transfer encoding will be enabled (See $chunked). 
            @param uri Optional request uri. If non-null, this overrides any previously defined uri for the Http object.
            @param data Data objects to send with the post request. Data is written raw and is not encoded or converted. 
                However, this routine intelligently handles arrays such that, each element of the array will be written. 
            @throws IOError if the request cannot be issued to the remote server.
         */
        native function post(uri: String, ...data): Void

        /**
            Issue a PUT request for the current uri. This call initiates a PUT request. If a contentLength has not been 
            previously defined for this request, chunked transfer encoding will be enabled (See $chunked). 
            @param uri The uri to delete. This overrides any previously defined uri for the Http object.
            @param data Optional data objects to write to the request stream. Data is written raw and is not encoded 
                or converted.  However, put intelligently handles arrays such that, each element of the array will be 
                written. If encoding of put data is required, use the BinaryStream filter. If no putData is supplied,
                and the $contentLength is non-zero you must call $write to supply the body data.
            @param data Optional object hash of key value pairs to use as the post data.
            @throws IOError if the request cannot be issued to the remote server.
         */
        native function put(uri: String, ...data): Void

        /**
            Read a block of response content data from the connection.
            @param buffer Destination byte array for the read data.
            @param offset Offset in the byte array to place the data. If offset is -1, data is appended at the 
                current byte array write position.
            @param count Number of bytes to read. 
            @returns a count of the bytes actually read. This call may return with fewer bytes read than requested.
                If a callback has been defined, this call will not block and may return zero if no data is currently 
                available. If no callback has been defined, it will block.
            @throws IOError if an I/O error occurs.
         */
        native function read(buffer: ByteArray, offset: Number = 0, count: Number = -1): Number

        /**
            Read the request response as a string.
            @param count of bytes to read. Returns the entire response contents if count is -1.
            @returns a string of $count characters beginning at the start of the response data.
            @throws IOError if an I/O error occurs.
         */
        native function readString(count: Number = -1): String

        /**
            Read the request response as an array of lines.
            @param count of linese to read. Returns the entire response contents if count is -1.
            @returns an array of strings
            @throws IOError if an I/O error occurs.
         */
        native function readLines(count: Number = -1): Array

        /**
            Read the request response as an XML document.
            @returns the response content as an XML object 
            @throws IOError if an I/O error occurs.
         */
        function readXml(): XML
            XML(response)

        /**
            Response body content. The first time this property is read, the response content will be read and buffered.
            Set to the response as a string of characters. If the response has no body content, the empty string will 
            be returned.
            @throws IOError if an I/O error occurs.
         */
        native function get response(): String

        /**
            The maximum number of retries for a request. Retries are essential as the HTTP protocol permits a 
            server or network to be unreliable. The default retries is 2.
            @hide
         */
        native function get retries(): Number

        /**
            Define the number of retries of a request. Retries are essential as the HTTP protocol permits a server or
            network to be unreliable. The default retries is 2.
            @param count Number of retries. A retry count of 1 will retry a failed request once.
            @hide
         */
        native function set retries(count: Number): Void

        /**
            WARNING: this feature will be replaced with a listener based interface in the next release.
            Define a callback to be invoked for readable and/or writable events. Callbacks are used when writing event
            based programs that must not block. When a callback is defined, the $read and $write routines will not block. 
            @param eventMask Mask of events of interest. Select from Read, Write.
            @param cb Callback function to invoke in response to I/O events.
            <pre>
                function callback(e: Event): Void
            </pre>
            Where e.data == http. The event arg may be either a HttpDataEvent or a HtttpErrorEvent. The callback is
            invoked when there is an error, response data to read  or on end of request where $available == 0 and a read
                will return null). It may also invoked to signal that the underlying socket can accept write data. 
            The HttpError event will be passed  on any request processing errors. Does not include remote server errors.
            @hide
         */
        native function setCallback(eventMask: Number, cb: Function): Void

        /**
            Set the user credentials to use if the request requires authentication.
         */
        native function setCredentials(username: String, password: String): Void

        /**
            Request timeout in milliseconds. This is the idle timeout value. If the request has no I/O activity for 
            this time period, it will be retried or aborted.
         */
        native function get timeout(): Number

        /**
            Set the request timeout.
            @param timeout Number of milliseconds to block while attempting requests. -1 means no timeout.
         */
        native function set timeout(timeout: Number): Void

        /**
            Issue a TRACE request for the current uri. Use $readString to retrieve the response.
            @param uri New uri to use. This overrides any previously defined uri for the Http object.
            @throws IOError if the request cannot be issued to the remote server.
         */
        native function trace(uri: String = null): Void

        /**
            Upload files using multipart/mime. This routine initiates a POST request and sends the specified files
            and form fields using multipart mime encoding. This call also automatically enables chunked transfer 
            encoding (See $chunked). It can only be used on servers that accept HTTP/1.1 request which should be all 
            servers.
            @param url The url to upload to. This overrides any previously defined uri for the Http object.
            @param files Object hash of files to upload
            @param fields Object hash of of form fields to send
            @example
                fields = { name: "John Smith", address: "700 Park Avenue" }
                files = { file1: "a.txt, file2: "b.txt" }
                http.upload(URL, files, fields)
         */
        function upload(url: String, files: Object, fields: Object = null): Void {
            let boundary = "<<Upload Boundary>>"
            let buf = new ByteArray(4096)
            let http = this
            buf.output = function (buf) {
                http.write(buf)
                buf.flush(false)
            }
            addHeader("Content-Type", "multipart/form-data; boundary=" + boundary)
            post(url)
            if (fields) {
                for (key in fields) {
                    buf.write('--' + boundary + "\r\n")
                    buf.write('Content-Disposition: form-data; name=' + encodeURI(key) + "\r\n")
                    buf.write('Content-Type: application/x-www-form-urlencoded\r\n\r\n')
                    buf.write(encodeURI(fields[key]) + "\r\n")
                }
            }
            for (key in files) {
                file = files[key]
                buf.write('--' + boundary + "\r\n")
                buf.write('Content-Disposition: form-data; name=' + key + '; filename=' + Path(file).basename + "\r\n")
                buf.write('Content-Type: ' + mimeType(file) + "\r\n\r\n")

                f = File(file).open()
                data = new ByteArray
                while (f.read(data)) {
                    buf.write(data)
                }
                f.close()
                buf.write("\r\n")
            }
            buf.write('--' + boundary + "--\r\n\r\n")
            buf.flush()
            http.wait()
        }

        /**
            URI for this Http object.
         */
        native function get uri(): String

        /**
            Set or reset the Http object's URI.
            @param newUri The URI as a string.
            @throws IOError if the URI is malformed.
         */
        native function set uri(newUri: String): Void

        /**
            Wait for a request to complete.
            @param timeout Time in seconds to wait for the request to complete
            @return True if the request successfully completes.
         */
        native function wait(timeout: Number = -1): Boolean

        /**
            Write body data to the request stream. The write call blocks while writing data. The Http.contentLength should 
            normally be set prior to writing any data to ensure that the request "Content-length" header is properly 
            defined. If the body length has not been defined, the data will be transferred using chunked transfers. In
            this case, you should call write() with no data to signify the end of the write stream.
            the Content-Length header will not be defined and the remote server will have to close the underling 
            HTTP connection at the completion of the request. This will prevent HTTP keep-alive for subsequent 
            requests.
            @param data Data objects to write to the request stream. Data is written raw and is not encoded or converted. 
                However, write intelligently handles arrays such that, each element of the array will be written. 
                If encoding of write data is required, use the BinaryStream filter. 
            @throws StateError if the Http method is not set to POST.
            @throws IOError if more post data is written than specified via the contentLength property.
         */
        native function write(...data): Void
    }

    /**
        WARNING: this feature will be replaced with a listener based interface in the next release.
        Data event issued to the callback function.
        @hide
     */
    class HttpDataEvent extends Event {
        /**
            Mask of pending events. Set to include $Read and/or $Write values.
         */
        public var eventMask: Number
    }

    /**
        WARNING: this feature will be replaced with a listener based interface in the next release.
        Error event issued to the callback function if any errors occur during an Http request.
        @hide
     */
    class HttpErrorEvent extends Event {
    }
}


/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
    
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
    
    @end
 */
