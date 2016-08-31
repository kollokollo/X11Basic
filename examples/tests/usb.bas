' Demonstrated the use of the USB Interface of X11-Basic
' This also can be used to communicate with a USB-Serial
' adapter. Example is done with a VDL 101 T Datalogger:
' The program reads out all of the memory of the logger and dumps it
' into a file.
'
' Program was successfully tested on UBUNTU linux running as root
'
'
' (c) by Markus Hoffmann 2015
' X11-Basic Version >= 1.24
'
'
debug=0     ! set to 1 if you want verbose information
protocol=1  ! for VDL101T

' Open the USB-Device by Product and Vendor Number
OPEN "UY",#1,"0x10c4:0xea61"
@print_device_info
dumpfilename$="VDL101T-"+serialnr$+"-"+STR$(JULIAN(DATE$))+"-"+REPLACE$(TIME$,":","-")+".dat"
a=IOCTL(#1,0)   ! USB reset
IF a<0
  PRINT "usb_reset failed with status ";a
ENDIF
a=IOCTL(#1,2,1) ! USB set configuration
IF a<0
  PRINT "usb_set_configuration failed with status ";a
ENDIF
a=IOCTL(#1,3,0) ! USB claim interface
IF a<0
  PRINT "usb_claim_interface failed with status ";a
ENDIF
~@control_msg(0x40,0,0xff,0xff,5000,"")
~@control_msg(0x40,2,4,0,5000,"")
~@control_msg(0x40,2,2,0,5000,"")

~IOCTL(#1,5,64)   ! set block len
~IOCTL(#1,6,0x82) ! set endpoint in
~IOCTL(#1,7,2)    ! set endpoint out

' read config block
PRINT "Logger-Config:"
IF protocol=2
  buf$=CHR$(0xf)+CHR$(0)+CHR$(0)
ELSE
  buf$=CHR$(0)+CHR$(0x10)+CHR$(1)
ENDIF
SEND #1,buf$,2
RECEIVE #1,res$
MEMDUMP VARPTR(res$),LEN(res$)
RECEIVE #1,res$
MEMDUMP VARPTR(res$),LEN(res$)

PRINT "Logger-Data: (memory dump) -> ";dumpfilename$
OPEN "O",#2,dumpfilename$
FOR count=0 TO 15
  PRINT #2,@readmembank$(count);
NEXT count
PRINT "done."

IF debug
  @print_device_config
ENDIF
QUIT

' Logger Protocol functions

FUNCTION readmembank$(n%)
  LOCAL res$,buf$,mem$,j
  mem$=""
  ~@control_msg(0x40,2,4,0,5000,"")
  ~@control_msg(0x40,2,2,0,5000,"")
  IF protocol=2
    buf$=CHR$(0xf)+CHR$(n%)+CHR$(0)
  ELSE
    buf$=CHR$(0)+CHR$(n%)+CHR$(0x40)
  ENDIF
  PRINT "SEND ";n%,
  SEND #1,buf$,2
  RECEIVE #1,res$
  IF LEN(res$)=0
    RECEIVE #1,res$
  ENDIF
  ' memdump varptr(res$),len(res$)
  FOR j=0 TO 63
    PRINT ".";
    RECEIVE #1,res$
    IF LEN(res$)=0
      RECEIVE #1,res$
    ENDIF
    ' memdump varptr(res$),len(res$)
    mem$=mem$+res$
  NEXT j
  PRINT
  RETURN mem$
ENDFUNCTION

' general USB functions

' This prints out all information about the device
PROCEDURE print_device_config
  LOCAL t$,a%,config_adr%,filename$.i%
  t$=SPACE$(4148)  ! Device info takes 4148 bytes
  a%=IOCTL(#1,1,VARPTR(t$))
  IF a%<0
    PRINT "ERROR ioctl ";a%
  ENDIF

  ' MEMDUMP VARPTR(t$),LEN(t$)
  PRINT "next=$"+HEX$(LPEEK(VARPTR(t$)),8)
  PRINT "prev=$"+HEX$(LPEEK(VARPTR(t$)+4),8)
  filename$=""
  FOR i%=0 TO 4096-1
    EXIT IF PEEK(VARPTR(t$)+8+i%)=0
    filename$=filename$+CHR$(PEEK(VARPTR(t$)+8+i%))
  NEXT i%
  PRINT "filename: ";filename$
  PRINT "bus=$"+HEX$(LPEEK(VARPTR(t$)+8+4100),8)
  PRINT "bLength=";PEEK(VARPTR(t$)+8+4100+4) AND 255
  PRINT "bDescriptorType=$";HEX$(PEEK(VARPTR(t$)+8+4100+5),2)
  PRINT "bcdUSB=$";HEX$(DPEEK(VARPTR(t$)+8+4100+6),4)
  PRINT "bDeviceClass=$";HEX$(PEEK(VARPTR(t$)+8+4100+8),2)
  PRINT "bDeviceSubClass=$";HEX$(PEEK(VARPTR(t$)+8+4100+9),2)
  PRINT "bDeviceProtocol=$";HEX$(PEEK(VARPTR(t$)+8+4100+10),2)
  PRINT "bMaxPacketSize0=$";HEX$(PEEK(VARPTR(t$)+8+4100+11),2)
  PRINT "idVendor=$";HEX$(DPEEK(VARPTR(t$)+8+4100+12),4)
  PRINT "idProduct=$";HEX$(DPEEK(VARPTR(t$)+8+4100+14),4)
  PRINT "bcdDevice=$";HEX$(DPEEK(VARPTR(t$)+8+4100+16),4)

  PRINT "iManufacturer=$";HEX$(PEEK(VARPTR(t$)+8+4100+18),2)
  PRINT "iProduct=$";HEX$(PEEK(VARPTR(t$)+8+4100+19),2)
  PRINT "iSerialNumber=$";HEX$(PEEK(VARPTR(t$)+8+4100+20),2)
  PRINT "bNumConfigurations=$";HEX$(PEEK(VARPTR(t$)+8+4100+21),2)
  PRINT "config=$"+HEX$(LPEEK(VARPTR(t$)+8+4100+24),8)
  config_adr%=LPEEK(VARPTR(t$)+8+4100+24)
  PRINT "dev=$"+HEX$(LPEEK(VARPTR(t$)+8+4100+28),8)
  PRINT "devnum=$";HEX$(PEEK(VARPTR(t$)+8+4100+32),2)
  PRINT "num_children=$";HEX$(PEEK(VARPTR(t$)+8+4100+33),2)
  PRINT "children=$"+HEX$(LPEEK(VARPTR(t$)+8+4100+34),8)
  PRINT "Config:"
  PRINT "bLength=";PEEK(config_adr%)
  PRINT "bDescriptorType=$";HEX$(PEEK(config_adr%+1),2)
  PRINT "wTotalLength=$";HEX$(dpeek(config_adr%+2),4)
  PRINT "bNumInterfaces=$";HEX$(PEEK(config_adr%+4),2)
  PRINT "bConfigurationValue=$";HEX$(PEEK(config_adr%+5),2)
  PRINT "iConfiguration=$";HEX$(PEEK(config_adr%+6),2)
  PRINT "bmAttributes=$";HEX$(PEEK(config_adr%+7),2)
  PRINT "MaxPower=$";HEX$(PEEK(config_adr%+8),2)
  PRINT "interface=$"+HEX$(LPEEK(config_adr%+12),8)
  interface_adr%=LPEEK(config_adr%+12)
  PRINT "extra=$"+HEX$(LPEEK(config_adr%+16),8)
  PRINT "extralen=";LPEEK(config_adr%+20)
  PRINT "interface:"
  PRINT "interfacedescriptor=$"+HEX$(LPEEK(interface_adr%+0),8)
  interfaced_adr%=LPEEK(interface_adr%+0)
  PRINT "numaltsetting=";PEEK(interface_adr%+4) AND 255
  PRINT "bLength=$";HEX$(PEEK(interfaced_adr%+0),2)

  PRINT "bDescriptorType=$";HEX$(PEEK(interfaced_adr%+1),2)
  PRINT "bInterfaceNumber=$";HEX$(PEEK(interfaced_adr%+2),2)
  PRINT "bAlternateSetting=$";HEX$(PEEK(interfaced_adr%+3),2)
  PRINT "bNumEndpoints=";PEEK(interfaced_adr%+4)
  anzendpoints%=PEEK(interfaced_adr%+4)
  PRINT "bInterfaceClass=$";HEX$(PEEK(interfaced_adr%+5),2)
  PRINT "bInterfaceSubClass=$";HEX$(PEEK(interfaced_adr%+6),2)
  PRINT "bInterfaceProtocol=$";HEX$(PEEK(interfaced_adr%+7),2)
  PRINT "iInterface=$";HEX$(PEEK(interfaced_adr%+8),2)
  PRINT "endpoint=$"+HEX$(LPEEK(interfaced_adr%+12),8)
  endpoint_adr%=LPEEK(interfaced_adr%+12)
  PRINT "extra=$"+HEX$(LPEEK(interfaced_adr%+16),8)
  PRINT "extralen=";LPEEK(interfaced_adr%+20)
  PRINT "endpoints:"
  FOR i%=0 TO anzendpoints%-1
    PRINT "endpoint #";i%
    @print_endpoint(endpoint_adr%+i%*20)
  NEXT i%
RETURN
PROCEDURE print_endpoint(endpoint_adr%)
  PRINT "  bLength=";PEEK(endpoint_adr%+0),
  PRINT "  bDescriptorType=$";HEX$(PEEK(endpoint_adr%+1),2)
  PRINT "  bEndpointAddress=$";HEX$(PEEK(endpoint_adr%+2) AND 255,2)
  PRINT "  bmAttributes=$";HEX$(PEEK(endpoint_adr%+3),2)
  PRINT "  wMaxPacketSize=";DPEEK(endpoint_adr%+4)
  PRINT "  bInterval=$";HEX$(PEEK(endpoint_adr%+6),2),
  PRINT "  bRefresh=$";HEX$(PEEK(endpoint_adr%+7),2),
  PRINT "  bSynchAddress=$";HEX$(PEEK(endpoint_adr%+8),2)
  PRINT "  extra[";LPEEK(endpoint_adr%+16);"]=$"+HEX$(LPEEK(endpoint_adr%+12),8)
  ' print "extralen=";lpeek(endpoint_adr%+16)
RETURN
PROCEDURE print_device_info
  LOCAL t$,a%
  t$=SPACE$(4100)
  a%=IOCTL(#1,15,VARPTR(t$))
  serialnr$=LEFT$(t$,a%)
  WHILE RIGHT$(serialnr$)=CHR$(0)
    serialnr$=LEFT$(serialnr$,LEN(serialnr$)-1)
  WEND
  PRINT "Serialnr: ";serialnr$,
  a%=IOCTL(#1,14,VARPTR(t$))
  product$=LEFT$(t$,a%)
  PRINT "Product: ";product$,
  a%=IOCTL(#1,13,VARPTR(t$))
  manufacturer$=LEFT$(t$,a%)
  PRINT "Manufacturer: ";manufacturer$
  a%=IOCTL(#1,12,VARPTR(t$))
  devicefilenr$=LEFT$(t$,a%)
  PRINT "Filename: ";devicefilenr$
RETURN
FUNCTION control_msg(a%,b%,c%,d%,timeout%,data$)
  LOCAL ret,t$
  t$=MKL$(a%)+MKL$(b%)+MKL$(c%)+MKL$(d%)+MKL$(timeout%)+MKL$(LEN(data$))+data$
  ret=IOCTL(#1,4,VARPTR(t$))   ! USB control msg
  IF ret<0
    PRINT "usb_control_msg failed with status ";ret;" ";
    t$=SPACE$(100)
    ret=IOCTL(#1,16,VARPTR(t$)) ! USB control msg
    PRINT "Error: ";t$
  ENDIF
  RETURN ret
ENDFUNC
