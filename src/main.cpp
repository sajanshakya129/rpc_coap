#include <iostream>

#include "network.hpp"

using namespace std;

int main(int argc, char *argv[]) {

  cout << "Program initiating" << endl;
 
  if (argc != 2) {
    cerr << "Usage:" << argv[0] << " <ipv4|ipv6>" << endl;
    exit(EXIT_FAILURE);
  }


  Network network;
  network.showIpAddr(argv[1]);
  network.showIpLinks(argv[1]);
  // network.ipRouteAdd(argv[1]);
  // network.ipRouteShow(argv[1]);
  // network.ipRouteDel(argv[1]);
  return 0;
}