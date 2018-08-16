#ifndef COAP_CLIENT_HPP
#define COAP_CLIENT_HPP

#include "coap.h"
using namespace std;

class CoapClient {
private:
  static void message_handler(struct coap_context_t *, coap_session_t * , coap_pdu_t *,
                              coap_pdu_t *, const coap_tid_t);
public:
  void sendCOAPRequest(struct coapRequest *);
};

#endif