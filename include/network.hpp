#ifndef NETWORK_HPP
#define NETWORK_HPP

class Network {

private:
  void socket(char *, int, int);
  static int data_cb_showIpAddr(const struct nlmsghdr *, void *);
  static int data_attr_cb_showIpAddr(const struct nlattr *, void *);
  static int data_cb_showIpLinks(const struct nlmsghdr *, void *);
  static int data_attr_cb_showIpLinks(const struct nlattr *, void *);

public:
  Network() {
    // constructor
  }
  void showIpAddr(char *);
  void showIpLinks(char *);
  void setIpLink(char *);

  // void ipRouteAdd(char*);
  // void ipRouteShow(char*);
  // void ipRouteDel(char*);
};

#endif