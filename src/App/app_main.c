/***********************************************************************
*
*  FILE        : RX65N_Sample.c
*  DATE        : 2023-07-08
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include "FreeRTOS_IP.h"
#include "FreeRTOS.h"
#include "task.h"


#define USE_IPv6_END_POINTS						1
#define USE_USER_COMMAND_TASK					1

/* Default MAC address configuration.  The demo creates a virtual network
connection that uses this MAC address by accessing the raw Ethernet/WiFi data
to and from a real network connection on the host PC.  See the
configNETWORK_INTERFACE_TO_USE definition above for information on how to
configure the real network connection to use. */

/* Default gateway IP address configuration.  Used in case ipconfigUSE_DHCP is
set to 0, or ipconfigUSE_DHCP is set to 1 but a DHCP server cannot be contacted. */
#define configGATEWAY_ADDR0                     192
#define configGATEWAY_ADDR1                     168
#define configGATEWAY_ADDR2                     2
#define configGATEWAY_ADDR3                     5

/* Default DNS server configuration.  OpenDNS addresses are 208.67.222.222 and
208.67.220.220.  Used if ipconfigUSE_DHCP is set to 0, or ipconfigUSE_DHCP is set
to 1 but a DHCP server cannot be contacted.*/
#define configDNS_SERVER_ADDR0                  118
#define configDNS_SERVER_ADDR1                  98
#define configDNS_SERVER_ADDR2                  44
#define configDNS_SERVER_ADDR3                  100

BaseType_t xEndPointCount = 0;
BaseType_t xUpEndPointCount = 0;

#define mainUSER_COMMAND_TASK_STACK_SIZE    2048
#define mainUSER_COMMAND_TASK_PRIORITY      (tskIDLE_PRIORITY)

void app_main(void)
{
    BaseType_t xRet;
    const uint8_t ucIPAddress[ 4 ] = { configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3 };
    const uint8_t ucNetMask[ 4 ] = { configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3 };
    const uint8_t ucGatewayAddress[ 4 ] = { configGATEWAY_ADDR0, configGATEWAY_ADDR1, configGATEWAY_ADDR2, configGATEWAY_ADDR3 };
    const uint8_t ucDNSServerAddress[ 4 ] = { configDNS_SERVER_ADDR0, configDNS_SERVER_ADDR1, configDNS_SERVER_ADDR2, configDNS_SERVER_ADDR3 };
	const uint8_t ucMACAddress[ 6 ] = { configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5 };

    configPRINTF( ( "Calling FreeRTOS_IPInit...\n" ) );
    //FreeRTOS_IPInit( ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress );
	#if ( ipconfigMULTI_INTERFACE == 1 ) && ( ipconfigCOMPATIBLE_WITH_SINGLE == 0 )
    	static NetworkInterface_t xInterfaces[1];
    	static NetworkEndPoint_t xEndPoints[4];
	#endif
    //FreeRTOS_debug_printf((“FreeRTOS_IPInit\r\n”));
	memcpy(ipLOCAL_MAC_ADDRESS, ucMACAddress, sizeof ucMACAddress);

	extern NetworkInterface_t * pxRX_FillInterfaceDescriptor( BaseType_t xEMACIndex,
                                                        NetworkInterface_t * pxInterface );

	/* Initialize the interface descriptor for RX. */
	pxRX_FillInterfaceDescriptor(0, &(xInterfaces[0]));

	/* === End-point 0 === */
	#if ( ipconfigUSE_IPv4 != 0 )
		{
			FreeRTOS_FillEndPoint(&(xInterfaces[0]), &(xEndPoints[xEndPointCount]), ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);
			#if ( ipconfigUSE_DHCP != 0 )
			{
				/* End-point 0 wants to use DHCPv4. */
				xEndPoints[xEndPointCount].bits.bWantDHCP = pdTRUE; // pdFALSE; // pdTRUE;
			}
			#endif /* ( ipconfigUSE_DHCP != 0 ) */

			xEndPointCount += 1;

		}
	#endif /* ( ipconfigUSE_IPv4 != 0 */

        /*
        *     End-point-1 : public
        *     Network: 2001:470:ed44::/64
        *     IPv6   : 2001:470:ed44::4514:89d5:4589:8b79/128
        *     Gateway: fe80::ba27:ebff:fe5a:d751  // obtained from Router Advertisement
        */
        #if ( ipconfigUSE_IPv6 != 0 && USE_IPv6_END_POINTS != 0 )
            {
                IPv6_Address_t xIPAddress;
                IPv6_Address_t xPrefix;
                IPv6_Address_t xGateWay;
                IPv6_Address_t xDNSServer1, xDNSServer2;

                FreeRTOS_inet_pton6( "2001:470:ed44::", xPrefix.ucBytes );

                FreeRTOS_CreateIPv6Address( &xIPAddress, &xPrefix, 64, pdTRUE );
                FreeRTOS_inet_pton6( "fe80::ba27:ebff:fe5a:d751", xGateWay.ucBytes );

                FreeRTOS_FillEndPoint_IPv6( &( xInterfaces[ 0 ] ),
                                            &( xEndPoints[ xEndPointCount ] ),
                                            &( xIPAddress ),
                                            &( xPrefix ),
                                            64uL, /* Prefix length. */
                                            &( xGateWay ),
                                            NULL, /* pxDNSServerAddress: Not used yet. */
                                            ucMACAddress );
                FreeRTOS_inet_pton6( "2001:4860:4860::8888", xEndPoints[ xEndPointCount ].ipv6_settings.xDNSServerAddresses[ 0 ].ucBytes );
                FreeRTOS_inet_pton6( "fe80::1", xEndPoints[ xEndPointCount ].ipv6_settings.xDNSServerAddresses[ 1 ].ucBytes );
                FreeRTOS_inet_pton6( "2001:4860:4860::8888", xEndPoints[ xEndPointCount ].ipv6_defaults.xDNSServerAddresses[ 0 ].ucBytes );
                FreeRTOS_inet_pton6( "fe80::1", xEndPoints[ xEndPointCount ].ipv6_defaults.xDNSServerAddresses[ 1 ].ucBytes );

                #if ( ipconfigUSE_RA != 0 )
                    {
                        /* End-point 1 wants to use Router Advertisement */
                        xEndPoints[ xEndPointCount ].bits.bWantRA = pdTRUE;
                    }
                #endif /* #if( ipconfigUSE_RA != 0 ) */
                #if ( ipconfigUSE_DHCPv6 != 0 )
                    {
                        /* End-point 1 wants to use DHCPv6. */
                        xEndPoints[ xEndPointCount ].bits.bWantDHCP = pdTRUE;
                    }
                #endif /* ( ipconfigUSE_DHCPv6 != 0 ) */

                xEndPointCount += 1;

            }

            /*
            *     End-point-3 : private
            *     Network: fe80::/10 (link-local)
            *     IPv6   : fe80::d80e:95cc:3154:b76a/128
            *     Gateway: -
            */
            {
                IPv6_Address_t xIPAddress;
                IPv6_Address_t xPrefix;

                FreeRTOS_inet_pton6( "fe80::", xPrefix.ucBytes );
                FreeRTOS_inet_pton6( "fe80::7009", xIPAddress.ucBytes );

                FreeRTOS_FillEndPoint_IPv6(
                    &( xInterfaces[ 0 ] ),
                    &( xEndPoints[ xEndPointCount ] ),
                    &( xIPAddress ),
                    &( xPrefix ),
                    10U,  /* Prefix length. */
                    NULL, /* No gateway */
                    NULL, /* pxDNSServerAddress: Not used yet. */
                    ucMACAddress );

                xEndPointCount += 1;
            }

        #endif /* ( ipconfigUSE_IPv6 != 0 && USE_IPv6_END_POINTS != 0 ) */

	FreeRTOS_IPInit_Multi();

}

#if defined(ipconfigIPv4_BACKWARD_COMPATIBLE) && ( ipconfigIPv4_BACKWARD_COMPATIBLE == 0 )
	void vApplicationIPNetworkEventHook_Multi( eIPCallbackEvent_t eNetworkEvent, struct xNetworkEndPoint* pxEndPoint )
#else
	void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent)
#endif /* defined(ipconfigIPv4_BACKWARD_COMPATIBLE) && ( ipconfigIPv4_BACKWARD_COMPATIBLE == 0 ) */
{
    /* If the network has just come up...*/
    if( eNetworkEvent == eNetworkUp )
    {
        uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
        char cBuffer[ 16 ];

        #if defined(ipconfigIPv4_BACKWARD_COMPATIBLE) && ( ipconfigIPv4_BACKWARD_COMPATIBLE == 0 )

            showEndPoint( pxEndPoint );

            xUpEndPointCount += 1;

            if( xUpEndPointCount >= xEndPointCount )
            {

#if USE_USER_COMMAND_TASK

    /* Sockets, and tasks that use the TCP/IP stack can be created here. */
    extern void prvServerWorkTask( void * pvArgument );
    xTaskCreate( prvServerWorkTask,
                "user_cmnd_task",
                mainUSER_COMMAND_TASK_STACK_SIZE,
                NULL,
                mainUSER_COMMAND_TASK_PRIORITY,
                NULL );

#endif
            }

        #else
        
            /* Print out the network configuration, which may have come from a DHCP
            * server. */
            #if defined(ipconfigIPv4_BACKWARD_COMPATIBLE) && ( ipconfigIPv4_BACKWARD_COMPATIBLE == 0 )
                FreeRTOS_GetEndPointConfiguration( &ulIPAddress, &ulNetMask, &ulGatewayAddress, &ulDNSServerAddress, pxNetworkEndPoints );
            #else
                FreeRTOS_GetAddressConfiguration( &ulIPAddress, &ulNetMask, &ulGatewayAddress, &ulDNSServerAddress );
            #endif /* defined(ipconfigIPv4_BACKWARD_COMPATIBLE) && ( ipconfigIPv4_BACKWARD_COMPATIBLE == 0 ) */

            FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
            configPRINTF( ( "IP Address: %s\n", cBuffer ) );

            FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
            configPRINTF( ( "Subnet Mask: %s\n", cBuffer ) );

            FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
            configPRINTF( ( "Gateway Address: %s\n", cBuffer ) );

            FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
            configPRINTF( ( "DNS Server Address: %s\n", cBuffer ) );
        
        #endif /* defined(ipconfigIPv4_BACKWARD_COMPATIBLE) && ( ipconfigIPv4_BACKWARD_COMPATIBLE == 0 ) */

    }
    else if( eNetworkEvent == eNetworkDown )
    {

    }
}
/*-----------------------------------------------------------*/

#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )

#if defined(ipconfigIPv4_BACKWARD_COMPATIBLE) && ( ipconfigIPv4_BACKWARD_COMPATIBLE == 0 )
	BaseType_t xApplicationDNSQueryHook_Multi( struct xNetworkEndPoint * pxEndPoint, const char * pcName )
#else
	BaseType_t xApplicationDNSQueryHook( const char * pcName )
#endif /* defined(ipconfigIPv4_BACKWARD_COMPATIBLE) && ( ipconfigIPv4_BACKWARD_COMPATIBLE == 0 ) */
{
    BaseType_t xReturn = pdFAIL;

    /* Determine if a name lookup is for this node.  Two names are given
     * to this node: that returned by pcApplicationHostnameHook() and that set
     * by mainDEVICE_NICK_NAME. */
    //if( strcasecmp( pcName, pcApplicationHostnameHook() ) == 0 )
    //{
        xReturn = pdPASS;
    //}
    return xReturn;
}

#endif /* ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) */

/*-----------------------------------------------------------*/

const char *pcApplicationHostnameHook( void )
{
    /* Assign the name "RX65" to this network node.  This function will be
     * called during the DHCP: the machine will be registered with an IP address
     * plus this name. */
    return "RX72M";
}
/*-----------------------------------------------------------*/

BaseType_t xApplicationGetRandomNumber( uint32_t *pulValue )
{
	BaseType_t xReturn = pdPASS;

	static uint32_t ulMockRand = 0xCAFE1234;
	*pulValue = ulMockRand++;

    return xReturn;
}
/*-----------------------------------------------------------*/

uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                             uint16_t usSourcePort,
                                             uint32_t ulDestinationAddress,
                                             uint16_t usDestinationPort )
{
    uint32_t ulReturn;

    ( void ) ulSourceAddress;
    ( void ) usSourcePort;
    ( void ) ulDestinationAddress;
    ( void ) usDestinationPort;

    xApplicationGetRandomNumber( &ulReturn );

    return ulReturn;
}
/*-----------------------------------------------------------*/

#if 0
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus,
                                uint16_t usIdentifier )
{
    /* Provide a stub for this function. */
}
#endif
