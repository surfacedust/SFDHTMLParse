namespace cpp radar

enum Status {
    UNLOAD = 0,//还未进入URL Server TODO缓存
    TODO  = 1,//进入URL Server缓存中，等待被Spider请求
    DOING = 2,//已发送给Spider，正在爬取和解析
    OK = 200,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    PAYMENT_REQUIRED = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    REQUEST_TIMEOUT = 408,
    CONFLIICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    REQUEST_ENTITY_TOO_LARGE = 413,
    REQUEST_URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    REQUESTED_RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505 
}

enum ExpandWay {
    BY_SEED_DIRECTORY,//目录扩展
    BY_SEED_ROOT,//站内扩展
    BY_SEED_DOMAIN,//按域扩展
    BY_NOLIMIT,//无限扩展
}

struct Url {
    1:required i64 id,
    2:required i64 seed_id,
    3:required i64 parent_id,
    4:required string url,
    5:required double weight,
    6:required i32 depth,
    7:required ExpandWay expend_way,
    8:required Status status,
}

//for crawler sends failed urls
struct FailedUrl {
    1:required i64 id,
    2:required string url,
    3:required Status status,
}

//for parser sends success urls
struct SuccessUrl {
    1:required i64 seed_id,
    2:required i64 id,
    3:required string url,
}

//for parser sends new urls
struct NewUrl {
    1:required string url,
    2:required double weight,
    3:required i32 depth,
}

struct ParserUrl {
    1:required SuccessUrl succ_url,
    2:required list<NewUrl> new_urls,
}

service UrlService {
    list<Url> GetUrls(1:i32 num);
    i32 SendParserUrls(1: list<ParserUrl> urls);
    i32 SendFailedUrls(1: list<FailedUrl> urls);
}
