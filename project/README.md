## Project "NBMA support in ip6gre"

# build

Linux must be 5.10.140 <br />

cd linux <br />
git apply ../project/ip_net_ipv6_patch/ip6net_patch_0001.patch <br />
cd /net/ipv6 <br />
make <br />
<br />
rmmod ip6_gre <br />
rmmod ip6_tunnel <br />
insmod ip6_tunnel.ko <br />
insmod ip6_gre.ko <br />