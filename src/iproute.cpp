#include <arpa/inet.h>
#include <iostream>
#include <libmnl/libmnl.h>
#include <linux/if_link.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <time.h>
#include <unistd.h>

#include "iproute.hpp"

using namespace std;
/*Function to convert IPv6 addresses from binary to text form*/
static const char *inet6_ntoa(struct in6_addr in6) {
  static char buf[INET6_ADDRSTRLEN];
  return inet_ntop(AF_INET6, &in6.s6_addr, buf, sizeof(buf));
}

/*Function to validate attribute of callback responses of showIpRoute*/
int IProute::data_attr_cb_showIpRoute(const struct nlattr *attr, void *data) {
  const struct nlattr **tb = (const nlattr **)data;

  if (mnl_attr_type_valid(attr, RTAX_MAX) < 0)
    return MNL_CB_OK;

  if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
    perror("mnl_attr_validate");
    return MNL_CB_ERROR;
  }

  tb[mnl_attr_get_type(attr)] = attr;
  return MNL_CB_OK;
}

/*Function to display attributes if address family is ipv4*/
void IProute::display_attr_ipv4(struct nlattr *tb[]) {
  if (tb[RTA_TABLE]) {
    cout << "\tTable= " << mnl_attr_get_u32(tb[RTA_TABLE]);
  }
  if (tb[RTA_DST]) {
    struct in_addr *addr = (in_addr *)mnl_attr_get_payload(tb[RTA_DST]);
    cout << "\tDst= " << inet_ntoa(*addr);
  }
  if (tb[RTA_SRC]) {
    struct in_addr *addr = (in_addr *)mnl_attr_get_payload(tb[RTA_SRC]);
    cout << "\tSrc= " << inet_ntoa(*addr);
  }
  if (tb[RTA_OIF]) {
    cout << "\tOIF= " << mnl_attr_get_u32(tb[RTA_OIF]);
  }
  if (tb[RTA_FLOW]) {
    cout << "\tFlow= " << mnl_attr_get_u32(tb[RTA_FLOW]);
  }
  if (tb[RTA_PREFSRC]) {
    struct in_addr *addr = (in_addr *)mnl_attr_get_payload(tb[RTA_PREFSRC]);
    cout << "\tPrefsrc= " << inet_ntoa(*addr);
  }
  if (tb[RTA_GATEWAY]) {
    struct in_addr *addr = (in_addr *)mnl_attr_get_payload(tb[RTA_GATEWAY]);
    cout << "\tgw= " << inet_ntoa(*addr);
  }
  if (tb[RTA_PRIORITY]) {
    cout << "\tPrio= " << mnl_attr_get_u32(tb[RTA_PRIORITY]);
  }
  if (tb[RTA_METRICS]) {
    int i;
    struct nlattr *tbx[RTAX_MAX + 1] = {};

    mnl_attr_parse_nested(tb[RTA_METRICS], data_attr_cb_showIpRoute, tbx);

    for (i = 0; i < RTAX_MAX; i++) {
      if (tbx[i]) {
        cout << "\tMetrics[" << i << "]=" << mnl_attr_get_u32(tbx[i]);
      }
    }
  }
}

/*Function to display attributes if address family is ipv6*/
void IProute::display_attr_ipv6(struct nlattr *tb[]) {
  if (tb[RTA_TABLE]) {
    cout << "\tTable= " << mnl_attr_get_u32(tb[RTA_TABLE]);
  }
  if (tb[RTA_DST]) {
    struct in6_addr *addr = (in6_addr *)mnl_attr_get_payload(tb[RTA_DST]);
    cout << "\tDst= " << inet6_ntoa(*addr);
  }
  if (tb[RTA_SRC]) {
    struct in6_addr *addr = (in6_addr *)mnl_attr_get_payload(tb[RTA_SRC]);
    cout << "\tSrc= " << inet6_ntoa(*addr);
  }
  if (tb[RTA_OIF]) {
    cout << "\tOIF= " << mnl_attr_get_u32(tb[RTA_OIF]);
  }
  if (tb[RTA_FLOW]) {
    cout << "\tFlow= " << mnl_attr_get_u32(tb[RTA_FLOW]);
  }
  if (tb[RTA_PREFSRC]) {
    struct in6_addr *addr = (in6_addr *)mnl_attr_get_payload(tb[RTA_PREFSRC]);
    cout << "\tPrefsrc= " << inet6_ntoa(*addr);
  }
  if (tb[RTA_GATEWAY]) {
    struct in6_addr *addr = (in6_addr *)mnl_attr_get_payload(tb[RTA_GATEWAY]);
    cout << "\tgw= " << inet6_ntoa(*addr);
  }
  if (tb[RTA_PRIORITY]) {
    cout << "\tPrio= " << mnl_attr_get_u32(tb[RTA_PRIORITY]);
  }
  if (tb[RTA_METRICS]) {
    int i;
    struct nlattr *tbx[RTAX_MAX + 1] = {};

    mnl_attr_parse_nested(tb[RTA_METRICS], data_attr_cb_showIpRoute, tbx);

    for (i = 0; i < RTAX_MAX; i++) {
      if (tbx[i]) {
        cout << "\tMetrics[" << i << "]=" << mnl_attr_get_u32(tbx[i]);
      }
    }
  }
}

/*Function to validate attribute of ipv4 callback data*/
int IProute::data_ipv4_attr_cb(const struct nlattr *attr, void *data) {
  const struct nlattr **tb = (const nlattr **)data;
  int type = mnl_attr_get_type(attr);

  if (mnl_attr_type_valid(attr, RTA_MAX) < 0)
    return MNL_CB_OK;

  switch (type) {
  case RTA_TABLE:
  case RTA_DST:
  case RTA_SRC:
  case RTA_OIF:
  case RTA_FLOW:
  case RTA_PREFSRC:
  case RTA_GATEWAY:
  case RTA_PRIORITY:
    if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
      perror("mnl_attr_validate");
      return MNL_CB_ERROR;
    }
    break;
  case RTA_METRICS:
    if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
      perror("mnl_attr_validate");
      return MNL_CB_ERROR;
    }
    break;
  }
  tb[type] = attr;
  return MNL_CB_OK;
}

/*Function to validate attribute of ipv6 callback data*/
int IProute::data_ipv6_attr_cb(const struct nlattr *attr, void *data) {
  const struct nlattr **tb = (const nlattr **)data;
  int type = mnl_attr_get_type(attr);

  if (mnl_attr_type_valid(attr, RTA_MAX) < 0)
    return MNL_CB_OK;

  switch (type) {
  case RTA_TABLE:
  case RTA_OIF:
  case RTA_FLOW:
  case RTA_PRIORITY:
    if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
      perror("mnl_attr_validate");
      return MNL_CB_ERROR;
    }
    break;
  case RTA_DST:
  case RTA_SRC:
  case RTA_PREFSRC:
  case RTA_GATEWAY:
    if (mnl_attr_validate2(attr, MNL_TYPE_BINARY, sizeof(struct in6_addr)) <
        0) {
      perror("mnl_attr_validate2");
      return MNL_CB_ERROR;
    }
    break;
  case RTA_METRICS:
    if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0) {
      perror("mnl_attr_validate");
      return MNL_CB_ERROR;
    }
    break;
  }
  tb[type] = attr;
  return MNL_CB_OK;
}

// Callback function for show IP routes
int IProute::data_cb_showIproutes(const struct nlmsghdr *nlh, void *data) {
  struct nlattr *tb[RTA_MAX + 1] = {};
  struct rtmsg *rm = (rtmsg *)mnl_nlmsg_get_payload(nlh);
  cout << "\tFamily="
       << ((rm->rtm_family == AF_INET)
               ? "IPv4"
               : "IPv6"); // protocol family = AF_INET | AF_INET6
  cout << "\tDst_len="
       << (int)rm->rtm_dst_len; // destination CIDR, eg. 24 or 32 for IPv4
  cout << "\tSrc_len=" << (int)rm->rtm_src_len; // source CIDR
  cout << "\tTOS= " << (int)rm->rtm_tos;        // type of service (TOS)
  cout << "\tTable= " << (int)rm->rtm_table;
  cout << "\tType= " << (int)rm->rtm_type;
  cout << "\tScope= " << (int)rm->rtm_scope;
  cout << "\tProto= " << (int)rm->rtm_protocol;
  cout << "\tFlags= " << (int)rm->rtm_flags;

  switch (rm->rtm_family) {
  case AF_INET:
    mnl_attr_parse(nlh, sizeof(*rm), data_ipv4_attr_cb, tb);
    display_attr_ipv4(tb);
    break;
  case AF_INET6:
    mnl_attr_parse(nlh, sizeof(*rm), data_ipv6_attr_cb, tb);
    display_attr_ipv6(tb);
    break;
  }

  cout << endl;
  return MNL_CB_OK;
}



void IProute::addIpRoute(string ifName, string destAddr, string Cidr,string gateWay) {
  struct mnl_socket *nl;
  char buf[MNL_SOCKET_BUFFER_SIZE];
  struct nlmsghdr *nlh;
  struct rtmsg *rtm;
  uint32_t prefix, seq, portid;
  union {
    in_addr_t ip;
    struct in6_addr ip6;
  } dst;
  union {
    in_addr_t ip;
    struct in6_addr ip6;
  } gw;
  int iface, ret, family = AF_INET;
  const char *if_name = ifName.c_str();
  const char *dest_addr = destAddr.c_str();
  const char *gate_way = gateWay.c_str();
  const char *cidr = Cidr.c_str();

  // cout<<"if_name="<<if_name<<endl;
  // cout<<"dest_addr="<<dest_addr<<endl;
  // cout<<"gate_way="<<gate_way<<endl;
  // cout<<"cidr="<<cidr<<endl;

  iface = if_nametoindex(if_name);
  if (iface == 0) {
    perror("if_nametoindex");
    exit(EXIT_FAILURE);
  }

  if (!inet_pton(AF_INET, dest_addr, &dst)) {
    if (!inet_pton(AF_INET6, dest_addr, &dst)) {
      perror("inet_pton");
      exit(EXIT_FAILURE);
    }
    family = AF_INET6;
  }

  if (sscanf(cidr, "%u", &prefix) == 0) {
    perror("sscanf");
    exit(EXIT_FAILURE);
  }

  if (!(gateWay.empty()) && !inet_pton(family, gate_way, &gw)) {
    perror("inet_pton");
    exit(EXIT_FAILURE);
  }

  nlh = mnl_nlmsg_put_header(buf);
  nlh->nlmsg_type = RTM_NEWROUTE;
  nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_ACK;
  nlh->nlmsg_seq = seq = time(NULL);
  rtm = (rtmsg *)mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtmsg));
  rtm->rtm_family = family;
  rtm->rtm_dst_len = prefix;
  rtm->rtm_src_len = 0;
  rtm->rtm_tos = 0;
  rtm->rtm_protocol = RTPROT_STATIC;
  rtm->rtm_table = RT_TABLE_MAIN;
  rtm->rtm_type = RTN_UNICAST;
  rtm->rtm_scope = (gateWay.empty()) ? RT_SCOPE_LINK : RT_SCOPE_UNIVERSE;
  rtm->rtm_flags = 0;

  if (family == AF_INET)
    mnl_attr_put_u32(nlh, RTA_DST, dst.ip);
  else
    mnl_attr_put(nlh, RTA_DST, sizeof(struct in6_addr), &dst);

  mnl_attr_put_u32(nlh, RTA_OIF, iface);
  if (!(gateWay.empty())) {
    if (family == AF_INET)
      mnl_attr_put_u32(nlh, RTA_GATEWAY, gw.ip);
    else {
      mnl_attr_put(nlh, RTA_GATEWAY, sizeof(struct in6_addr), &gw.ip6);
    }
  }

  nl = mnl_socket_open(NETLINK_ROUTE);
  if (nl == NULL) {
    perror("mnl_socket_open");
    exit(EXIT_FAILURE);
  }

  if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
    perror("mnl_socket_bind");
    exit(EXIT_FAILURE);
  }
  portid = mnl_socket_get_portid(nl);

  if (mnl_socket_sendto(nl, nlh, nlh->nlmsg_len) < 0) {
    perror("mnl_socket_sendto");
    exit(EXIT_FAILURE);
  }

  ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
  if (ret < 0) {
    perror("mnl_socket_recvfrom");
    exit(EXIT_FAILURE);
  }

  ret = mnl_cb_run(buf, ret, seq, portid, NULL, NULL);
  if (ret < 0) {
    perror("ERROR:");
    exit(EXIT_FAILURE);
  }else if(ret==0){
  	cout<<"SUCCESS:Route has been added"<<endl;
  }

  mnl_socket_close(nl);
}


void IProute::removeIpRoute(string ifName,string destAddr,string gateWay){

 struct mnl_socket *nl;
  char buf[MNL_SOCKET_BUFFER_SIZE];
  struct nlmsghdr *nlh;
  struct rtmsg *rtm;
  uint32_t prefix, seq, portid;
  union {
    in_addr_t ip;
    struct in6_addr ip6;
  } dst;
  union {
    in_addr_t ip;
    struct in6_addr ip6;
  } gw;
  int iface, ret, family = AF_INET;
  const char *if_name = ifName.c_str();
  const char *dest_addr = destAddr.c_str();
  const char *gate_way = gateWay.c_str();
  // const char *cidr = Cidr.c_str();


  nlh = mnl_nlmsg_put_header(buf);
  nlh->nlmsg_type = RTM_DELROUTE;
  nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
  nlh->nlmsg_seq = seq = time(NULL);
  rtm = (rtmsg *)mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtmsg));
  rtm->rtm_family = family;
  rtm->rtm_dst_len = prefix;
  rtm->rtm_src_len = 0;
  rtm->rtm_tos = 0;
  rtm->rtm_protocol = RTPROT_STATIC;
  rtm->rtm_table = RT_TABLE_MAIN;
  rtm->rtm_type = RTN_UNICAST;
  rtm->rtm_scope = RT_SCOPE_UNIVERSE;
  rtm->rtm_flags = 0;

  if (family == AF_INET)
    mnl_attr_put_u32(nlh, RTA_DST, dst.ip);
  else
    mnl_attr_put(nlh, RTA_DST, sizeof(struct in6_addr), &dst);

  mnl_attr_put_u32(nlh, RTA_OIF, iface);
  if (!(gateWay.empty())) {
    if (family == AF_INET)
      mnl_attr_put_u32(nlh, RTA_GATEWAY, gw.ip);
    else {
      mnl_attr_put(nlh, RTA_GATEWAY, sizeof(struct in6_addr), &gw.ip6);
    }
  }

  nl = mnl_socket_open(NETLINK_ROUTE);
  if (nl == NULL) {
    perror("mnl_socket_open");
    exit(EXIT_FAILURE);
  }

  if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
    perror("mnl_socket_bind");
    exit(EXIT_FAILURE);
  }
  portid = mnl_socket_get_portid(nl);

  if (mnl_socket_sendto(nl, nlh, nlh->nlmsg_len) < 0) {
    perror("mnl_socket_sendto");
    exit(EXIT_FAILURE);
  }

  ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
  if (ret < 0) {
    perror("mnl_socket_recvfrom");
    exit(EXIT_FAILURE);
  }

  ret = mnl_cb_run(buf, ret, seq, portid, NULL, NULL);
  if (ret < 0) {
    perror("ERROR:");
    exit(EXIT_FAILURE);
  }else if(ret==0){
  	cout<<"SUCCESS:Route has been deleted"<<endl;
  }

  mnl_socket_close(nl);	
}