#include<iostream>
#include <cstring>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>
#include <net/if.h>

using namespace std;

#define BUFLEN		4096

#define for_each_nlmsg(n, buf, len)					\
	for (n = (struct nlmsghdr*)buf;					\
	     NLMSG_OK(n, (uint32_t)len) && n->nlmsg_type != NLMSG_DONE;	\
	     n = NLMSG_NEXT(n, len))

#define for_each_rattr(n, buf, len)					\
	for (n = (struct rtattr*)buf; RTA_OK(n, len); n = RTA_NEXT(n, len))

class Network {
private:
	inline char *ntop(int, void*);
	inline void check(int);
	int get_ip(int, struct sockaddr_nl *, int);
	int get_msg(int, struct sockaddr_nl *, void *, size_t);
	uint32_t parse_nl_msg(void *, size_t);
	int parse_ifa_msg(struct ifaddrmsg *, void *, size_t);

public:
	Network() {
		cout << "hello from constructor" << endl;
	}
	
	void ipShow(char*);
	// void ipRouteShow(char*);
	// void ipRouteDel(char*);
};

//-----start of PRIVATE function------------------------------------------------------------------------------------------------------------------
inline void Network::check(int val)
{
	if (val < 0) {
		cout<<"check error:"<<std::strerror(errno)<<endl;
		exit(1);
	}
}

inline char* Network::ntop(int domain, void *buf)
{
	static char ip[INET6_ADDRSTRLEN];
	inet_ntop(domain, buf, ip, INET6_ADDRSTRLEN);
	return ip;
}

int Network::get_ip(int fd, struct sockaddr_nl *sa, int domain)
{
	char buf[BUFLEN];

	memset(buf, 0, BUFLEN);

	// assemble the message according to the netlink protocol
	struct nlmsghdr *nl;
	nl = (struct nlmsghdr*)buf;
	nl->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
	nl->nlmsg_type = RTM_GETADDR;
	nl->nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;

	struct ifaddrmsg *ifa;
	ifa = (struct ifaddrmsg*)NLMSG_DATA(nl);
	ifa->ifa_family = domain; // we only get ipv4 address here

	// prepare struct msghdr for sending.
	struct iovec iov = { nl, nl->nlmsg_len };
	struct msghdr msg = { sa, sizeof(*sa), &iov, 1, NULL, 0, 0 };

	// send netlink message to kernel.
	int r = sendmsg(fd, &msg, 0);
	return (r < 0) ? -1 : 0;
}

int Network::get_link(int fd, struct sockaddr_nl *sa, int domain)
{
	char buf[BUFLEN];

	memset(buf, 0, BUFLEN);

	// assemble the message according to the netlink protocol
	struct nlmsghdr *nl;
	nl = (struct nlmsghdr*)buf;
	nl->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
	nl->nlmsg_type = RTM_GETADDR;
	nl->nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;

	struct ifaddrmsg *ifa;
	ifa = (struct ifaddrmsg*)NLMSG_DATA(nl);
	ifa->ifa_family = domain; // we only get ipv4 address here

	// prepare struct msghdr for sending.
	struct iovec iov = { nl, nl->nlmsg_len };
	struct msghdr msg = { sa, sizeof(*sa), &iov, 1, NULL, 0, 0 };

	// send netlink message to kernel.
	int r = sendmsg(fd, &msg, 0);
	return (r < 0) ? -1 : 0;
}

int Network::get_msg(int fd, struct sockaddr_nl *sa, void *buf, size_t len)
{
	struct iovec iov;
	struct msghdr msg;
	iov.iov_base = buf;
	iov.iov_len = len;

	memset(&msg, 0, sizeof(msg));
	msg.msg_name = sa;
	msg.msg_namelen = sizeof(*sa);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	return recvmsg(fd, &msg, 0);
}

int Network::parse_ifa_msg(struct ifaddrmsg *ifa, void *buf, size_t len)
{
	char ifname[IF_NAMESIZE];
	cout<<"=================================="<<endl;
	cout<<"Index--> "<<ifa->ifa_index<<endl;
	cout<<"Family--> "<< ((ifa->ifa_family == AF_INET) ? "IPv4" :"IPv6")<<endl;
	cout<<"Name--> "<< if_indextoname(ifa->ifa_index, ifname)<<endl;
	cout<<"Prefix length-->  "<<wchar_t(ifa->ifa_prefixlen)<<endl;

	struct rtattr *rta = NULL;
	int fa = ifa->ifa_family;
	for_each_rattr(rta, buf, len) {
		if (rta->rta_type == IFA_ADDRESS) {
			cout<<"IP Address-->  "<<ntop(fa, RTA_DATA(rta))<<endl;
		}

		if (rta->rta_type == IFA_LOCAL) {
			cout<<"Local Address-->  "<< ntop(fa, RTA_DATA(rta))<<endl;
		}

		if (rta->rta_type == IFA_BROADCAST) {
			cout<<"Broadcast Address-->  "<<ntop(fa, RTA_DATA(rta))<<endl;
		}
	}

	return 0;
}

uint32_t Network::parse_nl_msg(void *buf, size_t len)
{
	struct nlmsghdr *nl = NULL;
	for_each_nlmsg(nl, buf, len) {
		if (nl->nlmsg_type == NLMSG_ERROR) {
			cout<<"error"<<endl;
			return -1;
		}

		if (nl->nlmsg_type == RTM_NEWADDR) {
			struct ifaddrmsg *ifa;
			ifa = (struct ifaddrmsg*)NLMSG_DATA(nl);
			parse_ifa_msg(ifa, IFA_RTA(ifa), IFA_PAYLOAD(nl));
			continue;
		}


	}
	return nl->nlmsg_type;
}
//--------------------------------------end of Private Functions----------------------------------------------------------
//--------------------------------------Start of Public Functions---------------------------------------------------------

void Network::ipShow(char *ipType){
	int fd = 0, len = 0;
	char ip;//inet or inet6
	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	check(fd);
 
	struct sockaddr_nl sa;
	memset(&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;

	if (strcmp(ipType, "ipv4") == 0)
		ip = AF_INET;// setting IPv4
	else if (strcmp(ipType, "ipv6") == 0)
		ip = AF_INET6;// setting IPv6

	
	len = get_ip(fd, &sa, ip);
	check(len);

	char buf[BUFLEN];
	uint32_t nl_msg_type;
	do {
		len = get_msg(fd, &sa, buf, BUFLEN);
		check(len);

		nl_msg_type = parse_nl_msg(buf, len);
	} while (nl_msg_type != NLMSG_DONE && nl_msg_type != NLMSG_ERROR);
}

//-----------------------------------------------------End of public functions-------------------------------------------

int main(int argc, char *argv[])
{	
	if (argc != 2) {
		cerr<<"Usage:"<<argv[0]<<" <ipv4|ipv6>" <<endl;
		exit(EXIT_FAILURE);
	}

	Network network;
	network.ipShow(argv[1]);

	return 0;
}
