
#include "http.h"

void free_request(Request *req)
{
    if(req->path != NULL) free(req->path);
    if(req->accept != NULL) free(req->accept);
    if(req->accept_charset != NULL) free(req->accept_charset);
    if(req->accept_encoding != NULL) free(req->accept_encoding);
    if(req->accept_language != NULL) free(req->accept_language);
    if(req->authorization != NULL) free(req->authorization);
    if(req->expect != NULL) free(req->expect);
    if(req->from != NULL) free(req->from);
    if(req->host != NULL) free(req->host);
    if(req->if_match != NULL) free(req->if_match);
    if(req->if_modified_since != NULL) free(req->if_modified_since);
    if(req->if_none_match != NULL) free(req->if_none_match);
    if(req->if_range != NULL) free(req->if_range);
    if(req->if_unmodified_since != NULL) free(req->if_unmodified_since);
    if(req->max_forwards != NULL) free(req->max_forwards);
    if(req->proxy_authorization != NULL) free(req->proxy_authorization);
    if(req->range != NULL) free(req->range);
    if(req->referer != NULL) free(req->referer);
    if(req->TE != NULL) free(req->TE);
    if(req->user_agent != NULL) free(req->user_agent);

    if(req->cache_control != NULL) free(req->cache_control);
    if(req->connection != NULL) free(req->connection);
    //if(req->date != NULL) free(req->date);
    if(req->pragma != NULL) free(req->pragma);
    if(req->trailer != NULL) free(req->trailer);
    if(req->transfer_encoding != NULL) free(req->transfer_encoding);
    if(req->upgrade != NULL) free(req->upgrade);
    if(req->via != NULL) free(req->via);
    if(req->warning != NULL) free(req->warning);


    if(req->allow != NULL) free(req->allow);
    if(req->content_encoding != NULL) free(req->content_encoding);
    if(req->content_language != NULL) free(req->content_language);
    if(req->content_length != NULL) free(req->content_length);
    if(req->content_location != NULL) free(req->content_location);
    if(req->content_md5 != NULL) free(req->content_md5);
    if(req->content_range != NULL) free(req->content_range);
    if(req->content_type != NULL) free(req->content_type);
    if(req->expires != NULL) free(req->expires);
    if(req->last_modified != NULL) free(req->last_modified);

    if(req->message_body != NULL) free(req->message_body);
}


void free_response(Response *resp)
{
    if(resp->accept_ranges != NULL) free(resp->accept_ranges);

    if(resp->cache_control != NULL) free(resp->cache_control);
    if(resp->connection != NULL) free(resp->connection);
    //if(resp->date != NULL) free(resp->date);
    if(resp->pragma != NULL) free(resp->pragma);
    if(resp->trailer != NULL) free(resp->trailer);
    if(resp->transfer_encoding != NULL) free(resp->transfer_encoding);
    if(resp->upgrade != NULL) free(resp->upgrade);
    if(resp->via != NULL) free(resp->via);
    if(resp->warning != NULL) free(resp->warning);


    if(resp->allow != NULL) free(resp->allow);
    if(resp->content_encoding != NULL) free(resp->content_encoding);
    if(resp->content_language != NULL) free(resp->content_language);
    if(resp->content_length != NULL) free(resp->content_length);
    if(resp->content_location != NULL) free(resp->content_location);
    if(resp->content_md5 != NULL) free(resp->content_md5);
    if(resp->content_range != NULL) free(resp->content_range);
    if(resp->content_type != NULL) free(resp->content_type);
    if(resp->expires != NULL) free(resp->expires);
    if(resp->last_modified != NULL) free(resp->last_modified);

    if(resp->message_body != NULL) free(resp->message_body);
}
