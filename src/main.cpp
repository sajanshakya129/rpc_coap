#include "clipp.h"
#include "network.hpp"
#include <iostream>
#include <map>

using namespace std;
using namespace clipp;

enum mode { ipshow, iplink, setlink, help };

int main(int argc, char *argv[]) {
  Network network;
  cout << "Program initiating" << endl;
  string IP_TYPE = "ipv4", LINK_STATE, LINK_NAME;
  mode selected = mode::help;

  auto ipShowMode = (command("ipshow").set(selected, mode::ipshow),
                     (option("-f", "--family") & value("ipv4|ipv6", IP_TYPE)) %
                         "Specify family ipv4 or ipv6, default is ipv4");
  auto ipLinkMode = (command("iplink").set(selected, mode::iplink),
                     (option("-f", "--family") & value("ipv4|ipv6", IP_TYPE)) %
                         "Specify family ipv4 or ipv6, default is ipv4");
  auto setLinkMode =
           (command("setlink").set(selected, mode::setlink),
            (required("-s", "--state") & value("up|down", LINK_STATE)) %
                "-up for activate, -down to deactivate link",
       (required("-n", "--name") & value("Name of link", LINK_NAME)) %
            "Assign name of link which you want to activate or deactivate");
  auto helpMode = (command("help").set(selected, mode::help));
  auto cli = ((ipShowMode | ipLinkMode | setLinkMode | helpMode),
              option("-v", "--version")
                  .call([] { cout << "version 0.0.1\n\n"; })
                  .doc("show version"));
  if (parse(argc, argv, cli)) {
    switch (selected) {
    case mode::ipshow:
      cout << "Command to be executed is ipshow" << endl;
      network.showIpAddr(IP_TYPE);
      break;
    case mode::iplink:
      cout << "Command to be executed is iplink" << endl;
      network.showIpLinks(IP_TYPE);
      break;
    case mode::setlink:
      network.setIpLink(LINK_NAME, LINK_STATE);
      cout << "Command to be executed is setlink" << endl;
      break;
    case mode::help:
      cout << make_man_page(cli, "irpc");
      break;
    }
  } else {
    cout << usage_lines(cli, "irpc") << '\n';
  }

  // network.ipRouteAdd(argv[1]);
  // network.ipRouteShow(argv[1]);
  // network.ipRouteDel(argv[1]);
  return 0;
}