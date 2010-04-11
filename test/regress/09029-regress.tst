/*
 *  Regression not parsing this literal
 */
str = ' {
  "public"::"actionName": null,
  "public"::"absUrl": "",
  "public"::"appUrl": ".",
  "public"::"controllerName": "_Solo",
  "public"::"config": null,
  "public"::"flash": null,
  "public"::"home": "/Users/mob/hg/appweb/test/web",
  "public"::"host": {
    "public"::"documentRoot": "/Users/mob/hg/appweb/test/web",
    "public"::"name": "127.0.0.1:6700",
    "public"::"protocol": "http",
    "public"::"isVirtualHost": false,
    "public"::"isNamedVirtualHost": false,
    "public"::"software": "Embedthis-Appweb/3.0.2",
    "public"::"logErrors": false,
  },
  "public"::"params": {
    "a b c": "",
  },
  "public"::"request": {
    "public"::"accept": null,
    "public"::"acceptCharset": null,
    "public"::"acceptEncoding": null,
    "public"::"authAcl": undefined,
    "public"::"authGroup": undefined,
    "public"::"authType": null,
    "public"::"authUser": undefined,
    "public"::"connection": "Keep-Alive",
    "public"::"contentLength": -1,
    "public"::"cookies": null,
    "public"::"extension": "ejs",
    "public"::"files": null,
    "public"::"headers": {
      "HTTP_USER_AGENT": "Embedthis-http/3.0.2",
      "HTTP_HOST": "127.0.0.1",
      "HTTP_CONNECTION": "Keep-Alive",
    },
    "public"::"hostName": "127.0.0.1",
    "public"::"method": "GET",
    "public"::"mimeType": null,
    "public"::"pathInfo": null,
    "public"::"pathTranslated": null,
    "public"::"pragma": null,
    "public"::"query": "a+b+c",
    "public"::"originalUri": "/ejsProgram.ejs?a+b+c",
    "public"::"referrer": null,
    "public"::"remoteAddress": "127.0.0.1",
    "public"::"remoteHost": "127.0.0.1",
    "public"::"sessionID": "x6c99a985de0933997dab94062265d45c",
    "public"::"url": "/ejsProgram.ejs",
    "public"::"userAgent": "Embedthis-http/3.0.2",
  },
  "public"::"response": {
    "public"::"code": 200,
    "public"::"filename": "/Users/mob/hg/appweb/test/web/ejsProgram.ejs",
    "public"::"headers": {
      "Last-Modified": "Fri, 02 Oct 2009 00:21:22 UTC",
      "Cache-control": "no-cache=\\"set-cookie\\"",
      "Set-Cookie": "-ejs-session-=x6c99a985de0933997dab94062265d45c; path=/",
    },
    "public"::"mimeType": "text/html",
  },
  "public"::"session": {
  },
}'

o = deserialize(str)
assert(o.length > 0)
