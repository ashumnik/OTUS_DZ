# DZ8-posix

## LEAK SUMMARY before patch

==36294==    definitely lost: 2,490 bytes in 149 blocks <br />
==36294==    indirectly lost: 990 bytes in 66 blocks <br />
==36294==      possibly lost: 11,470,879 bytes in 76,860 blocks <br />
==36294==    still reachable: 10,541 bytes in 21 blocks <br />
==36294==         suppressed: 0 bytes in 0 blocks<br />
 
## LEAK SUMMARY after patch

==36483==    definitely lost: 0 bytes in 0 blocks <br />
==36483==    indirectly lost: 0 bytes in 0 blocks <br />
==36483==      possibly lost: 8,606,147 bytes in 57,682 blocks <br />
==36483==    still reachable: 10,541 bytes in 21 blocks <br />
==36483==         suppressed: 0 bytes in 0 blocks <br />