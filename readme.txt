RX70N Setup 
-----------

    Device code: R5F572NNHDFB
    -------------------------

        RX72N Group
        N : 4 Mbytes/1 Mbyte/32 Kbytes
        H : Encryption module included
        D : Operating peripheral temperature: –40 to +85°C
        FB : LFQFP/144/0.50

    Setup Notes:
    ------------
    
        • Make sure to install E2 Lite driver (windows 64 bit) from: 
            ○ https://www.renesas.com/us/en/software-tool/e2-emulator-lite-rte0t0002lkce00000r
            ○ Close E2 studio
            ○ Disconnect device
        • Make sure to set - Emulator use enabled by setting the SW1 Settings 2nd ch to Off state
            ○ This is required to program/debug the device as, if 2nc ch if On then Emulator use is 
            disabled. 
            ○ The board is provided with an emulator. The emulator connector (ECN1) has a 
            USB Micro-B configuration, and it serves as an interface for the integrated development
            environment (IDE) and Renesas Flash Programmer (RFP)
        • Connectors:
            ○ ECN1: USB Connector for On-Board Emulator, is used for programming and debugging the device
            ○ CN8: USB connector CN8 can be connected to the RL78/G1C and used as a virtual COM port
                ○ CN8's usage is controlled by 2nd ch of SW3 Settings
        • The RTOS selection in smart cfg is done during project creation not inside the smart config itself
            ○ This demo doesnt use smart CFG to configure FreeRTOS Kernel, instead its added as a 
            git submodule in src\Libraries\FreeRTOS-Kernel
            ○ Initialisation of FreeRTOS Kernel and hooks are taken from other Renesas Demos / FreeRTOS Demos.
        • If using UART (r_sci_rx), with the logging library from FreeRTOS demo (serial.c) make sure to
        enable  ENABLE TRANSMIT END INTERRUPT (ASYNCHRONOUS) interrupt in the r_sci_rx component
        setting in the scfg.
        • If there is linker error which says B section overlapping in which another variable
        assign the following 3 sections to the extensive RAM area in 0x00800000:
            ○ B_ETHERNET_BUFFERS_1
            ○ B_RX_DESC_1
            ○ B_TX_DESC_1
        • For the 'The register bus of PHY0/1 for ETHER0/1 select' setting in SCFG, though the notes
        for the setting in SCFG suggests to use value 1 for RX72N devices 
        (0 = The access of the register of PHY uses ETHER0, 1 = The access of the register of PHY uses ETHER1),
        use 0 (tested with RX72N Envision Kit)
        • Link status detection by interrupt pin should be disabled
        • Use Microchip KSZ8041 PHY on RX72N Envision Kit
        • ETH - PHY interface is MII for RX72N Envision Kit


    Sample IPv4 / IPv6 multi endpoint demo log on RX72N Envision Kit
    ----------------------------------------------------------------

        /*==================== arpqc 192.168.1.5 (1/4) ====================*/

        xHandleTestingCommand: the function xHandleTesting() was not called
        Cleared caches.
        ARPGetCacheEntry returns "Miss" Look for 192.168.0.1. Found end-point: yes
        handle_arpq: Looking up 192.168.0.1
        Heap: current 139608 lowest 139608
        ipARP_REPLY from 192.168.0.1 to 192.168.0.149 end-point 192.168.0.149
        ... Pause ...
        handle_arpq: after lookup: "Hit"


        /*==================== ifconfig (2/4) ====================*/

        xHandleTestingCommand: the function xHandleTesting() was not called
        Interface eth0
        IP-address : 192.168.0.149
        Default IP : 192.168.0.200
        End-point  : up = yes method DHCP
        Net mask   : 255.255.255.0
        GW         : 192.168.0.1
        DNS-0      : 49.205.72.130
        DNS-1      : 183.82.243.66
        Broadcast  : 192.168.0.255
        MAC address: 00-11-22-33-44-32

        IP-address : 2406:7400:56:4914:3912:feca:3a12:feca
        Default IP : 2001:470:ed44::3412:feca:3512:feca
        End-point  : up = yes method RA
        Prefix     : 2406:7400:56:4914::/64
        GW         : fe80::522b:73ff:feb4:a60
        DNS-0      : 2001:4860:4860::8888
        DNS-1      : fe80::1
        MAC address: 00-11-22-33-44-32

        IP-address : fe80::7009
        End-point  : up = yes method static
        Prefix     : fe80::/10
        GW         : ::
        DNS-0      : ::
        DNS-1      : ::
        MAC address: 00-11-22-33-44-32



        /*==================== dnsq4  aws.amazon.com (3/4) ====================*/

        xHandleTestingCommand: the function xHandleTesting() was not called
        pxDNSLookup: 'aws.amazon.com' IPv4 Sync DNS-clear = false
        Heap: current 117072 lowest 117072
        DNS[0x123D]: The answer to 'aws.amazon.com' (99.86.12.77) will be stored
        FreeRTOS_getaddrinfo: rc 0
        Found Address: 99.86.12.77
        dns query4: 'aws.amazon.com' = 99.86.12.77 rc = 0


        /*==================== dnsq6  google.nl (4/4) ====================*/

        xHandleTestingCommand: the function xHandleTesting() was not called
        pxDNSLookup: 'google.nl' IPv6 Sync DNS-clear = false
        prvNDCacheLookup 2001:4860:4860::8888 Miss
        eNDGetCacheEntry: lookup 2001:4860:4860::8888 miss
        eNDGetCacheEntry: Using gw fe80::522b:73ff:feb4:a60
        eNDGetCacheEntry: From addr 2406:7400:56:4914:3912:feca:3a12:feca
        prvNDCacheLookup fe80::522b:73ff:feb4:a60 Miss
        Looking up fe80::522b:73ff:feb4:a60 with end-point
        NEIGHBOR_ADV from fe80::522b:73ff:feb4:a60
        DNS_ReadReply returns -11
        prvIncreaseDNS6Index: from 0 to 1
        prvNDCacheLookup fe80::1 Miss
        eNDGetCacheEntry: lookup fe80::1 miss
        eNDGetCacheEntry: FindEndPointOnIP failed for fe80::1 (endpoint fe80::7009)
        Looking up fe80::1 with end-point
        DNS_ReadReply returns -11
        prvIncreaseDNS6Index: from 1 to 0
        prvNDCacheLookup 2001:4860:4860::8888 Miss
        eNDGetCacheEntry: lookup 2001:4860:4860::8888 miss
        eNDGetCacheEntry: Using gw fe80::522b:73ff:feb4:a60
        eNDGetCacheEntry: From addr 2406:7400:56:4914:3912:feca:3a12:feca
        eNDGetCacheEntry: found end-point 2406:7400:56:4914:3912:feca:3a12:feca
        xCheckRequiresARPResolution: 2001:4860:4860::8888 type Global
        FreeRTOS_getaddrinfo: rc 0
        Found Address: 2404:6800:4007:81d::2003
        dns query6: 'google.nl' = 2404:6800:4007:81d::2003 rc = 0
        Server task now ready.

    