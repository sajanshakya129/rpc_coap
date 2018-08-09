#include <arpa/inet.h>
#include <iostream>
#include <libmnl/libmnl.h>
#include <linux/if_link.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <time.h>
#include <unistd.h>

#include "iproute.hpp"
#include "network.hpp"

using namespace std;
// identifiers for main functions
enum cb_identifier {
  SHOW_IP_ADDR = 1,
  SHOW_IP_LINK,
  SET_IP_LINK,
  SHOW_IP_ROUTE
} iden;

//-------Beginnning of Private Methods------------------------
// function used by showIpAddr, showIpLinks, showIpRoute
void Network::socket(string ipType, int nlMsgType, int flags, int cbFuncIden,
                     bool msgFmt) {
  unsigned int seq, portid;
  IProute iproute;
  char buf[MNL_SOCKET_BUFFER_SIZE];
  struct nlmsghdr *nlh;

  nlh = mnl_nlmsg_put_header(buf);
  nlh->nlmsg_type = nlMsgType;
  nlh->nlmsg_flags = flags;
  nlh->nlmsg_seq = seq = time(NULL);

  struct rtgenmsg *rt;
  rt = (rtgenmsg *)mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtgenmsg)); //
  if (ipType.compare("ipv4") == 0)
    rt->rtgen_family = AF_INET; // setting IPv4
  else if (ipType.compare("ipv6") == 0)
    rt->rtgen_family = AF_INET6; // setting IPv6

  struct mnl_socket *nl;
  nl = mnl_socket_open(NETLINK_ROUTE);
  if (nl == NULL) {
    perror("mnl_socket_open");
    exit(EXIT_FAILURE);
  }

  if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
    perror("mnl_socket_bind"); // error if socket binding failed
    exit(EXIT_FAILURE);
  }

  portid = mnl_socket_get_portid(nl);
  if (msgFmt) // if --msgFormat is used in command
    mnl_nlmsg_fprintf(stdout, nlh, nlh->nlmsg_len, sizeof(struct ifinfomsg));

  if (mnl_socket_sendto(nl, nlh, nlh->nlmsg_len) < 0) {
    perror("mnl_socket_sendto");
    exit(EXIT_FAILURE);
  }

  int ret;
  ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
  mnl_cb_t cb_func;
  while (ret > 0) {
    switch (cbFuncIden) { // switching callback functions
    case SHOW_IP_ADDR:
      cb_func = &data_cb_showIpAddr;
      break;
    case SHOW_IP_LINK:
      cb_func = &data_cb_showIpLinks;
      break;
    case SHOW_IP_ROUTE:
      cb_func = &iproute.data_cb_showIproutes;
    }
    ret = mnl_cb_run(buf, ret, seq, portid, cb_func, NULL);
    if (ret <= MNL_CB_STOP)
      break;
    ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
  }

  if (ret == -1) {
    perror("error");
    exit(EXIT_FAILURE);
  }

  mnl_socket_close(nl);
}
/** Function overloaded used by setIpLink function
    TODO:break socket function into small functions, find common between two
    socket functions overloaded
    **/
void Network::socket(int nlMsgType, int reqFlags,const char *devName, string linkState, bool msgFmt) {
  struct mnl_socket *nl;
  unsigned int seq, portid, change = 0, flags = 0;

  if (linkState.compare("up") == 0) {
    change |= IFF_UP;
    flags |= IFF_UP;
  } else if (linkState.compare("down") == 0) {
    change |= IFF_UP;
    flags &= ~IFF_UP;
  } else {
    cout << stderr << devName << " is not `up' nor `down'\n" << endl;
    exit(EXIT_FAILURE);
  }

  struct nlmsghdr *nlh;
  char buf[MNL_SOCKET_BUFFER_SIZE];

  nlh = mnl_nlmsg_put_header(buf);
  nlh->nlmsg_type = nlMsgType;
  nlh->nlmsg_flags = reqFlags;
  nlh->nlmsg_seq = seq = time(NULL);

  struct ifinfomsg *ifm;

  ifm = (ifinfomsg *)mnl_nlmsg_put_extra_header(nlh, sizeof(*ifm));
  ifm->ifi_family = AF_UNSPEC;
  ifm->ifi_change = change;
  ifm->ifi_flags = flags;

  mnl_attr_put_str(nlh, IFLA_IFNAME, devName);
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
  if (msgFmt) // if --msgFormat is used in command
    mnl_nlmsg_fprintf(stdout, nlh, nlh->nlmsg_len, sizeof(struct ifinfomsg));

  if (mnl_socket_sendto(nl, nlh, nlh->nlmsg_len) < 0) {
    perror("mnl_socket_sendto");
    exit(EXIT_FAILURE);
  }

  int ret;

  ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
  if (ret == -1) {
    perror("mnl_socket_recvfrom");
    exit(EXIT_FAILURE);
  }

  ret = mnl_cb_run(buf, ret, seq, portid, NULL, NULL);
  if (ret == -1) {
    cout << "Note:You might need to have Administrative privilege to execute "
            "this "
            "command. Try with \'sudo\'"
         << endl;
    perror("Operation Error");
    exit(EXIT_FAILURE);
  } else if (ret == 0) {
    cout << "SUCCESS:State of " << devName << " has been changed to "
         << linkState << endl;
  }

  mnl_socket_close(nl);
}

/*Function to validate attribute of callback responses of showIpAddr*/
int Network::data_attr_cb_showIpAddr(const struct nlattr *attr, void *data) {
  const struct nlattr **tb = (const nlattr **)data;
  int type = mnl_attr_get_type(attr);

  if (mnl_attr_type_valid(attr, IFA_MAX) < 0)
    return MNL_CB_OK;

  switch (type) {
  case IFA_ADDRESS:
    if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0) {
      perror("mnl_attr_validate");
      return MNL_CB_ERROR;
    }
    break;
  }
  tb[type] = attr;
  return MNL_CB_OK;
}

/*Function to validate attribute of callback responses of showIpLinks*/
int Network::data_attr_cb_showIpLinks(const struct nlattr *attr, void *data) {
  const struct nlattr **tb = (const nlattr **)data;
  int type = mnl_attr_get_type(attr);

  if (mnl_attr_type_valid(attr, IFLA_MAX) < 0)
    return MNL_CB_OK;

  switch (type) {
  case IFLA_ADDRESS:
    if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0) {
      perror("mnl_attr_validate");
      return MNL_CB_ERROR;
    }
    break;
  case IFLA_MTU:
    if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
      perror("mnl_attr_validate");
      return MNL_CB_ERROR;
    }
    break;
  case IFLA_IFNAME:
    if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0) {
      perror("mnl_attr_validate");
      return MNL_CB_ERROR;
    }
    break;
  }
  tb[type] = attr;
  return MNL_CB_OK;
}

/*Call back function for showing Ip address
  to be passed to mnl_cb_run() inside socket*/
int Network::data_cb_showIpAddr(const struct nlmsghdr *nlh, void *data) {
  char ifrn_name[IFNAMSIZ];
  struct nlattr *tb[IFA_MAX + 1] = {};
  struct ifaddrmsg *ifa = (ifaddrmsg *)mnl_nlmsg_get_payload(nlh);

  mnl_attr_parse(nlh, sizeof(*ifa), data_attr_cb_showIpAddr, tb);
  cout << "Index= " << ifa->ifa_index << endl;
  cout << "Name= " << if_indextoname(ifa->ifa_index, ifrn_name) << endl;
  cout << "Family= " << ((ifa->ifa_family == AF_INET) ? "IPv4" : "IPv6")
       << endl;
  cout << "IP address= ";

  if (tb[IFA_ADDRESS]) {
    void *addr = mnl_attr_get_payload(tb[IFA_ADDRESS]);
    char out[INET6_ADDRSTRLEN];

    if (inet_ntop(ifa->ifa_family, addr, out, sizeof(out)))
      cout << out << "/" << wchar_t(ifa->ifa_prefixlen) << endl;
  }
  cout << "Scope= ";
  switch (ifa->ifa_scope) {
  case 0:
    cout << "Global " << endl;
    break;
  case 200:
    cout << "Site " << endl;
    break;
  case 253:
    cout << "Link " << endl;
    break;
  case 254:
    cout << "Host " << endl;
    break;
  case 255:
    cout << "Nowhere " << endl;
    break;
  default:
    cout << ifa->ifa_scope << endl;
    break;
  }

  cout << "=================================\n" << endl;
  return MNL_CB_OK;
}

/*Call back function for showing IP Links
  to be passed to mnl_cb_run() inside socket*/
int Network::data_cb_showIpLinks(const struct nlmsghdr *nlh, void *data) {
  struct nlattr *tb[IFLA_MAX + 1] = {};
  struct ifinfomsg *ifm = (ifinfomsg *)mnl_nlmsg_get_payload(nlh);
  cout << "Index=" << ifm->ifi_index << endl;
  cout << "Type=" << ifm->ifi_type << endl;
  cout << "Flags=" << ifm->ifi_flags << endl;
  cout << "Status=";
  if (ifm->ifi_flags & IFF_RUNNING)
    cout << "[RUNNING]" << endl;
  else
    cout << "[NOT RUNNING]" << endl;

  mnl_attr_parse(nlh, sizeof(*ifm), data_attr_cb_showIpLinks, tb);
  if (tb[IFLA_MTU]) {
    cout << "MTU=" << mnl_attr_get_u32(tb[IFLA_MTU]) << endl;
  }
  if (tb[IFLA_IFNAME]) {
    cout << "Name=" << mnl_attr_get_str(tb[IFLA_IFNAME]) << endl;
  }
  if (tb[IFLA_ADDRESS]) {
    uint8_t *hwaddr = (uint8_t *)mnl_attr_get_payload(tb[IFLA_ADDRESS]);
    int i;

    cout << "MAC Address=";
    for (i = 0; i < mnl_attr_get_payload_len(tb[IFLA_ADDRESS]); i++) {
      printf("%.2x", hwaddr[i] & 0xff);
      if (i + 1 != mnl_attr_get_payload_len(tb[IFLA_ADDRESS]))
        cout << ":";
    }
  }
  cout << "\n" << endl;
  cout << "=================================" << endl;
  return MNL_CB_OK;
}

//------End of Private  Functions------------------

//-----Start of Public Functions-------------------

void Network::showIpAddr(string ipType, bool msgFmt) {
  socket(ipType, RTM_GETADDR, NLM_F_REQUEST | NLM_F_DUMP, SHOW_IP_ADDR, msgFmt);
}

void Network::showIpLinks(string ipType, bool msgFmt) {
  socket(ipType, RTM_GETLINK, NLM_F_REQUEST | NLM_F_DUMP, SHOW_IP_LINK, msgFmt);
}

void Network::setIpLink(string linkName, string linkState, bool msgFmt) {
  const char *lName = linkName.c_str();
  socket(RTM_NEWLINK, NLM_F_REQUEST | NLM_F_ACK, lName, linkState,
         msgFmt);
}

void Network::showIpRoute(string ipType, bool msgFmt) {
  socket(ipType, RTM_GETROUTE, NLM_F_REQUEST | NLM_F_DUMP, SHOW_IP_ROUTE,
         msgFmt);
}

//----End of Public Functions--------------------
