#include <arpa/inet.h>
#include <iostream>
#include <libmnl/libmnl.h>
#include <linux/rtnetlink.h>

#include "iproute.hpp"
#include "network.hpp"

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
    cout << "Table= " << mnl_attr_get_u32(tb[RTA_TABLE]) << endl;
  }
  if (tb[RTA_DST]) {
    struct in_addr *addr = (in_addr *)mnl_attr_get_payload(tb[RTA_DST]);
    cout << "Dst= " << inet_ntoa(*addr) << endl;
  }
  if (tb[RTA_SRC]) {
    struct in_addr *addr = (in_addr *)mnl_attr_get_payload(tb[RTA_SRC]);
    cout << "Src= " << inet_ntoa(*addr) << endl;
  }
  if (tb[RTA_OIF]) {
    cout << "OIF= " << mnl_attr_get_u32(tb[RTA_OIF]) << endl;
  }
  if (tb[RTA_FLOW]) {
    cout << "Flow= " << mnl_attr_get_u32(tb[RTA_FLOW]) << endl;
  }
  if (tb[RTA_PREFSRC]) {
    struct in_addr *addr = (in_addr *)mnl_attr_get_payload(tb[RTA_PREFSRC]);
    cout << "Prefsrc= " << inet_ntoa(*addr) << endl;
  }
  if (tb[RTA_GATEWAY]) {
    struct in_addr *addr = (in_addr *)mnl_attr_get_payload(tb[RTA_GATEWAY]);
    cout << "gw= " << inet_ntoa(*addr) << endl;
  }
  if (tb[RTA_PRIORITY]) {
    cout << "Prio= " << mnl_attr_get_u32(tb[RTA_PRIORITY]) << endl;
  }
  if (tb[RTA_METRICS]) {
    int i;
    struct nlattr *tbx[RTAX_MAX + 1] = {};

    mnl_attr_parse_nested(tb[RTA_METRICS], data_attr_cb_showIpRoute, tbx);

    for (i = 0; i < RTAX_MAX; i++) {
      if (tbx[i]) {
        cout << "Metrics[" << i << "]=" << mnl_attr_get_u32(tbx[i]) << endl;
      }
    }
  }
}

/*Function to display attributes if address family is ipv6*/
void IProute::display_attr_ipv6(struct nlattr *tb[]) {
  if (tb[RTA_TABLE]) {
    cout << "Table= " << mnl_attr_get_u32(tb[RTA_TABLE]) << endl;
  }
  if (tb[RTA_DST]) {
    struct in6_addr *addr = (in6_addr *)mnl_attr_get_payload(tb[RTA_DST]);
    cout << "Dst= " << inet6_ntoa(*addr) << endl;
  }
  if (tb[RTA_SRC]) {
    struct in6_addr *addr = (in6_addr *)mnl_attr_get_payload(tb[RTA_SRC]);
    cout << "Src= " << inet6_ntoa(*addr) << endl;
  }
  if (tb[RTA_OIF]) {
    cout << "OIF= " << mnl_attr_get_u32(tb[RTA_OIF]) << endl;
  }
  if (tb[RTA_FLOW]) {
    cout << "Flow= " << mnl_attr_get_u32(tb[RTA_FLOW]) << endl;
  }
  if (tb[RTA_PREFSRC]) {
    struct in6_addr *addr = (in6_addr *)mnl_attr_get_payload(tb[RTA_PREFSRC]);
    cout << "Prefsrc= " << inet6_ntoa(*addr) << endl;
  }
  if (tb[RTA_GATEWAY]) {
    struct in6_addr *addr = (in6_addr *)mnl_attr_get_payload(tb[RTA_GATEWAY]);
    cout << "gw= " << inet6_ntoa(*addr) << endl;
  }
  if (tb[RTA_PRIORITY]) {
    cout << "Prio= " << mnl_attr_get_u32(tb[RTA_PRIORITY]) << endl;
  }
  if (tb[RTA_METRICS]) {
    int i;
    struct nlattr *tbx[RTAX_MAX + 1] = {};

    mnl_attr_parse_nested(tb[RTA_METRICS], data_attr_cb_showIpRoute, tbx);

    for (i = 0; i < RTAX_MAX; i++) {
      if (tbx[i]) {
        cout << "Metrics[" << i << "]=" << mnl_attr_get_u32(tbx[i]) << endl;
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

//Callback function for show IP routes
int IProute::data_cb_showIproutes(const struct nlmsghdr *nlh, void *data) {
  struct nlattr *tb[RTA_MAX + 1] = {};
  struct rtmsg *rm = (rtmsg *)mnl_nlmsg_get_payload(nlh);
  cout << "Family="
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
