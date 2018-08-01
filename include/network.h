class Network {
private:
  void socket(char *, int);
  static int data_cb_ipshow(const struct nlmsghdr *, void *);
  static int data_attr_cb_ipshow(const struct nlattr *, void *);
  static int data_cb_iplink(const struct nlmsghdr *, void *);
  static int data_attr_cb_iplink(const struct nlattr *, void *);


public:
  Network() {
    // constructor
  }
  void ipShow(char *);
  // void ipLink(char*);

  // void ipRouteAdd(char*);
  // void ipRouteShow(char*);
  // void ipRouteDel(char*);
};