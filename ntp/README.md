# NTP

NTP 是网络时间协议 (Network Time Protocol)，它是用来同步网络中各个计算机的时间的协议。

## 协议

```
// Packet is an NTPv4 packet
/*
http://seriot.ch/ntp.php
https://tools.ietf.org/html/rfc958
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
0 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |LI | VN  |Mode |    Stratum     |     Poll      |  Precision   |
4 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                         Root Delay                            |
8 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                         Root Dispersion                       |
12+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                          Reference ID                         |
16+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                                                               |
  +                     Reference Timestamp (64)                  +
  |                                                               |
24+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                                                               |
  +                      Origin Timestamp (64)                    +
  |                                                               |
32+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                                                               |
  +                      Receive Timestamp (64)                   +
  |                                                               |
40+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                                                               |
  +                      Transmit Timestamp (64)                  +
  |                                                               |
48+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

 0 1 2 3 4 5 6 7
+-+-+-+-+-+-+-+-+
|LI | VN  |Mode |
+-+-+-+-+-+-+-+-+
 0 1 1 0 0 0 1 1

Setting = LI | VN  |Mode. Client request example:
00 011 011 (or 0x1B)
|  |   +-- client mode (3)
|  + ----- version (3)
+ -------- leap year indicator, 0 no warning
*/
```

# 参考来源

- [Network Time Protocol](https://en.wikipedia.org/wiki/Network_Time_Protocol)
- [beevik/ntp](https://github.com/beevik/ntp)
- [facebook/time](https://github.com/facebook/time/blob/main/ntp/protocol/ntp.go)