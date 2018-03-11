#ifndef HTTP_H
#define HTTP_H


/*!
Data type for date and time
*/
typedef struct Date
{
    int year;
    int month;
    int day;
    int hr;
    int min;
    int sec;
    int time_zone;
} Date;

/*!
An enum for all the HTTP Request type/method
*/
typedef enum RequestType
{
    GET, HEAD, POST, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH, NA
} RequestType;

/*!
Data type for HTTP Request
*/
typedef struct Request
{
    /*!
    Starting Line.
    */
    RequestType type;
    /*!
    Starting Line.
    */
    int protocol_version;
    /*!
    Starting Line.
    */
    char *path;


    /*!
    Request Header Fields..
    Accept:
    */
    char *accept;
    /*!
    Request Header Fields..
    Accept-Charset:
    */
    char *accept_charset;
    /*!
    Request Header Fields..
    Accept-Encoding:
    */
    char *accept_encoding;
    /*!
    Request Header Fields..
    Accept-Language:
    */
    char *accept_language;
    /*!
    Request Header Fields..
    Authorization:
    */
    char *authorization;
    /*!
    Request Header Fields.
    Expect:
    */
    char *expect;
    /*!
    Request Header Fields.
    From:
    */
    char *from;
    /*!
    Request Header Fields.
    Host:
    */
    char *host;
    /*!
    Request Header Fields.
    If-Match:
    */
    char *if_match;
    /*!
    Request Header Fields.
    If-Modified-Since:
    */
    char *if_modified_since;
    /*!
    Request Header Fields.
    If-None-Match
    */
    char *if_none_match;
    /*!
    Request Header Fields.
    If-Range
    */
    char *if_range;
    /*!
    Request Header Fields.
    If-Unmodified-Since:
    */
    char *if_unmodified_since;
    /*!
    Request Header Fields.
    Max-Forwards:
    */
    char *max_forwards;
    /*!
    Request Header Fields.
    Proxy-Authorization:
    */
    char *proxy_authorization;
    /*!
    Request Header Fields.
    Range:
    */
    char *range;
    /*!
    Request Header Fields.
    Referer:
    */
    char *referer;
    /*!
    Request Header Fields.
    TE:
    */
    char *TE;
    /*!
    Request Header Fields.
    User-Agent:
    */
    char *user_agent;


    /*!
    General Header Fields.
    Cache-Control:
    */
    char *cache_control;
    /*!
    General Header Fields.
    Connection:
    */
    char *connection;
    /*!
    General Header Fields.
    Date:
    */
    char *date;
    /*!
    General Header Fields.
    Pragma:
    */
    char *pragma;
    /*!
    General Header Fields.
    Trailer:
    */
    char *trailer;
    /*!
    General Header Fields.
    Transfer-Encoding:
    */
    char *transfer_encoding;
    /*!
    General Header Fields.
    Upgrade:
    */
    char *upgrade;
    /*!
    General Header Fields.
    Via:
    */
    char *via;
    /*!
    General Header Fields.
    Warning:
    */
    char *warning;


    /*!
    Entity Header Fields.
    Allow:
    */
    char *allow;
    /*!
    Entity Header Fields.
    Content-Encoding:
    */
    char *content_encoding;
    /*!
    Entity Header Fields.
    Content-Language:
    */
    char *content_language;
    /*!
    Entity Header Fields.
    Content-Length:
    */
    char *content_length;
    /*!
    Entity Header Fields.
    Content-Range:
    */
    char *content_location;
    /*!
    Entity Header Fields.
    Content-MD5:
    */
    char *content_md5;
    /*!
    Entity Header Fields.
    Content-Range:
    */
    char *content_range;
    /*!
    Entity Header Fields.
    Content-Type:
    */
    char *content_type;
    /*!
    Entity Header Fields.
    Expires:
    */
    char *expires;
    /*!
    Entity Header Fields.
    Last-Modified:
    */
    char *last_modified;
} Request;

#define StatusCode_Continue 100
#define StatusCode_SwitchingProtocols 101
#define StatusCode_OK 200
#define StatusCode_Created 201
#define StatusCode_Accepted 202
#define StatusCode_NonAuthoritativeInformation 203
#define StatusCode_NoContent 204
#define StatusCode_ResetContent 205
#define StatusCode_PartialContent 206
#define StatusCode_MultipleChoices 300
#define StatusCode_MovedPermanently 301
#define StatusCode_Found 302
#define StatusCode_SeeOther 303
#define StatusCode_NotModified 304
#define StatusCode_UseProxy 305
#define StatusCode_TemporaryRedirect 307
#define StatusCode_BadRequest 400
#define StatusCode_Unauthorized 401
#define StatusCode_PaymentRequired 402
#define StatusCode_Forbidden 403
#define StatusCode_NotFound 404
#define StatusCode_MethodNotAllowed 405
#define StatusCode_NotAcceptable 406
#define StatusCode_ProxyAuthenticationRequired 407
#define StatusCode_RequestTimeout 408
#define StatusCode_Conflict 409
#define StatusCode_Gone 410
#define StatusCode_LengthRequired 411
#define StatusCode_PreconditionFailed 412
#define StatusCode_RequestEntityTooLarge 413
#define StatusCode_RequestURITooLarge 414
#define StatusCode_UnsupportedMediaType 415
#define StatusCode_RequestedRangeNotSatisfiable 416
#define StatusCode_ExpectationFailed 417
#define StatusCode_InternalServerError 500
#define StatusCode_NotImplemented 501
#define StatusCode_BadGateway 502
#define StatusCode_ServiceUnavailable 503
#define StatusCode_GatewayTimeOut 504
#define StatusCode_HTTPVersionNotSupported 505

/*!
Data type for HTTP Response
*/
typedef struct Response
{
    int protocol_version;
    int status_code;
    Date date;
    char *server_name;
    char *content_type;
    char *content_encoding;
    int connection;
    char *content;

    /*!
    Response Header Field.
    Accept-Ranges:
    */
    char *accept_ranges;
    /*!
    Response Header Field.
    Age:
    */
    char *age;
    /*!
    Response Header Field.
    ETag:
    */
    char *etag;
    /*!
    Response Header Field.
    Location:
    */
    char *location;
    /*!
    Response Header Field.
    Proxy-Authenticate:
    */
    char *proxy_authenticate;
    /*!
    Response Header Field.
    Retry-After:
    */
    char *retry_after;
    /*!
    Response Header Field.
    Server:
    */
    char *server;
    /*!
    Response Header Field.
    Vary:
    */
    char *vary;
    /*!
    Response Header Field.
    WWW-Authenticate:
    */
    char *www_authenticate;


} Response;

/*
GET /index.html HTTP/1.1
Host: www.example.com
*/

/*
HTTP/1.1 200 OK
Content-Length: 3059
Server: GWS/2.0
Date: Sat, 11 Jan 2003 02:44:04 GMT
Content-Type: text/html
Cache-control: private
Set-Cookie: PREF=ID=73d4aef52e57bae9:TM=1042253044:LM=1042253044:S=SMCc_HRPCQiqy
X9j; expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=.google.com
Connection: keep-alive
*/
/*
HTTP/1.1 200 OK
Date: Mon, 23 May 2005 22:38:34 GMT
Content-Type: text/html; charset=UTF-8
Content-Encoding: UTF-8
Content-Length: 138
Last-Modified: Wed, 08 Jan 2003 23:11:55 GMT
Server: Apache/1.3.3.7 (Unix) (Red-Hat/Linux)
ETag: "3f80f-1b6-3e1cb03b"
Accept-Ranges: bytes
Connection: close
*/

#endif
