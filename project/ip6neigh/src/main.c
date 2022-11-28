#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/if_arp.h>
#include <linux/if_tunnel.h>
#include <linux/ip6_tunnel.h>

#include "neigh.h"

int fd;

static int netlink_open()
{
	struct sockaddr_nl addr;
	int buf = NETLINK_KERNEL_BUFFER;

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (fd < 0) {
		printf("Cannot open netlink socket\n");
		return FALSE;
	}

	fcntl(fd, F_SETFD, FD_CLOEXEC);
	if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &buf, sizeof(buf)) < 0) {
		printf("SO_SNDBUF\n");
		goto error;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &buf, sizeof(buf)) < 0) {
		printf("SO_RCVBUF\n");
		goto error;
	}

	memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTMGRP_NEIGH; // RTMGRP_NEIGH;
	if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		printf("Cannot bind netlink socket\n");
		goto error;
	}

	return TRUE;

error:
	close(fd);
	return FALSE;
}

static int netlink_send(struct nlmsghdr *req)
{
	struct sockaddr_nl nladdr;
	struct iovec iov = {
		.iov_base = (void*) req,
		.iov_len = req->nlmsg_len
	};
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
	int status;

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;

	status = sendmsg(fd, &msg, 0);
	if (status < 0) {
		printf("Cannot talk to rtnetlink\n");
		return FALSE;
	}
	return netlink_receive();
}

static int netlink_add_rtattr_l(struct nlmsghdr *n, int maxlen, int type,
				const void *data, int alen)
{
	int len = RTA_LENGTH(alen);
	struct rtattr *rta;

	if (NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len) > maxlen)
		return FALSE;

	rta = NLMSG_TAIL(n);
	rta->rta_type = type;
	rta->rta_len = len;
	memcpy(RTA_DATA(rta), data, alen);
	n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len);
	return TRUE;
}

int kernel_inject_neighbor(struct ash_address *neighbor,
			   struct ash_address *hwaddr,
			   int *ifindex)
{
	struct {
		struct nlmsghdr 	n;
		struct ndmsg 		ndm;
		char   			buf[256];
	} req;
	char neigh[MAX_BUFF_SIZE], nbma[MAX_BUFF_SIZE];

	memset(&req.n, 0, sizeof(req.n));
	memset(&req.ndm, 0, sizeof(req.ndm));

	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ndmsg));
	req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_REPLACE | NLM_F_CREATE;
	req.n.nlmsg_type = RTM_NEWNEIGH;
	req.ndm.ndm_family = neighbor->type;
	req.ndm.ndm_ifindex = *ifindex;
	req.ndm.ndm_type = RTN_UNICAST;
	req.ndm.ndm_state = NUD_REACHABLE;

	netlink_add_rtattr_l(&req.n, sizeof(req), NDA_DST,
			     (char *)&neighbor->addr, neighbor->addr_len);
				 
	netlink_add_rtattr_l(&req.n, sizeof(req), NDA_LLADDR,
				 (char *)&hwaddr->addr, hwaddr->addr_len);

	return netlink_send(&req.n);
}

int iface_id_by_name(char *name)
{
	struct ifconf ifc = {0};
	struct ifreq *ifr = NULL;
	struct ifreq *item;
	int sck = 0, nInterfaces = 0;
	char buf[8192] = {0};
	char flag = 0;

	sck = socket(PF_INET, SOCK_DGRAM, 0);
	if(sck < 0) {
		fatal_perror("socket");
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if(ioctl(sck, SIOCGIFCONF, &ifc) < 0) {
		fatal_perror("ioctl(SIOCGIFCONF)");
	}

	ifr = ifc.ifc_req;
	nInterfaces = ifc.ifc_len / sizeof(struct ifreq); 
	for(int i = 0; i < nInterfaces; i++) {
		item = &ifr[i];
		if (!strcmp(name, item->ifr_name))
		{
			if(ioctl(sck, SIOCGIFINDEX, item) < 0) {
				fatal_perror("ioctl(SIOCGIFINDEX)");
			}
			flag = 1;
		}
	}
	
	if (!flag) 
	{
		printf("No interface named \"%s\"\n", name);
		exit(1);
	}
	
	return item->ifr_ifindex;
}

int addr_parce(struct ash_address *addr, char *addr_str)
{
	if (inet_pton(PF_INET6, addr_str, &addr->addr) == 1) {
		addr->type = PF_INET6;
        addr->addr_len = 16;
		return TRUE;
    }
	return FALSE;
}

void usage()
{
	printf("\n                       USAGE\n"
        "=======================================================\n"
        "./ip6neigh <IPv6 address> <link-layer address> <device>\n");
}

static int netlink_receive()
{
	int *reply;
	struct sockaddr_nl nladdr;
	struct iovec iov;
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
	int got_reply = FALSE, len;
	char buf[NETLINK_RECV_BUFFER];

	iov.iov_base = buf;
	while (!got_reply) {
		int status;
		struct nlmsghdr *h;

		iov.iov_len = sizeof(buf);
		status = recvmsg(fd, &msg, MSG_DONTWAIT);
		if (status < 0) {
			if (errno == EINTR)
				continue;
			if (errno == EAGAIN)
				return reply == NULL;
			printf("Netlink overrun\n");
			continue;
		}

		if (status == 0) {
			printf("Netlink returned EOF\n");
			return FALSE;
		}

		h = (struct nlmsghdr *) buf;
		while (NLMSG_OK(h, status)) {
			if (reply != NULL) {
				len = h->nlmsg_len;
				printf("Netlink message truncated\n");
				memcpy(reply, h, len);
				got_reply = TRUE;
			} else if (h->nlmsg_type != NLMSG_DONE) {
				printf("Unknown NLmsg: 0x%08x, len %d\n",
					  h->nlmsg_type, h->nlmsg_len);
			}
			h = NLMSG_NEXT(h, status);
		}
	}
	
	return TRUE;
}

int main(int argc, char **argv)
{
	int ifindex;
	struct ash_address *neigh;
	struct ash_address *hwaddr;
	
	if (argc != 4)
	{
		usage(argc);
		return 1;
	}
	
	netlink_open();
	
	ifindex = iface_id_by_name(argv[3]);
	neigh = malloc(sizeof(struct ash_address));
	hwaddr = malloc(sizeof(struct ash_address));
	addr_parce(neigh, argv[1]);
	addr_parce(hwaddr, argv[2]);
	kernel_inject_neighbor(neigh, hwaddr, &ifindex);

	close(fd);
	return 0;
}