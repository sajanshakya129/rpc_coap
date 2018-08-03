#ifndef NETWORK_HPP
#define NETWORK_HPP
using namespace std;
class Network {

private:
  void socket(string, int, int, int, const char *, string );
  static int data_cb_showIpAddr(const struct nlmsghdr *, void *);
  static int data_attr_cb_showIpAddr(const struct nlattr *, void *);
  static int data_cb_showIpLinks(const struct nlmsghdr *, void *);
  static int data_attr_cb_showIpLinks(const struct nlattr *, void *);
  static int data_cb_setiplink(struct nlmsghdr *,string);

public:
  Network() {
    // constructor
  }
  void showIpAddr(string);
  void showIpLinks(string);
  void setIpLink(string,string);

  // void ipRouteAdd(char*);
  // void ipRouteShow(char*);
  // void ipRouteDel(char*);
};

#endif