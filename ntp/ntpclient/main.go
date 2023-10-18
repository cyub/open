package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"net"
	"time"
)

// NanosecondsToUnix is the difference between the start of NTP Era 0 and the Unix epoch in nanoseconds
// Jan-1 1900 00:00:00 UTC (start of NTP epoch Era 0) and Jan-1 1970 00:00:00 UTC (start of Unix epoch)
// Formula is 70 * (365 + 17) * 86400 (17 leap days)
// 2208988800 = -1 * time.Date(1900, 1, 1, 0, 0, 0, 0, time.UTC).Unix()
const NtpEpochOffset = 2208988800

type Packet struct {
	Settings       uint8  // leap year indicator, version number and mode
	Stratum        uint8  // stratum
	Poll           int8   // poll. Power of 2
	Precision      int8   // precision. Power of 2
	RootDelay      uint32 // total delay to the reference clock
	RootDispersion uint32 // total dispersion to the reference clock
	ReferenceID    uint32 // identifier of server or a reference clock
	RefTimeSec     uint32 // last time local clock was updated sec
	RefTimeFrac    uint32 // last time local clock was updated frac
	OrigTimeSec    uint32 // client time sec
	OrigTimeFrac   uint32 // client time frac
	RxTimeSec      uint32 // receive time sec
	RxTimeFrac     uint32 // receive time frac
	TxTimeSec      uint32 // transmit time sec
	TxTimeFrac     uint32 // transmit time frac
}

func main() {
	var host string
	flag.StringVar(&host, "host", "time.google.com", "ntp server host")
	flag.Parse()
	conn, err := net.Dial("udp", net.JoinHostPort(host, "123"))
	if err != nil {
		log.Fatal(fmt.Printf("connect %s error: %v", host, err))
	}
	defer conn.Close()

	conn.SetDeadline(time.Now().Add(5 * time.Second))
	var packet Packet
	packet.Settings = 0x1B // 客户端模式，版本：v3
	err = binary.Write(conn, binary.BigEndian, packet)
	if err != nil {
		log.Fatal(fmt.Printf("request ntp server error: %v", err))
	}

	err = binary.Read(conn, binary.BigEndian, &packet)
	if err != nil {
		log.Fatal(fmt.Printf("read ntp server response error: %v", err))
	}

	fmt.Println("mode:", packet.Settings&0x7)
	fmt.Println("version:", packet.Settings>>3&0x7)
	fmt.Println("leap year:", packet.Settings>>6&0x3)
	fmt.Println("Stratum:", packet.Stratum)
	fmt.Println("Poll:", packet.Poll)
	fmt.Println("Precision:", packet.Precision)

	secs := float64(packet.TxTimeSec) - NtpEpochOffset
	nanos := (int64(packet.TxTimeFrac) * time.Second.Nanoseconds()) >> 32
	t := time.Unix(int64(secs), nanos)
	fmt.Println("time:", t)
}
