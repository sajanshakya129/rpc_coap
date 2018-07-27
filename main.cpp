#include<iostream>
#include<string.h>


#include <libmnl/libmnl.h>
#include <linux/rtnetlink.h>




using namespace std;

class Network {
private:
	void socket(char*,int);

public:
	Network() {
		cout << "hello from constructor" << endl;
	}
	void ipShow(char*);
	// void ipRouteAdd(char*);
	// void ipRouteShow(char*);
	// void ipRouteDel(char*);
};

void Network::socket(char *ipType, int nlMsgType) {
	unsigned int seq, portid;

	char buf[MNL_SOCKET_BUFFER_SIZE]; //buffer
	struct nlmsghdr *nlh; //network link message header
	struct rtgenmsg *rt;
	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type	= nlMsgType;//get information about Ip address
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;// flag for request
	nlh->nlmsg_seq = seq = time(NULL);
	
	
	rt = mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtgenmsg));//
	if (strcmp(ipType, "ipv4") == 0)
		rt->rtgen_family = AF_INET;// setting IPv4
	else if (strcmp(ipType, "ipv6") == 0)
		rt->rtgen_family = AF_INET6;// setting IPv6

	struct mnl_socket *nl; //socket pointer nl
	
	nl = mnl_socket_open(NETLINK_ROUTE);//socket opening
	if (nl == NULL) {
		perror("mnl_socket_open");
		exit(EXIT_FAILURE);
	}

	if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) { //binding socket passing socket pointer  mnl_socket_bind (struct mnl_socket *nl, unsigned int groups, pid_t pid)
		perror("mnl_socket_bind");//error if socket binding failed
		exit(EXIT_FAILURE);
	}
	portid = mnl_socket_get_portid(nl);//obtain Netlink PortID from netlink socket

	if (mnl_socket_sendto(nl, nlh, nlh->nlmsg_len) < 0) {//socket pointer, header, header length
		perror("mnl_socket_sendto");
		exit(EXIT_FAILURE);
	}

	int ret;
	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, seq, portid, data_cb, NULL);
		if (ret <= MNL_CB_STOP)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}

	if (ret == -1) {
		perror("error");
		exit(EXIT_FAILURE);
	}

	mnl_socket_close(nl);
}

void Network::ipShow(char *ipType) {
	cout << "inside ip show" << endl;
	int msgtype=RTM_GETADDR;
	socket(ipType,msgtype);//accessing private function
}

// void Network::ipRouteAdd(char *ipType) {
// 	cout << "inside ipRouteAdd" << endl;
// 	int msgtype=RTM_GETADDR;
// 	socket(ipType,msgtype);
// }

// void Network::ipRouteShow(char *ipType) {
// 	cout << "inside ipRouteShow" << endl;
// 	int msgtype=RTM_GETADDR;
// 	socket(ipType,msgtype);
// }

// void Network::ipRouteDel(char *ipType) {
// 	cout << "inside ipRouteDel" << endl;
// 	int msgtype=RTM_GETADDR;
// 	socket(ipType,msgtype);
// }




int main(int argc, char *argv[]) {
	cout << "Program initiating" << endl;
	cout << "number of Argument:" << argc << endl;
	cout << "first argument:" << argv[0] << endl;
	cout<<"Second argument:"<<argv[1]<<endl;
	if (argc != 2) {
		cerr<<"Usage:"<<argv[0]<<" <ipv4|ipv6>" <<endl;
		exit(EXIT_FAILURE);
	}


	Network network;
	// network.ipShow(argv[1]);
	// network.ipRouteAdd(argv[1]);
	// network.ipRouteShow(argv[1]);
	// network.ipRouteDel(argv[1]);
	return 0;
}