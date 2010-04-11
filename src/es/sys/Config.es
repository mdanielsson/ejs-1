/*
 *  Config.es - Configuration settings from ./configure
 *
 *  Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
 */

module ejs.sys {

    /* NOTE: These values are updated at run-time by src/types/sys/ejsConfig.c */

    /**
     *  Config class providing settings for various "configure" program settings.
     *  @spec ejs
     *  @stability evolving
     */
    native class Config extends Object {

        use default namespace public

        /**
         *  True if a debug build
         */
        static const Debug: Boolean

        /**
         *  CPU type (eg. i386, ppc, arm)
         */
        static const CPU: String

        /**
         *  Build with database (SQLite) support
            @hide
         */
        static const DB: Boolean

        /**
         *  Build with E4X support
            @hide
         */
        static const E4X: Boolean

        /**
         *  Build with floating point support
            @hide
         */
        static const Floating: Boolean

        /**
         *  Build with HTTP client support 
            @hide
         */
        static const Http: Boolean

        /**
         *  Language specification level. (ecma|plus|fixed)
            @hide
         */
        static const Lang: String

        /**
         *  Build with legacy API support
            @hide
         */
        static const Legacy: Boolean

        /**
         *  Build with multithreading support
         */
        static const Multithread: Boolean

        /**
         *  Number type
            @hide
         */
        static const NumberType: String

        /**
         *  Operating system version. One of: WIN, LINUX, MACOSX, FREEBSD, SOLARIS
         */
        static const OS: String

        /**
         *  Ejscript product name. Single word name.
         */
        static const Product: String

        /**
         *  Regular expression support.
         *  @hide
         */
        static const RegularExpressions: Boolean

        /**
         *  Ejscript product title. Multiword title.
         */
        static const Title: String

        /**
         *  Ejscript version. Multiword title. Format is Major.Minor.Patch-Build For example: 1.1.2-1
         */
        static const Version: String

        /**
         *  Installation library directory
         */
        static const LibDir: Path

        /**
         *  Binaries directory
         */
        static const BinDir: Path

        /**
         *  Modules directory
         */
        static const ModDir: Path
    }
}

