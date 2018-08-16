#include "coapClient.hpp"
#include "coap.h"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/*
 * The response handler
 */

typedef struct node{
  const char* addr;
  int port;
  int family;
} node;
typedef struct header{
  int type;
  int code;
} header;
typedef struct coapRequest{
  node src;
  node dest;
  string api;
  header hdr;
} coapRequest;

void CoapClient::message_handler(struct coap_context_t *ctx,
                                coap_session_t *sess, coap_pdu_t *sent,
                                 coap_pdu_t *received, const coap_tid_t id) {
  unsigned char *data;
  size_t data_len;
  if (COAP_RESPONSE_CLASS(received->code) == 2) {
    if (coap_get_data(received, &data_len, &data)) {
      cout << "Received:" << data << endl;
    }
  }
}

void CoapClient::sendCOAPRequest(struct coapRequest *req) {
  coap_context_t *ctx;
  coap_session_t *sess;
  coap_address_t dst_addr, src_addr;
  static coap_uri_t uri;
  fd_set readfds;
  coap_pdu_t *request;
  //const char *server_uri = "coap://" + req->dest.addr + req->api;
  const char *server_uri = "coap://127.0.0.1/hello";

  /* Prepare coap socket*/
  coap_address_init(&src_addr);
  src_addr.addr.sin.sin_family = req->src.family;
  src_addr.addr.sin.sin_port = htons(req->src.port);
  src_addr.addr.sin.sin_addr.s_addr = inet_addr(req->src.addr);
  ctx = coap_new_context(&src_addr);
  /* The destination endpoint */
  coap_address_init(&dst_addr);
  dst_addr.addr.sin.sin_family = req->dest.family;
  dst_addr.addr.sin.sin_port = htons(req->dest.port);
  dst_addr.addr.sin.sin_addr.s_addr = inet_addr(req->dest.addr);
  /* Prepare the request */
  coap_split_uri((const uint8_t *)server_uri,strlen(server_uri), &uri);
  request = coap_new_pdu(sess);
  request->type = req->hdr.type;
  request->tid = coap_new_message_id(sess);
  request->code = req->hdr.code;
  coap_add_option(request, COAP_OPTION_URI_PATH, uri.path.length, uri.path.s);
  coap_register_response_handler(ctx, message_handler);
  coap_send(sess, request);
  // FD_ZERO(&readfds);
  // FD_SET(sess->sock, &readfds);
  // int result = select(FD_SETSIZE, &readfds, 0, 0, NULL);
  // if (result < 0)  socket error 
  // {
  //   exit(EXIT_FAILURE);
  // } else if (result > 0 && FD_ISSET(sess->sock, &readfds)) /* socket read*/
  // {
  //   coap_read(ctx,1000);
  // }

 //  coap_run_once(ctx, 0);

 //  result = EXIT_SUCCESS;
 // finish:

 //  coap_session_release(session);
 //  coap_free_context(ctx);
 //  coap_cleanup();

 //  return result;
}
