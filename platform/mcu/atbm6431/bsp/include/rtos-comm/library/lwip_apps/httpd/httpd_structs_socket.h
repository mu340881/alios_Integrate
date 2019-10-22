#ifndef __HTTPD_STRUCTS_H__
#define __HTTPD_STRUCTS_H__

//#include "httpd.h"

/** This string is passed in the HTTP header as "Server: " */
#ifndef HTTPD_SERVER_AGENT
#define HTTPD_SERVER_AGENT "atbm iot wifi webserver"
#endif

/** Set this to 1 if you want to include code that creates HTTP headers
 * at runtime. Default is off: HTTP headers are then created statically
 * by the makefsdata tool. Static headers mean smaller code size, but
 * the (readonly) fsdata will grow a bit as every file includes the HTTP
 * header. */
#ifndef LWIP_HTTPD_DYNAMIC_HEADERS
#define LWIP_HTTPD_DYNAMIC_HEADERS 1
#endif
#define LWIP_HTTPD_SUPPORT_POST 1
#define LWIP_HTTPD_CGI 1
#define LWIP_HTTPD_SSI  1




#if LWIP_HTTPD_DYNAMIC_HEADERS
/** This struct is used for a list of HTTP header strings for various
 * filename extensions. */
typedef struct
{
  const char *extension;
  int headerIndex;
} tHTTPHeader;


/* Indexes into the g_psHTTPHeaderStrings array */
#define HTTP_HDR_HTML           0  /* text/html */
#define HTTP_HDR_SSI            1  /* text/html Expires... */
#define HTTP_HDR_GIF            2  /* image/gif */
#define HTTP_HDR_PNG            3  /* image/png */
#define HTTP_HDR_JPG            4  /* image/jpeg */
#define HTTP_HDR_BMP            5  /* image/bmp */
#define HTTP_HDR_ICO            6  /* image/x-icon */
#define HTTP_HDR_APP            7  /* application/octet-stream */
#define HTTP_HDR_JS             8  /* application/x-javascript */
#define HTTP_HDR_RA             9  /* application/x-javascript */
#define HTTP_HDR_CSS            10 /* text/css */
#define HTTP_HDR_SWF            11 /* application/x-shockwave-flash */
#define HTTP_HDR_XML            12 /* text/xml */
#define HTTP_HDR_DEFAULT_TYPE   13 /* text/plain */
#define HTTP_HDR_OK             14 /* 200 OK */
#define HTTP_HDR_NOT_FOUND      15 /* 404 File not found */
#define HTTP_HDR_BAD_REQUEST    16 /* 400 Bad request */
#define HTTP_HDR_NOT_IMPL       17 /* 501 Not Implemented */
#define HTTP_HDR_OK_11          18 /* 200 OK */
#define HTTP_HDR_NOT_FOUND_11   19 /* 404 File not found */
#define HTTP_HDR_BAD_REQUEST_11 20 /* 400 Bad request */
#define HTTP_HDR_NOT_IMPL_11    21 /* 501 Not Implemented */
#define HTTP_HDR_CONTENT_LENGTH 22 /* Content-Length: (HTTP 1.1)*/
#define HTTP_HDR_CONN_CLOSE     23 /* Connection: Close (HTTP 1.1) */
#define HTTP_HDR_SERVER         24 /* Server: HTTPD_SERVER_AGENT */
#define DEFAULT_404_HTML        25 /* default 404 body */
#define HTTP_HDR_ENCODING       26 /*encoding/gzip*/



#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */

#endif /* __HTTPD_STRUCTS_H__ */
