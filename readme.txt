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
