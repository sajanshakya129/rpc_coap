#ifndef NETWORK_HPP
#define NETWORK_HPP
using namespace std;
class Network {

private:
  void socket(string, int, int, int, bool msgFmt);
  void socket(int, int, int, const char *, string, bool msgFmt);
  static int data_cb_showIpAddr(const struct nlmsghdr *, void *);
  static int data_cb_showIpLinks(const struct nlmsghdr *, void *);
  static int data_cb_showIproutes(const struct nlmsghdr *, void *);

  static int data_attr_cb_showIpAddr(const struct nlattr *, void *);
  static int data_attr_cb_showIpLinks(const struct nlattr *, void *);
  static int data_attr_cb_showIpRoute(const struct nlattr *, void *);
  static int data_ipv6_attr_cb(const struct nlattr *, void *);
  static int data_ipv4_attr_cb(const struct nlattr *, void *);
  
  static int data_cb_setiplink(struct nlmsghdr *, string);
  static void attributes_show_ipv6(struct nlattr *[]);
  static void attributes_show_ipv4(struct nlattr *[]);
  
public:
  Network() {
    // constructor
  }
  void showIpAddr(string, bool msgFmt);
  void showIpLinks(string, bool msgFmt);
  void setIpLink(string, string, bool msgFmt);
  void showIpRoute(string, bool msgFmt);

  // void ipRouteAdd(char*);
  // void ipRouteDel(char*);
};

#endif