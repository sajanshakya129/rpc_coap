#include "coap.h"
#include <stdio>

/*
 * The response handler
 */ 
static void
message_handler(struct coap_context_t *ctx, const coap_endpoint_t *local_interface, 
                const coap_address_t *remote, coap_pdu_t *sent, coap_pdu_t *received, 
                const coap_tid_t id) 
{
	unsigned char* data;
	size_t         data_len;
	if (COAP_RESPONSE_CLASS(received->hdr->code) == 2) 
	{
		if (coap_get_data(received, &data_len, &data))
		{
			cout<<"Received: %s\n", data<<endl;
		}
	}
}

struct node{
	string addr;
	int port;
	int family;
};
struct header{
	int type;
	int code;
};
struct request{
	struct node src;
	struct node dest;
	string api;
	struct header hdr;
};

void sendCOAPRequest(struct request *req){
	coap_context_t*   ctx;
	coap_address_t    dst_addr, src_addr;
	static coap_uri_t uri;
	fd_set            readfds; 
	coap_pdu_t*       request;
	const char*       server_uri = "coap://"+req.dest.addr+req.api;

	/* Prepare coap socket*/
	coap_address_init(&src_addr);
	src_addr.addr.sin.sin_family      = req.src.family;
	src_addr.addr.sin.sin_port        = htons(req.src.port);
	src_addr.addr.sin.sin_addr.s_addr = inet_addr(req.src.addr);
	ctx = coap_new_context(&src_addr);
	/* The destination endpoint */
	coap_address_init(&dst_addr);
	dst_addr.addr.sin.sin_family      = req.dest.family;
	dst_addr.addr.sin.sin_port        = htons(req.dest.port);
	dst_addr.addr.sin.sin_addr.s_addr = inet_addr(req.dest.addr);
	/* Prepare the request */
	coap_split_uri(server_uri, strlen(server_uri), &uri);
	request            = coap_new_pdu();	
	request->hdr->type = req.hdr.type;
	request->hdr->id   = coap_new_message_id(ctx);
	request->hdr->code = req.hdr.code;
	coap_add_option(request, COAP_OPTION_URI_PATH, uri.path.length, uri.path.s);
	coap_register_response_handler(ctx, message_handler);
	coap_send_confirmed(ctx, ctx->endpoint, &dst_addr, request);
	FD_ZERO(&readfds);
	FD_SET( ctx->sockfd, &readfds );
	int result = select( FD_SETSIZE, &readfds, 0, 0, NULL );
	if ( result < 0 ) /* socket error */
	{
		exit(EXIT_FAILURE);
	} 
	else if ( result > 0 && FD_ISSET( ctx->sockfd, &readfds )) /* socket read*/
	{	 
			coap_read( ctx );       
	}
}

int main(int argc, char* argv[])
{	struct request *req;
	unsigned char     get_method = 1;
	req->src->address="0.0.0.0";
	req->src->port=0;
	req->src->family=INET;
	req->dest->address=127.0.0.1;
	req->dest->port=5683;
	req->dest->family=INET6;
	req->api="/hello";
	req->hdr->type=COAP_MESSAGE_CON;
	req->hdr->code=get_method;
	sendCOAPRequest(req)
  return 0;
}