#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
// identifiers for main functions
enum cb_identifier {
  SHOW_IP_ADDR = 1,
  SHOW_IP_LINK,
  SET_IP_LINK,
  SHOW_IP_ROUTE
};

enum mode {
  ipshow,
  iplink,
  setlink,
  showiproute,
  addiproute,
  removeiproute,
  help
};

#endif