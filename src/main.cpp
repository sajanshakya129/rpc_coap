#include "clipp.h"
#include "constants.hpp"
#include "iproute.hpp"
#include "network.hpp"
#include <iostream>

using namespace std;
using namespace clipp;

// enum defined for various commands

int main(int argc, char *argv[]) {

  Network network;
  IProute iproute;
  // cout << "Program initiating" << endl;
  string IP_TYPE = "ipv4", LINK_STATE, IFACE_NAME, DEST_ADDR, CIDR,
         GATE_WAY = "";
  bool msgFmt = false;

  /*CLI definations */
  mode selected = mode::help; // default
  auto ipShowMode = (command("ipshow").set(selected, mode::ipshow),
                     (option("-f", "--family") & value("ipv4|ipv6", IP_TYPE)) %
                         "Specify family ipv4 or ipv6, default is ipv4");

  auto ipLinkMode = (command("iplink").set(selected, mode::iplink));

  auto setLinkMode =
      (command("setlink").set(selected, mode::setlink),
       (required("--iface") & value("Interface Name", IFACE_NAME)) %
           "Name of Interface",
       (required("-s", "--state") & value("up|down", LINK_STATE)) %
           "-up for activate, -down to deactivate link");

  auto showIpRouteMode =
      (command("showiproute").set(selected, mode::showiproute),
       (option("-f", "--family") & value("ipv4|ipv6", IP_TYPE)) %
           "Specify family ipv4 or ipv6, default is ipv4");

  auto addIpRouteMode =
      (command("addiproute").set(selected, mode::addiproute),
       (required("--iface") & value("Interface Name", IFACE_NAME)),
       (required("--dest") &
        value("Dest. Address", DEST_ADDR) % "Destination Address"),
       (required("--cidr") &
        value("CIDR", CIDR) % "Classless Inter-Domain Routing CIDR"),
       option("--gw") &
           value("gateway address", GATE_WAY) % "Gateway" % "Gateway");

  auto removeIpRouteMode =
      (command("removeiproute").set(selected, mode::removeiproute),
       (required("--iface") & value("Interface Name", IFACE_NAME)),
       (required("--dest") &
        value("Dest. Address", DEST_ADDR) % "Destination Address"),
       option("--gw") &
           value("gateway address", GATE_WAY) % "Gateway" % "Gateway");

  auto helpMode = (command("help").set(selected, mode::help));

  auto cli = ((ipShowMode | ipLinkMode | setLinkMode | showIpRouteMode |
               addIpRouteMode | removeIpRouteMode | helpMode),
              option("--msgformat").set(msgFmt) % "shows message Format",
              option("-v", "--version")
                  .call([] { cout << "version 0.0.1\n\n"; })
                  .doc("Shows current version"));

  if (parse(argc, argv, cli)) {
    switch (selected) {
    case mode::ipshow:
      cout << "Command to be executed is ipshow" << endl;
      network.showIpAddr(IP_TYPE, msgFmt);
      break;
    case mode::iplink:
      cout << "Command to be executed is iplink" << endl;
      network.showIpLinks(IP_TYPE, msgFmt);
      break;
    case mode::setlink:
      cout << "Command to be executed is setlink" << endl;
      network.setIpLink(IFACE_NAME, LINK_STATE, msgFmt);
      break;
    case mode::showiproute:
      cout << "Command to be executed is showiproute" << endl;
      network.showIpRoute(IP_TYPE, msgFmt);
      break;
    case mode::addiproute:
      cout << "Command to be executed is addiproute" << endl;
      iproute.addIpRoute(IFACE_NAME, DEST_ADDR, CIDR, GATE_WAY);
      break;
    case mode::removeiproute:
      cout << "Command to be executed is removeIpRouteMode" << endl;
      iproute.removeIpRoute(IFACE_NAME, DEST_ADDR, GATE_WAY);
      break;
    case mode::help:
      cout << make_man_page(cli, "irpc");
      break;
    }
  } else {
    cout << usage_lines(cli, "irpc") << '\n';
  }

  return 0;
}