#include <iostream>

#include "network.h"

using namespace std;

int main(int argc, char *argv[]) {

  cout << "Program initiating" << endl;
  cout << "number of Argument:" << argc << endl;
  cout << "first argument:" << argv[0] << endl;
  cout << "Second argument:" << argv[1] << endl;
  if (argc != 2) {
    cerr << "Usage:" << argv[0] << " <ipv4|ipv6>" << endl;
    exit(EXIT_FAILURE);
  }


  Network network;
  network.ipShow(argv[1]);
  // network.ipLink(argv[1]);
  // network.ipRouteAdd(argv[1]);
  // network.ipRouteShow(argv[1]);
  // network.ipRouteDel(argv[1]);
  return 0;
}