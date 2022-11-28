## Project "NBMA support in ip6gre"

# build

Linux must be 5.10.140

cd linux
git apply ../project/ip_net_ipv6_patch/ip6net_patch_0001.patch
cd /net/ipv6
make

rmmod ip6_gre
rmmod ip6_tunnel
insmod ip6_tunnel.ko
insmod ip6_gre.ko