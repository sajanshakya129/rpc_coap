#ifndef NETWORK_HPP
#define NETWORK_HPP

using namespace std;

class Network {

private:
  void socket(string, int, int, int, bool msgFmt);
  void socket(int, int, int, const char *, string, bool msgFmt);
  static int data_cb_showIpAddr(const struct nlmsghdr *, void *);
  static int data_cb_showIpLinks(const struct nlmsghdr *, void *);

  static int data_attr_cb_showIpAddr(const struct nlattr *, void *);
  static int data_attr_cb_showIpLinks(const struct nlattr *, void *);

  static int data_cb_setiplink(struct nlmsghdr *, string);

public:
  void showIpAddr(string, bool msgFmt);
  void showIpLinks(string, bool msgFmt);
  void setIpLink(string, string, bool msgFmt);
  void showIpRoute(string, bool msgFmt);
};

#endif