#ifndef IPROUTE_HPP
#define IPROUTE_HPP
using namespace std;
class IProute {
private:
  static int data_attr_cb_showIpRoute(const struct nlattr *, void *);
  static int data_ipv6_attr_cb(const struct nlattr *, void *);
  static int data_ipv4_attr_cb(const struct nlattr *, void *);
  static void attributes_show_ipv6(struct nlattr *[]);
  static void attributes_show_ipv4(struct nlattr *[]);
  
public:
  IProute() {
    // constructor
  }
  static int data_cb_showIproutes(const struct nlmsghdr *, void *);
};

#endif