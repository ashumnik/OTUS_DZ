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

#define fatal_perror(X) do { perror(X), exit(1); } while(0)

#define NLMSG_TAIL(nmsg) \
	((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

#define TRUE 1
#define FALSE 0

#define NETLINK_RECV_BUFFER	(8 * 1024)
#define MAX_BUFF_SIZE 128
#define NETLINK_KERNEL_BUFFER (256 * 1024)

struct ash_address {
	uint16_t type;
	uint8_t addr_len;
	uint8_t addr[16];
};

static int netlink_receive();