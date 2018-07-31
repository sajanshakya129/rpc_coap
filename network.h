#ifndef NETWORK_H
#define NETWORK_H

#include<iostream>
#include<string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <libmnl/libmnl.h>
#include <linux/if_link.h>
#include <linux/rtnetlink.h>

using namespace std;

class Network {
private:
	void socket(char*,int);
	static int data_cb_ipshow(const struct nlmsghdr*, void*);
	static int data_attr_cb_ipshow(const struct nlattr *, void *);
	static int data_cb_iplink(const struct nlmsghdr*, void *);
	static int data_attr_cb_iplink(const struct nlattr *, void *);
	

public:
	Network() {
		//constructor
	}
	void ipShow(char*);
	//void ipLink(char*);

	// void ipRouteAdd(char*);
	// void ipRouteShow(char*);
	// void ipRouteDel(char*);
};

//--------------------------------------------------------Beginnning of Private Methods--------------------------------------------------------------------------------

void Network::socket(char *ipType, int nlMsgType) {

	unsigned int seq, portid;

	char buf[MNL_SOCKET_BUFFER_SIZE]; //buffer
	struct nlmsghdr *nlh; //network link header
	struct rtgenmsg *rt;
	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type	= nlMsgType;//get information about Ip address
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;// flag for request
	nlh->nlmsg_seq = seq = time(NULL);
	
	rt = (rtgenmsg *)mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtgenmsg));//
	if (strcmp(ipType,"ipv4") == 0)
		rt->rtgen_family = AF_INET;// setting IPv4
	else if (strcmp(ipType,"ipv6") == 0)
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
		ret = mnl_cb_run(buf, ret, seq, portid, data_cb_ipshow, NULL);
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

int Network::data_attr_cb_ipshow(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = (const nlattr**)data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, IFA_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case IFA_ADDRESS:
		if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0) {
			perror("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	}
	tb[type] = attr;
	return MNL_CB_OK;
}

int Network::data_cb_ipshow(const struct nlmsghdr *nlh, void *data)
{	
	char ifrn_name[IFNAMSIZ];
	struct nlattr *tb[IFA_MAX + 1] = {};
	struct ifaddrmsg *ifa = (ifaddrmsg *)mnl_nlmsg_get_payload(nlh);

	//cout<<"index=%d family=%d ", ifa->ifa_index, ifa->ifa_family<<endl;
	mnl_attr_parse(nlh, sizeof(*ifa), data_attr_cb_ipshow, tb);
	cout <<"=================================="<<endl;
	cout << "Index= " << ifa->ifa_index<<endl;
	cout << "Name= "<< if_indextoname(ifa->ifa_index, ifrn_name)<<endl; 
	cout << "Family= "<<((ifa->ifa_family == AF_INET) ? "IPv4" :"IPv6")<<endl;
	cout << "Address= ";

	if (tb[IFA_ADDRESS]) {
		void *addr = mnl_attr_get_payload(tb[IFA_ADDRESS]);
		char out[INET6_ADDRSTRLEN];

		if (inet_ntop(ifa->ifa_family, addr, out, sizeof(out)))
			cout<<out<<"/"<<wchar_t(ifa->ifa_prefixlen)<<endl;
	}
	cout<<"Scope= ";
	switch(ifa->ifa_scope) {
	case 0:
		cout<<"Global "<<endl;
		break;
	case 200:
		cout<<"Site "<<endl;
		break;
	case 253:
		cout<<"Link "<<endl;
		break;
	case 254:
		cout<<"Host "<<endl;
		break;
	case 255:
		cout<<"Nowhere "<<endl;
		break;
	default:
		cout<<ifa->ifa_scope<<endl;
		break;
	}

	cout<<"\n"<<endl;
	return MNL_CB_OK;
}

int Network::data_attr_cb_iplink(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb =(const nlattr**) data;
	int type = mnl_attr_get_type(attr);

	/* skip unsupported attribute in user-space */
	if (mnl_attr_type_valid(attr, IFLA_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case IFLA_ADDRESS:
		if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0) {
			perror("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	case IFLA_MTU:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
			perror("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	case IFLA_IFNAME:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0) {
			perror("mnl_attr_validate");
			return MNL_CB_ERROR;
		}
		break;
	}
	tb[type] = attr;
	return MNL_CB_OK;
}

int Network::data_cb_iplink(const struct nlmsghdr *nlh, void *data)
{
	struct nlattr *tb[IFLA_MAX+1] = {};
	struct ifinfomsg *ifm = (ifinfomsg *)mnl_nlmsg_get_payload(nlh);

	cout<<"Index="<<ifm->ifi_index<<endl;
	cout<<"Type="<<ifm->ifi_type<<endl;
	cout<<"Flags="<<ifm->ifi_flags<<endl;
	cout<<"Family="<<ifm->ifi_family<<endl;

	if (ifm->ifi_flags & IFF_RUNNING)
		cout<<"[RUNNING]"<<endl;
	else
		cout<<"[NOT RUNNING]"<<endl;

	mnl_attr_parse(nlh, sizeof(*ifm), data_attr_cb_iplink, tb);
	if (tb[IFLA_MTU]) {
		cout<<"mtu="<<mnl_attr_get_u32(tb[IFLA_MTU])<<endl;
	}
	if (tb[IFLA_IFNAME]) {
		cout<<"name="<<mnl_attr_get_str(tb[IFLA_IFNAME])<<endl;
	}
	if (tb[IFLA_ADDRESS]) {
		uint8_t *hwaddr = (uint8_t *)mnl_attr_get_payload(tb[IFLA_ADDRESS]);
		int i;

		cout<<"hwaddr=";
		for (i=0; i<mnl_attr_get_payload_len(tb[IFLA_ADDRESS]); i++) {
			printf("%.2x", hwaddr[i] & 0xff);
			if (i+1 != mnl_attr_get_payload_len(tb[IFLA_ADDRESS]))
				cout<<":";
		}
	}
	cout<<"\n"<<endl;
	return MNL_CB_OK;
}

//-------------------------------------------------------------------------------------End of Private Functions-----------------------------------------------------------------------


//-------------------------------------------------------------------------------------Start of Public Functions----------------------------------------------------------------------

void Network::ipShow(char *ipType) {
	cout << "inside ip show" << endl;
	socket(ipType,RTM_GETADDR);
}

// void Network::ipLink(char *ipType) {
// 	cout << "inside ip link" << endl;
// 	socket(ipType,RTM_GETLINK);
// }

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


//-------------------------------------------------------End of Private Functions----------------------------------------

#endif