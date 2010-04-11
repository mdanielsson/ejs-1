/**
 *  uploadedFile.es - 
 */

module ejs.web {

    /**
     *  Upload file class. Users do not instantiate this class. Rather, the web framework will allocate and store
     *  instances in the Request.files hash.
     *  @spec ejs
     *  @stability evolving
     */
    class UploadFile {

        use default namespace public

        /**
         *  Name of the uploaded file given by the client
         */
        native var clientFilename: String

        /**
         *  Mime type of the encoded data
         */
        native var contentType: String

        /**
         *  Name of the uploaded file. This is a temporary file in the upload directory.
         */
        native var filename: String

        /**
         *  HTML input ID for the upload file element
         */
        var name: String
 
        /**
         *  Size of the uploaded file in bytes
         */
        native var size: Number
    }
}

