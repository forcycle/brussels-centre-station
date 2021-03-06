// pnx_parser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "Windows.h"


typedef signed char  int8_t;
typedef signed short int16_t;
typedef signed int   int32_t;
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

typedef struct _CHost_
{
    uint8_t host[50];
    uint8_t hid;
}_CHOST;

_CHOST _Host_Table_[]=
{
    {"Host Controller(Ex. PNx)",0x00},
    {"Terminal Host(Ex. MSMx)",0x01},
    {"UICC Host(Ex. GTOx)",0x02},
    {"RFU",0x03},
};
typedef enum _error_typing_{
	ERROR_TYPE_ = 0x0,
	ERROR_MAX_,
	ERROR_MIN_,
    ERROR_LEN_,
    ERROR_CRC_,
    ERROR_SF_,
    ERROR_OPT_,
    ERROR_WIN_,
    ERROR_BAU_,
	ERROR_UNKNOW_,
}_ERROR_TYPING_;

typedef enum _hic_type_
{
    CDE,//commands
    STS,//responses commands
    EVT,//events
    _HCI_TYPE_UNKNOW_
}_HCI_TYPE_;

typedef enum _proprietary_
{
    ETSI = 0x0,
    NXP,
    ETSI_NXP,
    _PROPRIETARY_UNKNOW
}_PROPRIETARY;

typedef enum _gate_type_
{
    Managemnet_gate = 0x0,
    Generic_Reader_RF_gate,
    Generic_Card_RF_gate,
    Generic_Connectivity_gate,
    Generic_System_Management_gate,
    Generic_Polling_Loop_Management_gate,
    Generic_SWP_Management_gate,
    Generic_NFC_WI_Management_gate,
    Generic_ISO_IEC_14443_TypeA_Reader_RF_gate,
    Generic_JIS6319_TypeF_Reader_RF_gate,
    Generic_Jewel_Topaz_Reader_RF_gate,
    Generic_ISO_IEC_15693_TypeV_Reader_RF_gate,
    Generic_NFC_IP1_gate,
    _All_gate_,
    _GATE_TYPE_UNKNOW,
}_GATE_TYPE_;

typedef enum
{
    _w_ = 0x01,
    _r_ = 0x02,
    _r_w_ = 0x10,
}_R_W_;
typedef struct _cRegistry_
{
    uint8_t gate[50];
    uint8_t iden_;
    _R_W_ _r_w_;//0x01 write,0x02 read,0x10 r/w
    uint8_t len_;
    uint32_t default_;
    uint8_t _comment_[2000];
}_CREGISTRY_;

_CREGISTRY_ Registry_Table[]=
{
    {"SESSION_IDENTITY",0x01,_r_w_,8,0xffffffffffffffff,"Session identifier that is used to detect if the connected\n\t\thost configuration changed."},
    {"MAX_PIPE",0x02,_r_,1,0x10,"Maximum number of created dynamic pipes supported by the host\n\t\tcontroller per host. The minimum value is '10' and the maximum\n\t\tvalue is '7D'.."},
    {"WHITELIST",0x03,_r_w_,0xff,0x0,"List of hosts that may communicate with the host connected\n\t\tto this administration gate."},
    {"HOST_LIST",0x04,_r_,0xff,0x10,"The list of the hosts that are accessible from this host\n\t\tcontroller including the host controller itself. The array\n\t\tcontains a list of host identifiers."},
    
    {"REC_ERROR",0x01,_r_w_,2,0x0000,"Number of invalid or lost frames previously sent by the\n\t\thost due to communication errors at the data link layer. This\n\t\tparameter can only be set to 0 in order to restart an\n\t\terror rate measure. When 'FFFF' is reached, the counter stops."},
    
    {"VERSION_SW",0x01,_r_,3,0x000000,"Optional,Version of the software defined by the vendor."},
    {"HCI_VERSION",0x02,_r_,1,0x01,"Optional,Version of HCI supported by the host."},
    {"VERSION_HARD",0x03,_r_,3,0x000000,"Optional,Version of the hardware defined by vendor"},
    {"VENDOR_NAME",0x04,_r_,0xff,0x00,"Optional,Vendor name UTF8 coding. The maximum value for N shall be 20."},
    {"MODEL_ID",0x05,_r_,1,0x00,"Optional,Model identifier assigned by the vendor."},
    {"GATES_LIST",0x06,_r_,0xff,0x0,"Mandatory,The list of all gates that accept dynamic pipes as an array\n\t\tof gate identifiers."},
    {"LOW_POWER_SUPPORT",0x07,_r_,1,0x0,"Indicates the power mode that is supported; where '01'\n\t\tindicates that low power mode is supported and '00'\n\t\tthat it is not supported."},
    
    {"DATARATE_MAX",0x01,_r_w_,1,0x10,"Maximum data rate supported"},
    {"UID",0x02,_r_,4,0x08000000,"UID as defined in ISO/IEC 14443-3 [6] for type A"},
    {"UID",0x02,_r_,7,0x08000000,"UID as defined in ISO/IEC 14443-3 [6] for type A"},
    {"UID",0x02,_r_,10,0x08000000,"UID as defined in ISO/IEC 14443-3 [6] for type A"},
    {"SAK",0x03,_r_,1,0x00,"Preformatted SAK as defined in ISO/IEC 14443-3 [6] for type A"},
    {"ATQA",0x04,_r_,2,0x0000,"Preformatted ATQA; byte 1 shall contain b8..1, byte 2 b16..9\n\t\tof the ATQA coded as defined in ISO/IEC 14443-3 [6]\n\t\tfor type A"},
    {"APPLICATION_DATA",0x05,_r_,0xff,0x0,"The historical bytes as defined in ISO/IEC 14443-3 [6]\n\t\tfor type A"},
    {"FWI, SFGT",0x06,_r_,1,0xee,"Preformatted SAK as defined in ISO/IEC 14443-3 [6] for type A"},

    {"HIGHER_LAYER_RESPONSE",0x01,_r_,0xff,0x0,"Higher layer response as defined in ISO/IEC 14443-3 [6]\n\t\tfor answer to ATTRIB command"},
    {"AFI",0x02,_r_w_,1,0x10,"Application family identifier as defined in ISO/IEC 14443-3[6]\n\t\tfor type B used by the reader to poll the target"},
    {"PUPI",0x03,_r_,0xff,0x0,"PUPI as defined in ISO/IEC 14443-3 [6] for type B"},
    {"APPLICATION_DATA",0x04,_r_,0xff,0x0,"Application data as defined in ISO/IEC 14443-3 [6] for ATQB"},
    {"HIGHER_LAYER_DATA",0x05,_r_w_,1,0x0,"Higher layer data as defined in ISO/IEC 14443-3 [6]\n\t\tfor ATTRIB command"},

    {"MODE",0x01,_r_w_,1,0xff,"Type A card emulation enabled indicator Coding for\n\t\tthe MODE is as follows:\n\t\t? 'FF' = Type A card emulation not enabled; and\n\t\t? '02' = Type A card emulation enabled; and\n\t\t? All other values are RFU."},
    {"UID_REG",0x02,_w_,0,0x0,"UID as defined in ISO/IEC14443-3 [6] for type A Coding for\n\t\tUID_REG is as follows:\n\t\t? If Length equals 0 then the CLF shall generate a single size\n\t\tUID with uid0 = '08' and uid1 to uid3 as random number.\n\t\tThe random number shall be generated only on state transitions\n\t\tfrom POWER-OFF to IDLE state(state definitions according\n\t\tto ISO/IEC 14443-3 [6]). The CLF shall interpret the absence\n\t\tof an RF-field as POWER-OFF state.\n\t\t? If Length equals 4, 7 or 10 then the CLF shall us\n\t\tUID_REG as UID."},
    {"UID_REG",0x02,_w_,4,0x0,"UID as defined in ISO/IEC14443-3 [6] for type A Coding for\n\t\tUID_REG is as follows:\n\t\t? If Length equals 0 then the CLF shall generate a single size\n\t\tUID with uid0 = '08' and uid1 to uid3 as random number.\n\t\tThe random number shall be generated only on state transitions\n\t\tfrom POWER-OFF to IDLE state(state definitions according\n\t\tto ISO/IEC 14443-3 [6]). The CLF shall interpret the absence\n\t\tof an RF-field as POWER-OFF state.\n\t\t? If Length equals 4, 7 or 10 then the CLF shall us\n\t\tUID_REG as UID."},
    {"UID_REG",0x02,_w_,7,0x0,"UID as defined in ISO/IEC14443-3 [6] for type A Coding for\n\t\tUID_REG is as follows:\n\t\t? If Length equals 0 then the CLF shall generate a single size\n\t\tUID with uid0 = '08' and uid1 to uid3 as random number.\n\t\tThe random number shall be generated only on state transitions\n\t\tfrom POWER-OFF to IDLE state(state definitions according\n\t\tto ISO/IEC 14443-3 [6]). The CLF shall interpret the absence\n\t\tof an RF-field as POWER-OFF state.\n\t\t? If Length equals 4, 7 or 10 then the CLF shall us\n\t\tUID_REG as UID."},
    {"UID_REG",0x02,_w_,10,0x0,"UID as defined in ISO/IEC14443-3 [6] for type A Coding for\n\t\tUID_REG is as follows:\n\t\t? If Length equals 0 then the CLF shall generate a single size\n\t\tUID with uid0 = '08' and uid1 to uid3 as random number.\n\t\tThe random number shall be generated only on state transitions\n\t\tfrom POWER-OFF to IDLE state(state definitions according\n\t\tto ISO/IEC 14443-3 [6]). The CLF shall interpret the absence\n\t\tof an RF-field as POWER-OFF state.\n\t\t? If Length equals 4, 7 or 10 then the CLF shall us\n\t\tUID_REG as UID."},
    
    {"CID_SUPPORT",0x07,_r_w_,1,0x0,"Support for CID as defined ISO/IEC 14443-4 [7] Coding\n\t\tfor the CID_SUPPORT parameter:\n\t\t? '01' = the support of the CID is required; the CLF shall set CID\n\t\tsupport in the ATS; and\n\t\t? '00' = the support of the CID is not required; the CLF may or may\n\t\tnot set CID support in the ATS.\n\t\t? All other values are RFU."},
    {"CLT_SUPPORT",0x08,_r_,1,0x0,"Support of tunnelling mode for ISO/IEC 14443-4 [7] non\n\t\tcompliant protocols Coding for the CLT_SUPPORT parameter:\n\t\t? '01' = the CLF contains a tunnelling mode capability for type A\n\t\tISO/IEC 14443-4 [7] non compliant protocol support; and\n\t\t? '00' = the CLF does not contain any tunnelling mode capability for\n\t\ttype A ISO/IEC 14443-4 [7] non compliant protocol support."},
    {"DATARATE_MAX",0x09,_r_w_,3,0x030300,"Maximum data rate supported DATARATE_MAX codes the maximum\n\t\tdivisor supported,\n\t\twith the coding as follows:\n\t\t? Byte 1 defines the maximum divisor\n\t\tsupported in direction\n\t\tPCD to PICC(All other values are RFU):\n\t\ 00 = the maximum divisor supported is 1 (~106 kbit/s);\n\t\01 = the maximum divisor supported is 2 (~212 kbit/s);\n\t\t- '02' = the maximum divisor supported is 4 (~424 kbit/s);\n\t\t- '03' = the maximum divisor supported is 8 (~848 kbit/s);\n\t\t? Byte 2 defines the maximum divisor supported in direction\n\t\tPICC to PCD(All other values are RFU):\n\t\t'00' = the maximum divisor supported is 1 (~106 kbit/s);\n\t\t'01' = the maximum divisor supported is 2 (~212 kbit/s);\n\t\t- '02' = the maximum divisor supported is 4 (~424 kbit/s);\n\t\t- '03' = the maximum divisor supported is 8 (~848 kbit/s);\n\t\t? Byte 3 defines a limitation to support different divisors\n\t\tfor each direction(All other values are RFU):\n\t\t- '00' = different divisors for each directions (PICC to PCD and\n\t\tPCD to PICC) is supported;\n\t\t- '01' = only the same divisor in both direction (PICC to PCD and\n\t\tPCD to PICC) is supported;\n\t\tNOTE: The resulting actual maximum supported divisor\n\t\tindicated by the interface byte TA(1) as defined\n\t\tin ISO/IEC 14443-4 [7] is given by the minimum\n\t\tof:the value and limitation as indicated in this\n\t\tregistry; ? the maximum divisor implemented\n\t\tin the CLF."},
    
    {"MODE",0x01,_r_w_,1,0xff,"Type B card emulation enabled Indicator. Coding for the\n\t\tMODE parameter(All other values are RFU):\n\t\t? 'FF' = Type B card emulation not enabled; and\n\t\t? '02' = Type B card emulation enabled; and"},
    {"PUPI_REG",0x02,_w_,0xff,0x0,"PUPI as defined in ISO/IEC 14443-3 [6] for type B.\n\t\tCoding for the PUPI_REG is as follows:\n\t\t? In case N=0 then the CLF shall generate the PUPI as\n\t\tdynamically generated number. The PUPI shall only be generated\n\t\tby a state transition from the POWER-OFF to the IDLE\n\t\tstate (state definitions according to ISO/IEC 14443-3 [6]).\n\t\tThe CLF shall interpret the absence of an RF-field as\n\t\tPOWER-OFF state.\n\t\t? In all other cases, the CLF shall use the PUPI _REG as PUPI."},
    {"AFI",0x03,_r_w_,1,0x0,"Application family identifier as defined in\n\t\tISO/IEC 14443-3 [6] for type B"},
    {"ATQB",0x04,_r_w_,4,0xe4000000,"Compressed CRC AID as described in the ISO/IEC 14443-3[6]\n\t\tNumber of applications as described in the ISO/IEC 14443-3[6]\n\t\tThird byte of the Protocol information as described in the\n\t\tISO/IEC 14443-3 [6] NOTE: PROTO_INFO also encodes FWI\n\t\tand support for CID, which are separate registry\n\t\tparameters in case of Type A."},
    {"HIGHER_LAYER_RESPONSE",0x05,_r_w_,0xff,0x00,"Higher Layer response in answer to ATTRIB command as defined\n\t\tin ISO/IEC 14443-3 [6] for type B"},
    {"DATARATE_MAX",0x06,_r_w_,0xff,0x030300,"Maximum data rate supported. codes the maximum bit\n\t\trates supported, with the coding as follows:\n\t\t? Byte 1 defines the maximum bit rate supported in direction\n\t\tPCD to PICC(All other values are RFU):\n\t\t'00' = the maximum bit rate supported is fc/128 (~106 kbps)\n\t\t- '01' = the maximum bit rate supported is fc/64 (~212 kbps)\n\t\t'02' = the maximum bit rate supported is fc/32 (~424 kbps)\n\t\t- '03' = the maximum bit rate supported is fc/16 (~848 kbps)\n\t\t? Byte 2 defines the maximum bit rate supported in direction\n\t\tPICC to PCD(All other values are RFU):\n\t\t- '00' = the maximum bit rate supported is fc/128 (~106 kbps)\n\t\t- '01' = the maximum bit rate supported is fc/64 (~212 kbps)\n\t\t- '02' = the maximum bit rate supported is fc/32 (~424 kbps)\n\t\t- '03' = the maximum bit rate supported is fc/16 (~848 kbps)\n\t\t? Byte 3 represents the limitation of having the bit rate in\n\t\tboth direction compulsory(All other values are RFU):\n\t\t- '00' = different bit rates in both directions (PICC\n\t\tto PCD and PCD to PICC) are supported;\n\t\t- '01' = only the same bit rate in both directions\n\t\t(PICC to PCD and PCD to PICC) is supported;\n\t\tNOTE: The resulting actual maximum bit rate supported is\n\t\tindicated in the first byte of the protocol information\n\t\tas defined in ISO/IEC 14443-3 [6] is given by the\n\t\tminimum of:the value and limitation as indicated\n\t\tin this registry; and the maximum divisor\n\t\timplemented in the CLF."},

    {"MODE",0x01,_r_w_,1,0xff,"Type B' card emulation enabled indicator.\n\t\tCoding for the MODE parameter\n\t\t(All other values are RFU):\n\t\t? 'FF' = Type B' card emulation not enabled; and\n\t\t? '02' = Type B' card emulation enabled; and"},
    {"PAT_IN",0x02,_r_w_,0xff,0x0,"Pattern to match with the incoming data"},
    {"DAT_OUT",0x03,_r_w_,0xff,0x0,"Data to reply if the incoming data matches with PAT_IN pattern"},

    {"MODE",0x01,_r_w_,1,0xff,"Type F card emulation enabled indicator. Coding for the MODE\n\t\tparameter(All other values are RFU):\n\t\t? 'FF' = Type F card emulation not enabled; and\n\t\t? '02' = Type F card emulation enabled; and"},
    {"SPEED_CAP",0x02,_r_,1,0x01,"CLF Speed Capabilities for Type F. coding for the SPEED_CAP\n\t\tparameter SPEED_CAP Meaning\n\t\tb8 1: auto bit rate detection supported,\n\t\t0: auto bit rate detection\n\t\tnot supported\n\t\tb7..b3 RFU\n\t\tb2 1: 424 kbps bit rate supported,0: 424 kbps bit rate\n\t\tnot supported\n\t\tb1 1: 212 kbps bit rate supported,\n\t\t0: 212 kbps bit rate not supported"},
    {"CLT_SUPPORT",0x03,_r_,1,0x0,"Support of tunnelling mode for anti-collision data exchange\n\t\tin type F card emulation Coding for the CLT_SUPPORT parameter:\n\t\t? '01' = the CLF contains a tunnelling mode capability for\n\t\ttype F card emulation anti-collision support; and\n\t\t? '00' = the CLF does not contain any tunnelling mode capability\n\t\tfor type F card emulation anti-collisionsupport."},

    {"NXP_INFO_NOTIFICATION",0x02,_r_w_,1,0x00,"Indicates on information the host wants to be notified of.\n\t\tBit Information event\n\t\t0 RFU\n\t\t1 PmuVcc switch\n\t\t2 External RF field\n\t\t3 Memory violation5\n\t\t4 Temperature overheat\n\t\t5 LLC Error Counter reached on\n\t\tUICC\n\t\t6...7 RFU\n\t\t0x0 -> do not notify\n\t\t0x1 -> notify"},
    {"NXP_INFO_EEPROM_ERR",0x03,_r_,1,0x00,"Indicates status of the last EEPROM check procedure:\n\t\t- Bit 0: CRC check on Patch area\n\t\t- Bit 1: CRC check on ConfigPage area\n\t\t- Bit 7: CRC check on TrimPage area\n\t\t- Others bits are RFU\n\t\t0x0 -> no CRC error\n\t\t0x1 -> CRC error\n\t\tIn case of CRC error on Patch area,\n\t\tpatches are disabled.In case of CRC error on\n\t\tConfigPage area,default configuration is\n\t\tapplied.In case of CRC error on TrimPage area,\n\t\tdefault trimming values are applied."},
    {"NXP_PL_RDPHASES",0x06,_r_w_,1,0x00,"Bit Technology\n\t\t0 Detection type A\n\t\t1 Detection type B\n\t\t2 Detection type F 212\n\t\t3 Detection type F 424\n\t\t4 Detectin ISO15693\n\t\t Detection NFCactive\n\t\t6 RFU\n\t\t7 Detection Pause(*)\n\t\t0 -> disabled\n\t\t1 -> enabled\n\t\t(*)When Pause phase is enabled, it implicitly\n\t\treplaces the Emulation phase (setting:\n\t\tNXP_PL_PAUSE)\n\t\t"},
    {"NXP_PL_EMULATION",0x07,_r_w_,2,0x5161,"Indicates the Emulation phase duration.\n\t\t(from 0 to 3.145s in 48μs step)."},
    {"NXP_PL_PAUSE",0x08,_r_w_,4,0x0824,"Indicates the Pause phase duration.\n\t\t(from 0 to 3.145s in 48μs step)."},
    {"NXP_PL_NFCT_DEACTIVATED",0x09,_r_w_,1,0x00,"Disable all NFC Target feature (passive/ active/allbaudrates).\n\t\t0x00 : NFC Target Phase Activated\n\t\t0x01 : NFC Target Phase deactivated\n\t\tOther values unsupported."},
    {"NXP_SWP_DEFAULTMODE",0x01,_r_w_,1,0x00,"Indicates if the SWP link has to be enabled\n\t\twhen required (only when external field is tpresent).\n\t\t0x00 -> Off (never activated)\n\t\t0x01 -> On (activated when required)\n\t\tOthers values are RFU.\n\t\t"},
    {"NXP_SWP_STATUS",0x02,_r_,1,0x00,"Indicates the current status of the SWP link\n\t\t(see below state chart):0x00 -> Not connected\n\t\t(activation sequence\n\t\tnot performed) 0x01 -> Connecting (activation sequence ongoing)\n\t\t0x02 -> Connected (activation sequencesuccessful)\n\t\t0x03 -> Connection lost (communication failedafter activation)\n\t\t0x04 -> Disconnecting (deactivation sequence on-going)\n\t\t0x05 -> Connection failed (activation sequence failed)\n\t\t0x06 -> Connection inactive (activation sequence successful\n\t\tbut SWP link deactivated) Others values are RFU."},
    {"NXP_SWP_PROTECTION_MODE",0x03,_r_w_,1,0x00,"Indicates if the protected mode is enabled (see\n\t\tbelow a description of the protection modefeature) :\n\t\t0x00 -> Disabled\n\t\t0x01 -> Enabled\n\t\tOthers values are RFU."},
    {"FWI",0x06,_r_w_,1,0x07,"Frame waiting time as defined in ISO/IEC 14443-4 for type A"},
    {"DATARATE_MAX",0x09,_r_w_,3,0x01,"Frame waiting time as defined in ISO/IEC 14443-4 for type A"},
    {"ATQB ->FWI(*)",0x04,_r_w_,1,0x07,"Frame waiting time as defined in ISO/IEC 14443-3 for Type B"},
    {"DATARATE_MAX",0x06,_r_w_,3,0x00,"Frame waiting time as defined in ISO/IEC 14443-3 for Type B"},
    {"NXP_SE_DEFAULTMODE",0x01,_r_w_,1,0x00,"Indicate the default mode of the SE connected over NFCWI.\n\t\t0 -> Off mode\n\t\t1 -> Virtual mode\n\t\tOthers values are RFU"},
    {"NXP_SE_EVENTS",0x05,_r_w_,4,0x08000000,"defines which events have to be raised to the host when\n\t\toccurred :\n\t\tBit Event\n\t\t0 Start of transaction 1 End of transaction\n\t\t2 Transaction (AID) 3 .. 7 RFU\n\t\t0 -> not to be raised\n\t\t1 -> to be raised"},
    {"NXP_AUTO_ACTIVATION",0x10,_r_w_,1,0x01,"If set to 0, the activation procedure will stop after Select\n\t\t(SAK has been received). The host could evaluate SAK value\n\t\tand then decide:\n\t\t- to start communicating with the remote card using proprietary\n\t\tcommands (see NXP_MIFARE_RAW and NXP_MIFARE_CMD) or\n\t\t- to activate the remote card up to ISO14443-4 level (RATS\n\t\tand PPS) using commandNXP_WRA_CONTINUE_ACTIVATION If set to 1,\n\t\tactivation follows the flow described in ETSI HCI specification\n\t\t(restrict detection to ISO14443-4 compliant cards)."},
    {"NXP_FELICA_SYSTEMCODE",0x01,_r_,2,0x00,"System Code of the FeliCa card."},
    {"NXP_FELICA_POLREQSYSCODE",0x02,_r_,2,0x00,"System code within the polling request in FeliCa reader mode.\n\t\tValues stored in EEPROM addresses 0x9F9B (MSB) and 0x9F9C (LSB)"},
    {"NXP_ FELICA_CURRENTIDM",0x04,_r_,8,0x00,"IDm of the tag currently communicating with the reader."},
    {"NXP_FELICA_CURRENTPMM",0x05,_r_,8,0x00,"PMm of the tag currently communicating with the reader."},
    {"NXP_ISO15693_INVENTORY",0x01,_r_,10,0x00,"Indicates the VICC parameters as follows:\n\t\t- 1st byte: FLAGS\n\t\t- 2nd byte: DSFID\n\t\t- 3rd to 10th bytes: UID"},
    {"NXP_ISO15693_AFI",0x02,_r_w_,1,0x00,"Indicates the AFI used by PNx for detection\n\t\t(during Inventory procedure)"},
    {"NXP_NFCI_MODE",0x01,_r_w_,1,0x00,"Supported NFCIP-1 Modes. This value shall be interpreted\n\t\tas a bit mask:\n\t\tBit Mode\n\t\t0 106kbit/s passive; 1 212kbit/s passive;\n\t\t2 424kbit/s passive; 3 106kbit/s active(*);\n\t\t4 212kbit/s active(*); 5 424kbit/s active(*);6 ... 7 RFU\n\t\t0 -> not supported; 1 -> supported\n\t\t(*) In active only one mode is used in the polling loop\n\t\tsequence (only the one which have the lowest baudrate from\n\t\tthe enabled ones)"},
    {"NXP_NFCI_ATR_REQ",0x02,_r_w_,0xff,0x00,"Contains the General Bytes of the ATR_REQ\n\t\t(max size is 48 bytes)"},
    {"NXP_NFCI_ATR_RES",0x03,_r_,0xff,0x00,"Contains the General Bytes of the ATR_RES\n\t\t(max size is 48 bytes)"},
    {"NXP_NFCI_BRS",0x04,_r_w_,1,0x00,"Specifies the bit rates to be used for DEP exchanges:\n\t\t- Bits 0 to 2: Target to initiator\n\t\t- Bits 3 to 5: Initiator to target\n\t\tBit rate is coded as follow:\n\t\t0x00 -> 106 kbps\n\t\t0x01 -> 212 kbps\n\t\t0x02 -> 424 kpbs\n\t\tOthers values are RFU\n\t\t- Bit 6: RFU\n\t\t- Bit 7: Indicates if bit rates have to be\n\t\tchanged for DEP exchanges (PSL command\n\t\tto be used or not)\n\t\t0x0 -> bit rates remain unchanged (from\n\t\tbit rate used for activation)\n\t\t0x1 -> bit rates to be change"},
    {"NXP_NFCI_DID",0x06,_r_w_,1,0x00,"Value used as DID (if set to 0, DID is not used)."},
    {"NXP_NFCI_STATUS",0x09,_r_,1,0x00,"Contains the current status of the NFCIP-1 link when \n\t\tcommunication has been set.\n\t\t0x00 -> data is expected from the host\n\t\t0x01 -> data is expected from the RF side\n\t\tOthers values are RFU"},
    {"NXP_NFCI_NFCID3I",0x0a,_r_,10,0x00,"Contains the random NFCID3I conveyed with the ATR_REQ."},
    {"NXP_NFCI_NFCID3T",0x0b,_r_,10,0x00,"Contains the random NFCID3T conveyed with the ATR_RES."},
    {"NXP_NFCI_PARAM",0x0c,_r_,1,0x00,"Contains the current parameters of the NFCIP-1 link when\n\t\tcommunication has been set.\n\t\t- bits 0 to 2: datarate Target to Initiator;\n\t\t- bits 3 to 5: datarate Initiator to\n\t\tTarget 0 -> Divisor equal to 1 ;\n\t\t1 -> Divisor equal to 2 ; 2 -> Divisor equal to 4 ;\n\t\tOther values -> RFU\n\t\t- bits 6 to 7: maximum frame length 0 -> 64 bytes ;\n\t\t1 -> 128 bytes ; 2 -> 192 bytes ; 3 -> 256 bytes"},
    {"NXP_NFCT_MODE",0x01,_r_w_,1,0x00,"Supported NFCIP-1 Modes.\n\t\tThis value shall be interpreted as a bit mask:Bit Mode\n\t\t0 106kbit/s passive ; 1 212kbit/s passive ;\n\t\t 2 424kbit/s passive ; 3 Active ; 4 ... 7 RFU\n\t\t0 -> not supported 1 -> supported"},
    {"NXP_NFCT_ATR_REQ",0x02,_r_,0xff,0x00,"Contains the General Bytes of the ATR_REQ\n\t\t(max size is 48 bytes)"},
    {"NXP_NFCT_ATR_RES",0x03,_r_w_,0xff,0x00,"Contains the General Bytes of the ATR_RES\n\t\t(max size is 48 bytes)"},
    {"NXP_NFCT_STATUS",0x09,_r_,0x1,0x00,"Contains the current status of the NFCIP-1link when\n\t\tcommunication has been set.\n\t\t0x00 -> data is expected from the host;\n\t\t0x01 -> data is expected from the RF side ;\n\t\tOthers values are RFU"},
    {"NXP_NFCT_NFCID3I",0x0a,_r_,10,0x00,"Contains the random NFCID3I conveyed with the ATR_REQ."},
    {"NXP_NFCT_NFCID3T",0x0b,_r_,10,0x00,"Contains the random NFCID3T conveyed with the ATR_RES."},
    {"NXP_NFCT_PARAM",0x0c,_r_,1,0x00,"Contains the current parameters of the NFCIP-1 link when\n\t\tcommunication has been set.\n\t\t- bits 0 to 2: datarate Target to Initiator ;\n\t\t- bits 3 to 5: datarate Initiator to Target\n\t\t0 -> Divisor equal to 1 ; 1 -> Divisor equal to 2 ; \n\t\t2 -> Divisor equal to 4 ; Other values -> RFU\n\t\t- bits 6 to 7: maximum frame length\n\t\t0 -> 64 bytes ; 1 -> 128 bytes ;\n\t\t2 -> 192 bytes ; 3 -> 256 bytes"},
    {"NXP_NFCT_MERGE",0x0d,_r_w_,1,0x00,"Indicates if the NFCIP-1 target feature must be merged with\n\t\tType A RF card feature in order to present only one type A\n\t\ttarget(set of the related bit in SAK to reflect the\n\t\tISO18092 compliancy).\n\t\t0x00 -> disabled ; 0x01 -> enabled ; Others values are RFU"},
};

typedef struct _cGATE_
{
    uint8_t gate[100];
    _GATE_TYPE_ gType;
}_CGATE_;

_CGATE_ Gate_Ref[] =
{
    {"Managemnet gate(ETSI)",Managemnet_gate},
    {"Generic Reader RF gate(ETSI)",Generic_Reader_RF_gate},
    {"Generic Card RF gate(ETSI)",Generic_Card_RF_gate},
    {"Generic Connectivity gate(ETSI)",Generic_Connectivity_gate},
    {"Generic System Management gate(NXP)",Generic_System_Management_gate},
    {"Generic Polling Loop Management_gate(NXP)",Generic_Polling_Loop_Management_gate},
    {"Generic SWP Management gate(NXP)",Generic_SWP_Management_gate},
    {"Generic NFC WI Management gate(NXP)",Generic_NFC_WI_Management_gate},
    {"Generic ISO IEC 14443 TypeA Reader RF gate(NXP)",Generic_ISO_IEC_14443_TypeA_Reader_RF_gate},
    {"Generic JIS6319 TypeF Reader RF gate(NXP)",Generic_JIS6319_TypeF_Reader_RF_gate},
    {"Generic Jewel Topaz Reader RF gate(NXP)",Generic_Jewel_Topaz_Reader_RF_gate},
    {"Generic ISO IEC 15693 TypeV Reader RF gate(NXP)",Generic_ISO_IEC_15693_TypeV_Reader_RF_gate},
    {"Generic NFC IP1 gate(NXP)",Generic_NFC_IP1_gate},
    {"All gate(ETSI/NXP)",_All_gate_},
    {"GATE TYPE UNKNOW",_GATE_TYPE_UNKNOW},
};
typedef struct _cREG_
{
    uint16_t address;
    uint8_t name[100];
    uint8_t comment[0xffff];
    uint8_t default_;
}_CREG_;

_CREG_ Register_Table[]=
{
    {0xF830,"Debug_Interface","Debug_Interface Number\n\t\t(default value 0x00 = No debug mode)",0x00},
    {0x9C00,"SWP_CurrentThreshold","0x02 -> 240 μA\n\t\t0x12-> 260 μA\n\t\t0x22-> 300 μA\n\t\t0x32-> 330 μA\n\t\tOthers values are RFU",0x22},
    {0x9C01,"SWP_Bitrate","Indicates the bitrates to be used on SWP according to UICC\n\t\tcapability).\n\t\tSupported bitrates are:\n\t\t- 0x02 -> 212 kbit/s\n\t\t- 0x04-> 424 kbit/s\n\t\t- 0x08-> 848 kbit/s\n\t\t- 0x10-> Maximum bitrate\n\t\tOthers values are RFU",0x08},
    {0x9C02,"SWP_SyncID0","Reference value for checking the UICC identity during ACT\n\t\tSYNC_ID verification sequence (SWP activation).\n\t\tCan be used to inhibit the UICC.",0x13},
    {0x9C03,"SWP_SyncID1","Reference value for checking the UICC identity during ACT\n\t\tSYNC_ID verification sequence (SWP activation).\n\t\tCan be used to inhibit the UICC.",0x13},
    {0x9C04,"SWP_MiscConf","- Bit 0: Bit1: force the CRC to be OK for each frame received\n\t\t- Bit 3: if equal to 1 the RNR will be sent after ClearAllPipe command\n\t\t(and other long processing command) instead of RR\n\t\t- Bit 4: if equal to 1 the soft reset in case of too many errors on RX is\n\t\tactivated\n\t\t- Bit 5: if equal to 1 the soft reset in case of too many errors on TX is\n\t\tactivated\n\t\t- Others Bits are RFU",0x30},
    {0x9EA2,"UICC_AdminSessionId","Registry entry related to SessionIdentity (Administration gate) of the\n\t\tUICC. Can be used to force UICC to re-create its context.",0xff},
    {0x9EA3,"--","--",0xff},
    {0x9EA4,"--","--",0xff},
    {0x9EA5,"--","--",0xff},
    {0x9EA6,"--","--",0xff},
    {0x9EA7,"--","--",0xff},
    {0x9EA8,"--","--",0xff},
    {0x9EA9,"--","--",0xff},
    {0x9EB3,"SwpMgt_Default_SwpState","- 0x00 -> default swio state is pull down\n\t\t- 0x01 -> default swio state is High Impedance\n\t\t- 0x02 -> default swio state is forced HighZ as soon as an UICC\n\t\tClassB is detected\n\t\t- Other values are RFU",0x00},
    {0x9EB4,"SwpMgt_Request_Power","Indicates if the power needs to be requested before a\n\t\tcommunication to the UICC can be started in case the voltage on\n\t\tPMUVcc is already there but the PMU is switched to low power\n\t\tmode.\n\t\t- 0x00 -> no request\n\t\t- 0x01 -> request through CLKREQ pin (GPIO pin 2 set to high\n\t\twhen clock requested, Hiz unless, whatever GPIO default\n\t\tconfiguration)\n\t\t- 0x02 -> request through PWR_REQUEST pin (GPIO pin 3 set to\n\t\thigh when clock requested, Hiz unless, whatever GPIO default\n\t\tconfiguration)\n\t\t- Others values are RFU",0x00},
    {0x9ED7,"UICC_GateList","Registry entry related to the Gate list (IdentityManagement gate) of\n\t\tthe Uicc. Can be used to define UICC rights. For example, to allow\n\t\tCard type B and Reader Type A feature, related gate Ids 0x21 and\n\t\t0x13 must be present (at any place, list order having no impact) in\n\t\tthis list.",0x04},
    {0x9ED8,"--","--",0xff},
    {0x9ED9,"--","--",0xff},
    {0x9EDA,"--","--",0xff},
    {0x9EDB,"--","--",0xff},
    {0x9EDC,"--","--",0xff},
    {0x9EDD,"--","--",0xff},
    {0x9EDE,"--","--",0xff},
    {0x9EDF,"--","--",0xff},
    {0x9EE0,"--","--",0xff},
    {0x9EE1,"--","--",0xff},
    {0x9EE2,"--","--",0xff},
    {0x9EE3,"--","--",0xff},
    {0x9EE4,"--","--",0xff},
    {0x9EE5,"--","--",0xff},
    {0x9EE6,"--","--",0xff},
    {0x9EE7,"--","--",0xff},
    {0x9EE8,"--","--",0xff},
    {0x9EE9,"--","--",0xff},
    {0x9EEA,"--","--",0xff},
    {0x9F0A,"SWP_PBTF_RFLD","SWP PowerByTheField RF Level Detector sensitivity:\n\t\tValue to be set from 0x00 to 0x07.\n\t\t0x00: Maximum sensitivity (to detect Low RF field).\n\t\t0x07: Minimum sensitivity.",0x05},
    {0xF841,"SWIO_HighZ","Register to set SWP SWIO line in HighZ mode.\n\t\t0x01: The SWP Pad is in Power Down mode and the SWP\n\t\tline is pull down mode.\n\t\t0x03: The SWP Pad is in Power Down mode and the SWP\n\t\tline is HighZ mode.\n\t\t- Bit 2-7 : RFU (keep unchanged)",0x03},
    {0x9800,"SE_Conf","Defines the properties of Secure Elements\n\t\t(UICC and NFC-WI device)\n\t\t-Bit 0 : NFC-WI device connected to PN54x\n\t\t-Bit 1 : UICC/SIM card connected to PN54x\n\t\t-Bit 2 : Powering of UICC enabled\n\t\t-Bit 3 : Communication to a UICC allowed\n\t\t-Bits 4 - 5 : External Power Selection for PbtF-mode\n\t\t00 ... SIM/UICC\n\t\t01 ... SE\n\t\t10-11 ... No device\n\t\t-Bit 6 : Enable power supply for NFC-WI during boot (battery\n\t\tpowered)\n\t\t-Bit 7 : Enable power supply for UICC/SIM during boot (battery\n\t\tpowered)",0x3f},
    {0x9B4A,"NfcWi_SE_TimeOut_H","Timeout (in milliseconds) value of inactivity on Sigin line to detect\n\t\tEndOfTransaction. If set to 0, it means that timeout is not used to\n\t\tdetect the end of the transaction.",0x00},
    {0x9B4B,"NfcWi_SE_TimeOut_L","Timeout (in milliseconds) value of inactivity on Sigin line to detect\n\t\tEndOfTransaction. If set to 0, it means that timeout is not used to\n\t\tdetect the end of the transaction.",0x00},
    {0x9B69,"NfcWi_SE_SigInDelay","Indicates whether the PNx apply a delay when transmitting data from\n\t\tSigin to RF or not\n\t\t- 0x00 -> No delay inserted\n\t\t- 0x01 -> Delay inserted",0x01},
    {0x9E75,"WI_SE_SigoutSel","Defines which signal is put to SigOut when in virtual mode (in wired\n\t\tmode this parameter is fixed to 0x02 -> TX envelope)\n\t\t0x00 -> Low\n\t\t0x01 -> High\n\t\t0x02 -> TX envelope\n\t\t0x03 -> MILLER enveloppe (ISO14443A @106kbit/s)\n\t\t0x04 -> MANCHESTER envelope (FeliCa)\n\t\t0x05 -> NRZ envelope\n\t\tOthers values are RFU",0x03},
    {0x9E76,"WI_SE_SigoutClkSel","Defines how the 13.56MHz clock is combined to the SigOut signal.\n\t\t- 0x00 -> Plain signal on SigOut, the clock is not combined\n\t\t- 0x20 -> Clock is XORed to the SigOut signal (ISO14443B, FeliCa)\n\t\t- 0x40 -> Clock is ANDed to the SigOut signal (ISO14443A\n\t\t@106kbit/s)\n\t\t- Others values are RFU",0x40},
    {0x9801,"ANAIRQ_Conf","Analog-IRQ Configuration\n\t\t-Bits 0-1 : RFU (keep unchanged)\n\t\t-Bit 2 : Enable/Disable PMU-Vcc IRQ\n\t\t-Bit 3 : RFU\n\t\t-Bit 4 : Enable/Disable RF-Modulation Tuning\n\t\t-Bit 5 : Enable/Disable SWP-Debug during boot-process\n\t\t-Bits 6-7 : RFU",0x17},
    {0x9805,"AST-TX1pass","Antenna Self Test: RFLD pass threshold on TX1 (1st parameter)",0x03},
    {0x9806,"AST-TX2pass","Antenna Self Test: RFLD pass threshold on TX2 (2nd parameter)",0x05},
    {0x9807,"AST-Current","Antenna Self Test: Current pass threshold (3rd parameter)",0x0E},
    {0x9809,"FRAC_ClkSel","Input Clock selection for FracNPLL\n\t\t-00h ... No input clock\n\t\t-01h ... 13.0 MHz\n\t\t-02h ... 19.2 MHz\n\t\t-03h ... 26.0 MHz\n\t\t-04h ... 38.4 MHz\n\t\t-05h ... custom\n\t\tOther values are RFU",0x00},
    {0x980A,"FRAC4_DIV","Customizable FracNPLL set",0x04},
    {0x980B,"FRAC4_OOF0","--",0xCF},
    {0x980C,"FRAC4_OOF1","--",0xF3},
    {0x980D,"FRAC4_OOF2","--",0x00},
    {0x980E,"FRAC4_CAL0","--",0x4C},
    {0x980F,"FRAC4_CAL1","--",0x54},
    {0x9810,"HW_Conf","TX-LDO configuration\n\t\t-Bits 0-1 : TVDD Supply select for TX-LDO\n\t\t00 ... 3.0 V\n\t\t01 ... 3.0 V\n\t\t10 ... 2.7 V\n\t\t11 ... 3.3 V\n\t\t-Bit 2 : Enables offset for TX-LDO, if set to 1\n\t\t-Bit 3 : Power down TX-LDO in Card-emulation mode, if set to 1\n\t\t-Bit 4 : RFU (keep unchanged)\n\t\t-Bit 5 : Enable TX-LDO limiter, if set to 1\n\t\tCLOCK configuration\n\t\t-Bit 7 : 0 ... Use internal FracNPLL for clock generation\n\t\t1 ... Use external crystal for clock generation",0xBC},
    {0x9890,"GPIO_Config_POUT","Gpio default configuration: POUT (Output Port Latch)\n\t\tRead or write the output port value.\n\t\tEach bit in the configuration register represents the GPIO pin with\n\t\tthe corresponding number\n\t\t(default value 0x00: output pins are set to 0)",0x00},
    {0x9891,"GPIO_Config_PIN","Gpio default configuration: PIN (Input Port Value)\n\t\tEach bit in the configuration register represents the GPIO pin with\n\t\tthe corresponding number\n\t\tRead the input port value",0x00},
    {0x9892,"GPIO_Config_PINV","Gpio default configuration: PINV (Port Invert)\n\t\tPort inversion register\n\t\tEach bit in the configuration register represents the GPIO pin with\n\t\tthe corresponding number",0x00},
    {0x9893,"GPIO_Config_PDIR","Gpio default configuration: PDIR (Port Direction)\n\t\tEach bit in the configuration register represents the GPIO pin with\n\t\tthe corresponding number\n\t\tIf set to 1, the corresponding GPIO is configured in output (whatever\n\t\tPEN value). If set to 0 and if the corresponding PEN bit is set to 1,\n\t\tthe corresponding GPIO is configured in input.",0x00},
    {0x9894,"GPIO_Config_UPUD","Gpio default configuration: UPUD (Pull up enable register)\n\t\tEach bit in the configuration register represents the GPIO pin with\n\t\tthe corresponding number\n\t\tSet to 1, an internal pull up is connected to the corresponding\n\t\tGPIO.",0x03},
    {0x9895,"GPIO_Config_DPUD","Gpio default configuration: DPUD (Pull Down Enable Register)\n\t\tEach bit in the configuration register represents the GPIO pin with\n\t\tthe corresponding number\n\t\tSet to 1, an internal pull down is connected to the corresponding\n\t\tGPIO",0x00},
    {0x9898,"IRQ_Config","- Bit 5 : 0 ... IRQ logical level is active HIGH (Default value)\n\t\t1 ... IRQ logical level is active LOW\n\t\t- Other Bits : RFU (keep unchanged)",0x02},
    {0x9899,"GPIO_Config_PEN","Gpio default configuration: PEN (Port Enable)\n\t\tEach bit in the configuration register represents the GPIO pin with\n\t\tthe corresponding number\n\t\tIf set to 1, and if the corresponding PDIR is set to 0, the\n\t\tcorresponding GPIO is configured in input",0x00},
    {0x989D,"AST-GSP2","Antenna Self Test: GSP for TX2",0x27},
    {0x989E,"AST_Current","Antenna Self Test: Current start threshold",0x3F},
    {0x989F,"AST-GSP1","Antenna Self Test: GSP for TX1",0x2D},
    {0x98A2,"NFCT_TO","WT value to compute RWT (See [4])\n\t\t(PNx timeout will be half as specified)",0x09},
    {0x98A3,"NFCT_PPt","Define NAD handling on target side\n\t\tSet to 0x30 to disable NAD usage",0x31},
    {0x98A4,"NFCT_RTOX","RTOX value to compute RWTINT (See [4])\n\t\t(PNx timeout will be half as specified)",0x07},
    {0x98DE,"NFCI_ATR_TO_MSB","Initiator Activation timeout (in 1ms step)\n\t\t(Recommended Max value is 30 ms).",0x00},
    {0x98DF,"--","--",0x1A},
    {0x9C0C,"Host_TIC_MSB_RX","TIC, inter-character timeout in Reception (in 3μs step)\n\t\t(When Host is writing to PNx)\n\t\t(0x0000 means TIC mechanism disabled, 0x0096 = 0,45ms)",0x00},
    {0x9C0D,"Host_TIC_LSB_RX","--",0x96},
    {0x9C12,"Host_TIC_MSB_TX","TIC, inter-character timeout in Transmission (in 3μs step)\n\t\t(When Host is reading from PNx)\n\t\t(0x0000 means TIC mechanism disabled, 0x203A = 25ms)",0x20},
    {0x9C13,"Host_TIC_LSB_TX","--",0x3A},
    {0x9C18,"SWP_Act_Retry","Maximum consecutive retries to activate SWP connection",0x03},
    {0x9C19,"Host_RX_Retry","Maximum consecutive retries on host interface RX path before\n\t\tdeactivating (soft reset)",0x0A},
    {0x9C1A,"Host_TX_Retry","Maximum consecutive retries on host interface TX path before\n\t\tdeactivating (soft reset)",0x0A},
    {0x9C1B,"SWP_RX_Retry","Maximum consecutive retries on SWP RX path before deactivating\n\t\t(soft reset)",0x0A},
    {0x9C1C,"SWP_TX_Retry","Maximum consecutive retries on SWP TX path before deactivating\n\t\t(soft reset)",0x0A},
    {0x9C22,"Timeout_S1HighV","Indicates the time between the PNx boot and the UICC First\n\t\tSWP boot (up to 255ms in 1ms steps).",0x01},
    {0x9C27,"AckHostTimeout_MSB","PNx Ack Timeout on Host link (in 1ms step).\n\t\t(0x0005 = 5ms)",0x00},
    {0x9C28,"AckHostTimeout_LSB","PNx Ack Timeout on Host link (in 1ms step).\n\t\t(0x0005 = 5ms)",0x05},
    {0x9C31,"GuardHostTimeoutMSB","Guard Host Timeout on Host link.\n\t\t(in 1ms step)\n\t\t(0x0000 means resend mechanism disabled, 0x0032 = 50ms)",0x00},
    {0x9C32,"GuardHostTimeoutLSB","Guard Host Timeout on Host link.\n\t\t(in 1ms step)\n\t\t(0x0000 means resend mechanism disabled, 0x0032 = 50ms)",0x32},
    {0x9C41,"IFSLEW","De/Activate IF0..3 pins slew rate control, default: activated\n\t\tSet to 0x00 to deactivate it",0x0F},
    {0x9E6F,"PlClockTimeout","Indicates the timeout value to be used for clock request\n\t\tacknowledgment (up to 255ms in 1ms steps).\n\t\tRecommended value is 10 ms or less.",0x0A},
    {0x9E71,"PlClockRequest","Indicates how the clock is requested to the host by the PNx.\n\t\t- 0x00 -> no request\n\t\t- 0x01 -> request through CLKREQ pin (GPIO pin 2 set to high\n\t\twhen clock requested, HighZ unless, whatever GPIO default\n\t\tconfiguration)\n\t\t- 0x02 -> request through NXP_EVT_CLK_REQUEST event\n\t\t- Others values are RFU",0x00},
    {0x9E72,"PlClockAck","Indicates how the clock request is acknowledged by the host. Clock\n\t\tacknowledgment is only valid in case of Clock request mechanism\n\t\tenabled (see NXP_PlClockRequest).\n\t\t- 0x00 -> acknowledged thanks to timeout (see PlClockTimeout)\n\t\t- 0x01 -> acknowledged through CLKACK pin (GPIO pin 1)\n\t\t- 0x02 -> acknowledged through NXP_EVT_CLK_ACK event\n\t\tOthers values are RFU",0x00},
    {0x9E74,"PlRfLowPower","Bits 0 to 2: Sensitivity of the RF Low Power Mode (value between\n\t\t0 and 6, 0 -> sensitivity maximal, 6 -> sensitivity minimal)\n\t\t- Bits 3 to 6 are RFU\n\t\t- Bit 7: use of the RF Low Power Mode (0 -> not used, 1 -> used)",0x00},
    {0x9EAA,"PWR_STATUS","Indicates PNx power modes used:\n\t\t0x00 -> PNx stays in active bat mode (except when generating\n\t\tRF field)\n\t\t0x01 -> PNx goes in standby when possible otherwise stays in\n\t\tactive bat mode\n\t\t0x02 -> RFU.\n\t\t0x03 -> RFU.",0x00},
    {0x9EB1,"FlashBuildVersion","EEPROM code identifier for FW build revision.",0x5C},
    {0x9F12,"PlMgtPassiveGTA","Guard time (in ms) to be used in detection type A phase",0x06},
    {0x9F13,"PlMgtPassiveGTB","Guard time (in ms) to be used in detection type B phase",0x06},
    {0x9F14,"PlMgtPassiveGTF","Guard time (in ms) to be used in detection type F phases\n\t\tIf previous phase on polling loop is a Felica Poll that fail on Timeout,\n\t\tyou will see an additional 5 ms delay due to the Felica timeout itself",0x14},
    {0x9F15,"PlMgtActiveGTA","Guard time (in ms) to be used in detection NFC active phase",0x06},
    {0x9F16,"PlMgtVicinityGT","Guard time (in ms) to be used in detection ISO15693 phase\n\t\tIf previous phase on polling loop is a Felica Poll that fail on Timeout,\n\t\tyou will see an additional 5 ms delay due to the Felica timeout itself",0x01},
    {0x9F19,"TO_Before_SDTBY_MSB","Timeout used to wait after last host/Uicc communication before\n\t\tgoing into standby (from 0 to 3.145s in 48μs step)",0x60},
    {0x9F1A,"TO_Before_SDTBY_LSB","Timeout used to wait after last host/Uicc communication before\n\t\tgoing into standby (from 0 to 3.145s in 48μs step)",0x00},
    {0x9F2C,"TGINIT_GUARD_TO_MSB","Target Activation timeout (from 0 to 3.145s in 48μs step)",0x51},
    {0x9F2D,"TGINIT_GUARD_TO_LSB","Target Activation timeout (from 0 to 3.145s in 48μs step)",0x61},
    {0x9F35,"Card_Emulation_TO_MSB","Time for which PNx stays in Card Emulation mode after leaving\n\t\tRF field (from 0 to 3.145s in 48μs step)",0x04},
    {0x9F36,"Card_Emulation_TO_LSB","Time for which PNx stays in Card Emulation mode after leaving\n\t\tRF field (from 0 to 3.145s in 48μs step)",0x11},
    
    {0x9F38,"MIF_KEYSET0_KEY_A","Key A of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F39,"MIF_KEYSET0_KEY_A","Key A of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F3A,"MIF_KEYSET0_KEY_A","Key A of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F3B,"MIF_KEYSET0_KEY_A","Key A of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F3C,"MIF_KEYSET0_KEY_A","Key A of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F3D,"MIF_KEYSET0_KEY_A","Key A of Keyset 0 for MIFARE authenticate command",0x00},
    
    {0x9F3E,"MIF_KEYSET0_KEY_B","Key B of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F3F,"MIF_KEYSET0_KEY_B","Key B of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F40,"MIF_KEYSET0_KEY_B","Key B of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F41,"MIF_KEYSET0_KEY_B","Key B of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F42,"MIF_KEYSET0_KEY_B","Key B of Keyset 0 for MIFARE authenticate command",0x00},
    {0x9F43,"MIF_KEYSET0_KEY_B","Key B of Keyset 0 for MIFARE authenticate command",0x00},

    {0x9F44,"MIF_KEYSET1_KEY_A","Key A of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F45,"MIF_KEYSET1_KEY_A","Key A of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F46,"MIF_KEYSET1_KEY_A","Key A of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F47,"MIF_KEYSET1_KEY_A","Key A of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F48,"MIF_KEYSET1_KEY_A","Key A of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F49,"MIF_KEYSET1_KEY_A","Key A of Keyset 1 for MIFARE authenticate command",0x00},
    
    {0x9F4A,"MIF_KEYSET1_KEY_B","Key B of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F4B,"MIF_KEYSET1_KEY_B","Key B of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F4C,"MIF_KEYSET1_KEY_B","Key B of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F4D,"MIF_KEYSET1_KEY_B","Key B of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F4E,"MIF_KEYSET1_KEY_B","Key B of Keyset 1 for MIFARE authenticate command",0x00},
    {0x9F4F,"MIF_KEYSET1_KEY_B","Key B of Keyset 1 for MIFARE authenticate command",0x00},

    {0x9F50,"MIF_KEYSET2_KEY_A","Key A of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F51,"MIF_KEYSET2_KEY_A","Key A of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F52,"MIF_KEYSET2_KEY_A","Key A of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F53,"MIF_KEYSET2_KEY_A","Key A of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F54,"MIF_KEYSET2_KEY_A","Key A of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F55,"MIF_KEYSET2_KEY_A","Key A of Keyset 2 for MIFARE authenticate command",0x00},

    {0x9F56,"MIF_KEYSET2_KEY_B","Key B of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F57,"MIF_KEYSET2_KEY_B","Key B of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F58,"MIF_KEYSET2_KEY_B","Key B of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F59,"MIF_KEYSET2_KEY_B","Key B of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F5A,"MIF_KEYSET2_KEY_B","Key B of Keyset 2 for MIFARE authenticate command",0x00},
    {0x9F5B,"MIF_KEYSET2_KEY_B","Key B of Keyset 2 for MIFARE authenticate command",0x00},

    {0x9F5C,"MIF_KEYSET3_KEY_A","Key A of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F5D,"MIF_KEYSET3_KEY_A","Key A of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F5E,"MIF_KEYSET3_KEY_A","Key A of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F5F,"MIF_KEYSET3_KEY_A","Key A of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F60,"MIF_KEYSET3_KEY_A","Key A of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F61,"MIF_KEYSET3_KEY_A","Key A of Keyset 3 for MIFARE authenticate command",0x00},

    {0x9F62,"MIF_KEYSET3_KEY_B","Key B of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F63,"MIF_KEYSET3_KEY_B","Key B of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F64,"MIF_KEYSET3_KEY_B","Key B of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F65,"MIF_KEYSET3_KEY_B","Key B of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F66,"MIF_KEYSET3_KEY_B","Key B of Keyset 3 for MIFARE authenticate command",0x00},
    {0x9F67,"MIF_KEYSET3_KEY_B","Key B of Keyset 3 for MIFARE authenticate command",0x00},

    {0x9F68,"MIF_KEYSET4_KEY_A","Key A of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F69,"MIF_KEYSET4_KEY_A","Key A of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F6A,"MIF_KEYSET4_KEY_A","Key A of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F6B,"MIF_KEYSET4_KEY_A","Key A of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F6C,"MIF_KEYSET4_KEY_A","Key A of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F6D,"MIF_KEYSET4_KEY_A","Key A of Keyset 4 for MIFARE authenticate command",0x00},

    {0x9F6E,"MIF_KEYSET4_KEY_B","Key B of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F6F,"MIF_KEYSET4_KEY_B","Key B of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F70,"MIF_KEYSET4_KEY_B","Key B of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F71,"MIF_KEYSET4_KEY_B","Key B of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F72,"MIF_KEYSET4_KEY_B","Key B of Keyset 4 for MIFARE authenticate command",0x00},
    {0x9F73,"MIF_KEYSET4_KEY_B","Key B of Keyset 4 for MIFARE authenticate command",0x00},
    
    {0x9F74,"MIF_KEYSET5_KEY_A","Key A of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F75,"MIF_KEYSET5_KEY_A","Key A of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F76,"MIF_KEYSET5_KEY_A","Key A of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F77,"MIF_KEYSET5_KEY_A","Key A of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F78,"MIF_KEYSET5_KEY_A","Key A of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F79,"MIF_KEYSET5_KEY_A","Key A of Keyset 5 for MIFARE authenticate command",0x00},
    
    {0x9F7A,"MIF_KEYSET5_KEY_B","Key B of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F7B,"MIF_KEYSET5_KEY_B","Key B of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F7C,"MIF_KEYSET5_KEY_B","Key B of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F7D,"MIF_KEYSET5_KEY_B","Key B of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F7E,"MIF_KEYSET5_KEY_B","Key B of Keyset 5 for MIFARE authenticate command",0x00},
    {0x9F7F,"MIF_KEYSET5_KEY_B","Key B of Keyset 5 for MIFARE authenticate command",0x00},

    {0x9F80,"MIF_KEYSET6_KEY_A","Key A of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F81,"MIF_KEYSET6_KEY_A","Key A of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F82,"MIF_KEYSET6_KEY_A","Key A of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F83,"MIF_KEYSET6_KEY_A","Key A of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F84,"MIF_KEYSET6_KEY_A","Key A of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F85,"MIF_KEYSET6_KEY_A","Key A of Keyset 6 for MIFARE authenticate command",0x00},

    {0x9F86,"MIF_KEYSET6_KEY_B","Key B of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F87,"MIF_KEYSET6_KEY_B","Key B of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F88,"MIF_KEYSET6_KEY_B","Key B of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F89,"MIF_KEYSET6_KEY_B","Key B of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F8A,"MIF_KEYSET6_KEY_B","Key B of Keyset 6 for MIFARE authenticate command",0x00},
    {0x9F8B,"MIF_KEYSET6_KEY_B","Key B of Keyset 6 for MIFARE authenticate command",0x00},
    
    {0x9F8C,"MIF_KEYSET7_KEY_A","Key A of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F8D,"MIF_KEYSET7_KEY_A","Key A of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F8E,"MIF_KEYSET7_KEY_A","Key A of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F8F,"MIF_KEYSET7_KEY_A","Key A of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F90,"MIF_KEYSET7_KEY_A","Key A of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F91,"MIF_KEYSET7_KEY_A","Key A of Keyset 7 for MIFARE authenticate command",0x00},

    {0x9F92,"MIF_KEYSET7_KEY_B","Key B of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F93,"MIF_KEYSET7_KEY_B","Key B of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F94,"MIF_KEYSET7_KEY_B","Key B of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F95,"MIF_KEYSET7_KEY_B","Key B of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F96,"MIF_KEYSET7_KEY_B","Key B of Keyset 7 for MIFARE authenticate command",0x00},
    {0x9F97,"MIF_KEYSET7_KEY_B","Key B of Keyset 7 for MIFARE authenticate command",0x00},

    {0x9F98,"Retry ReqB Timeout","timeout to send a another reqB on RF error or collision (unit in ms)\n\t\tnumber of reqB re done",0x01},
    {0x9F99,"max retry of Req B","--",0x03},
    {0x9F9A,"FeRd Request code","Request code used for Felica poll command in passive 212 phase\n\t\tSystem code used for Felica poll command in passive 212 phase",0x01},
    {0x9F9B,"FeRd System code MSB","--",0xff},
    {0x9F9C,"FeRd System code LSB","--",0xff},
    {0x9F9D,"TX_Current_Check","TX Over current Protection\n\t\tSet to 0x00 to disable the TX over current protection",0x02},
    {0x9F9E,"NfcT-RFOFF-TO_MSB","In NFC target active, Time the target wait after command reception\n\t\thas been ended and RF OFF before reinitializing.\n\t\tTo avoid active target in front of \n\t\tpassive reader (in 5.5μs step)",0x15},
    {0x9F9F,"NfcT-RFOFF-TO_LSB","In NFC target active, Time the target wait after command reception\n\t\thas been ended and RF OFF before reinitializing.\n\t\tTo avoid active target in front of \n\t\tpassive reader (in 5.5μs step)",0x70},
    {0x989E,"AntennaLdoCon1","Value used for antenna self test scenario 3: start value for current\n\t\tmeasurement",0x3F},
    {0x989F,"AntennaCwGsPOn","Value used for antenna self test CWGSPON register",0x20},
    {0x992F,"ANT","Switch_ANT1+ANT2_to_GND",0x12},
    {0x9976,"ANATXCWGSN","- bits 4 to 7: Set_Conductance_NMOS_LoadNoMod in PCD mode\n\t\t- Others bits are reserved",0xFF},
    {0x9977,"ANATXCWGSPON","Set_Conductance_PMOS_LoadNoMod in PCD mode",0xFF},
    {0x9979,"ANATXMODGSN","- bits 4 to 7: Set_Conductance_NMOS_LoadMod in PCD mode\n\t\t- Others bits are reserved",0xFF},
    {0x997A,"ANATXMODGSPON","Set_Conductance_PMOS_LoadMod in PCD mode",0x3E},
    {0x9F0F,"PipeLenght","Data length for pipelining",0x1C},
    {0x9F10,"Pipelining-Mode","Indicates SWP pipelining mode:\n\t\t- Bit 0 : Set to 1 to activate pipelining RF to SWP in reader mode\n\t\t- Bit 1 : Set to 1 to activate pipelining RF to SWP in card mode\n\t\t- Bit 2 : Set to 1 to activate pipelining SWP to RF in reader mode\n\t\t- Bit 3 : Set to 1 to activate pipelining SWP to RF in card mode\n\t\t- Bit 4 : Set to 1 to activate pipelining RF to host in reader mode\n\t\t- Bit 5 : Set to 1 to activate pipelining RF to host in card mode\n\t\t- Bits 6 and 7 : RFU",0x02},
    {0xF820,"PINV","Port Inversion register\n\t\tInverse the polarity",0x00},
    {0xF821,"PDIR","Port Direction Register\n\t\tIf set to 1, the corresponding GPIO is configured in output\n\t\t(whatever PEN value). If set to 0 and if the corresponding\n\t\tPEN bit is set to 1, the corresponding GPIO is configured in input.",0x00},
    {0xF822,"DPUD","Pull Down Enable register\n\t\tSet to 1, an internal pull down is connected to the corresponding GPIO.",0x00},
    {0xF823,"UPUD","Pull Up Enable Register\n\t\tSet to 1, an internal pull up is connected to the corresponding GPIO.",0x00},
    {0xF829,"PEN","Signal Input Enable\n\t\tIf set to 1, and if the corresponding PDIR is\n\t\tset to 0, the corresponding GPIO is configured in\n\t\tinput",0x00},
    {0xF82A,"PIN","Input Port Register\n\t\tRead the input port value.",0x00},
    {0xF82B,"POUT","Output Port Register\n\t\tRead or write the output port value.",0x00},
};


typedef struct _gID_
{
    uint8_t gate[100];
    uint8_t Gid;
    _PROPRIETARY standard;
    _GATE_TYPE_ gType;
}_GID_;

_GID_ Gate_Table[] =
{
    //picture 1
    //{"Link management gate",0x00, ETSI, Managemnet_gate},
    //{"Administration gate",0x01, ETSI, Managemnet_gate},
    {"Loop back gate(ETSI)",0x04, ETSI, Managemnet_gate},
    {"Identity management gate(ETSI)",0x05, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x06, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x07, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x08, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x09, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x0a, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x0b, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x0c, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x0d, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x0e, ETSI, Managemnet_gate},
    {"Managemnet gate RFU(ETSI)",0x0f, ETSI, Managemnet_gate},
    //picture 2
    {"ISO/IEC 14443 Type B reader RF gate(ETSI)",0x11, ETSI, Generic_Reader_RF_gate},
    {"ISO/IEC 14443 Type A reader RF gate(ETSI)",0x13, ETSI, Generic_Reader_RF_gate},
    //{"RFU",0x12, ETSI, Generic_Reader_RF_gate},
    //{"RFU",0x14, ETSI, Generic_Reader_RF_gate},
    {"Generic Reader RF gate RFU(ETSI)",0x1b, ETSI, Generic_Reader_RF_gate},
    {"Generic Reader RF gate RFU(ETSI)",0x7e, ETSI, Generic_Reader_RF_gate},
    {"Generic Reader RF gate RFU(ETSI)",0x7f, ETSI, Generic_Reader_RF_gate},
    //picture 3
    {"Type B card RF gate(ETSI)",0x21, ETSI, Generic_Card_RF_gate},
    {"Type B’ card RF gate(ETSI)",0x22, ETSI, Generic_Card_RF_gate},
    {"Type A card RF gate(ETSI)",0x23, ETSI, Generic_Card_RF_gate},
    {"Type F card RF gate(ETSI)",0x24, ETSI, Generic_Card_RF_gate},
    {"Generic Card RF gate RFU(ETSI)",0x25, ETSI, Generic_Card_RF_gate},
    {"Generic Card RF gate RFU(ETSI)",0x26, ETSI, Generic_Card_RF_gate},
    {"Generic Card RF gate RFU(ETSI)",0x27, ETSI, Generic_Card_RF_gate},
    {"Generic Card RF gate RFU(ETSI)",0x28, ETSI, Generic_Card_RF_gate},
    {"Generic Card RF gate RFU(ETSI)",0x29, ETSI, Generic_Card_RF_gate},
    //picture 4
    {"Connectivity gate(ETSI)",0x41, ETSI, Generic_Connectivity_gate},
    //picture 5
    {"PNxMgt gate(NXP)",0x90, NXP, Generic_System_Management_gate},
    {"PollingLoopMgt gate(NXP)",0x94, NXP, Generic_Polling_Loop_Management_gate},
    {"SWPMgt gate(NXP)",0xa0, NXP, Generic_SWP_Management_gate},
    {"NfcWiMgt gate(NXP)",0xa1, NXP, Generic_NFC_WI_Management_gate},
    //{"MIFARE UL & 1K/4K Reader gate",0x13, NXP, Generic_ISO_IEC_14443_TypeA_Reader_RF_gate},
    {"FeliCa Reader(ETSI_NXP)",0x14, ETSI_NXP, Generic_JIS6319_TypeF_Reader_RF_gate},
    {"Jewel/Topaz reader RF gate(ETSI_NXP)",0x15, ETSI_NXP, Generic_Jewel_Topaz_Reader_RF_gate},
    {"ISO/IEC 15693 reader RF gate(ETSI_NXP)",0x12, ETSI_NXP, Generic_ISO_IEC_15693_TypeV_Reader_RF_gate},
    {"NFC-IP1 gate initiator RF gate(NXP)",0x30, NXP, Generic_NFC_IP1_gate},
    {"NFC-IP1 gate target RF gate(NXP)",0x31, NXP, Generic_NFC_IP1_gate},
};

typedef struct _cLIST_
{
    uint8_t gate[2000];
    uint8_t vaule;
    _HCI_TYPE_ type;
    _GATE_TYPE_ gType;
    uint8_t parameter[2000];
    uint8_t registry_;
    uint8_t payload_len;
}_CLIST_;

_CLIST_ Check_List_[]=
{
    /********************************CDE***********************************/
    //picture 1
    //cde , all gate
    {"RFU",0x00,CDE,_All_gate_,"Parameter 2:Registry",0,2},
    {"ANY_SET_PARAMETER\n\t\tThe command to set a parameter in a registry",0x01,CDE,_All_gate_,"Parameter x:Registry",1,0xff},
    {"ANY_GET_PARAMETER\n\t\tThe command to get a parameter from a registry",0x02,CDE,_All_gate_,"Parameter 1:Registry",1,1},
    {"ANY_OPEN_PIPE\n\t\tThe command to open a pipe",0x03,CDE,_All_gate_,"No parameter",0,0},
    {"ANY_CLOSE_PIPE\n\t\tThe command to close a pipe",0x04,CDE,_All_gate_,"No parameter",0,0},
    {"RFU",0x05,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x06,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x07,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x08,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x09,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x0a,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x0b,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x0c,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x0d,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x0e,CDE,_All_gate_,"RFU",0,0},
    {"RFU",0x0f,CDE,_All_gate_,"RFU",0,0},
    //cde , admin gate
    {"ADM_CREATE_PIPE\n\t\tThe administration command to create a dynamic pipe",0x10,CDE,Managemnet_gate,"Parameter 3:Source gID + destination hID + destination gID",0,3},    
    {"ADM_DELETE_PIPE\n\t\tThe administration command to delete a dynamic pipe",0x11,CDE,Managemnet_gate,"Parameter 1:pID",0,1},
    {"ADM_NOTIFY_PIPE_CREATED\n\t\tThe administration command to notify the creation\n\t\tof a dynamic pipe",0x12,CDE,Managemnet_gate,"Parameter 5:Source hID + Source gID + destination hID + destination gID + pID",0,5},    
    {"ADM_NOTIFY_PIPE_DELETED\n\t\tThe administration command to notify the deletion\n\t\tof a dynamic pipe",0x13,CDE,Managemnet_gate,"Parameter 1:pID",0,1},
    {"ADM_CLEAR_ALL_PIPE\n\t\tThe command to close/delete all pipes connected\n\t\tto a requesting host",0x14,CDE,Managemnet_gate,"No parameter",0,0},
    {"ADM_NOTIFY_ALL_PIPE_CLEARED\n\t\tThe command to notify the closing/deletion of all\n\t\tpipes connected to a requesting host",0x15,CDE,Managemnet_gate,"No parameter",0,0},
    //0x16 ~ 0x3f for RFU

    //Picture 2 * A
    {"WR_XCHG_DATA\n\t\tExchange of data from a reader application gate\n\t\tto the reader RF gate",0x10,CDE,Generic_Reader_RF_gate,"Parameter 1:CTR",1,1},

    //Picture 4
    {"PRO_HOST_REQUEST\n\t\tThis command allows a host to request the terminal host to\n\t\tactivate during a limited time one or several other hosts\n\t\tin order to interact with them.",0x10,CDE,Generic_Connectivity_gate,"Parameter 2:Activation Duration(ms) + List of host identifiers",0,2},

    //Picture 5
    {"NXP_SELF_TEST_ANTENNA\n\t\tPerform antenna self test",0x20,CDE,Generic_System_Management_gate,"Parameter 4:Boundary Conditions\n\t\tBoundary conditions used during Antenna Self test:\n\t\t- 1st one relates to RF sensitivity on TX1.\n\t\t- 2nd one relates to RF sensitivity on TX2.\n\t\t- 3rd one relates to Current detection level on TX1 & TX2.\n\t\t- 4th one is RFU This parameter is optional,\n\t\tin case it is not set in the command the latest set\n\t\tvalues are used to perform the test (default values are\n\t\t0x03 0x05 0x0E 0xFF).\n\t\tReturn : ErrorCode + Measured Values Len: 1 + 4ErrorCode\n\t\tfield indicates which condition\n\t\thas failed during the test:\n\t\t- 0x00: No failure.\n\t\t- 0x79: 1st condition failed (RF sensitivity on TX1),\n\t\tbut does not have any signification for pass/fail\n\t\tof 2nd and 3 rd conditions.\n\t\t- 0x7A: 1st condition passed, 2nd condition failed\n\t\t(RF sensitivity on TX2),\n\t\tbut does not have any signification for pass/fail\n\t\tof 3 rd condition.\n\t\t- 0x7B: 1st and 2nd conditions passed, 3rd condition\n\t\tfailed(Current detection level on TX1 & TX2).\n\t\tMeasured Values: Last measured value ring self Test,\n\t\tthese values can be used\n\t\tto run the antenna test again in case of failure.",0,4},
    {"NXP_SELF_TEST_SWP\n\t\tPerform SWP self test. Purpose of the test is to check\n\t\tSWIO pin connectivity.Principle is to communicate (SWP\n\t\tactivation procedure)connected over SWP interface.\n\t\tErrorcode field indicates which condition has\n\t\tmade the test failing:\n\t\t- 0x00: No failure\n\t\t- 0x01: SWP self test failed\n\t\tPmuVccStatus field indicates if the PmuVcc\n\t\tis present or not:\n\t\t- 0x00: PmuVcc not present\n\t\t- 0x01: PmuVcc present\n\t\tNote: Pay attention that to make the detection working,\n\t\tPmuVcc must be present when PNx is booting.",0x21,CDE,Generic_System_Management_gate,"No parameter",0,0},
    {"NXP_PRBS_TEST\n\t\tPerform PRBS test sequence. As soon as the test is started,\n\t\tPNx generates PRBS9 [511 bits] data stream over the\n\t\tcontactless interface.PRBS test is ended via PNx\n\t\texternal reset (when the test is started no response\n\t\tto this command is sent by PNx to the host).",0x25,CDE,Generic_System_Management_gate,"Parameter 2:Technology + Bitrate\n\t\tTechnology Parameter:\n\t\t0x00 Type A\n\t\t0x01 Type B\n\t\t0x02 Type F\n\t\t0x03 No modulation\n\t\tothers RFU\n\t\tBitrate Parameter :\n\t\t0x00 106 kbps\n\t\t0x01 212kbps\n\t\t0x02 424 kbps\n\t\tothers RFU",0,2},
    {"NXP_READ\n\t\tThis command is used by the host to read PNx memory",0x3e,CDE,Generic_System_Management_gate,"Parameter 3:Regsiter address",0,3},
    {"NXP_WRITE\n\t\tThis command is used by the host to write PNx memory",0x3f,CDE,Generic_System_Management_gate,"Parameter 4:Register address 3 + Value 1",0,4},

    {"NXP_MIFARE_RAW\n\t\tThis command allows the exchange of raw data between the\n\t\thost and a MIFARE ? card previously activated. Within this\n\t\tcommand and response,the CRC has to be handled\n\t\tby the host.",0x20,CDE,Generic_ISO_IEC_14443_TypeA_Reader_RF_gate,"Parameters 3:TO + Status + Data",0,3},
    {"NXP_MIFARE_CMD\n\t\tThis command allows the exchange of data between the host\n\t\tand a MIFARE ? card previously activated.",0x21,CDE,Generic_ISO_IEC_14443_TypeA_Reader_RF_gate,"Parameters 3:CMD + Addr + Data",0,3},
    {"NXP_MIFARE_AUTHENTICATE_CMD\n\t\tThis command allows authenticate sector of a MIFARE ?\n\t\tcard with key stored in EEPROM (See 151).",0x22,CDE,Generic_ISO_IEC_14443_TypeA_Reader_RF_gate,"Parameters x: Address 1+ Authentication type 1 + Key to use 1+UID 4 or 7",0,0xff},
    {"NXP_WRA_CONTINUE_ACTIVATION\n\t\tThe host sends this command to inform the CLF Controller after\n\t\thaving received the event EVT_TARGET_DISCOVERED to continue\n\t\tactivation in case activation has been stopped after successful\n\t\tSAK response.\n\t\tThe response to this command, sent as soon as the activation\n\t\tis finished,indicates the result of the\n\t\tactivation procedure.",0x12,CDE,Generic_ISO_IEC_14443_TypeA_Reader_RF_gate,"No parameter",0,0},

    {"NXP_FELICA_RAW\n\t\tThis command allows the exchange of raw data between the host\n\t\tand a FeliCa card previously activated.",0x20,CDE,Generic_JIS6319_TypeF_Reader_RF_gate,"Parameters x:TO 1 + Status 1 + Data N",0,0xff},
    {"NXP_FELICA_CMD\n\t\tThis command allows the exchange of data between the host\n\t\tand a FeliCa card previously activated.",0x21,CDE,Generic_JIS6319_TypeF_Reader_RF_gate,"Parameters x:Cmd 1 + Data N",0,0xff},

    {"NXP_JEWEL_RAW_CMD\n\t\tThis command is used by the host to use Jewel native\n\t\tcommand in a RAW Format.",0x23,CDE,Generic_Jewel_Topaz_Reader_RF_gate,"Parameters x:Cmd 1 + Data N",0,0xff},

    {"NXP_ISO15693_CMD\n\t\tThis command is used by the host to access remote tag.",0x20,CDE,Generic_ISO_IEC_15693_TypeV_Reader_RF_gate,"Parameters x:Payload N",0,0xff},

    {"NXP_WR_PRESCHECK\n\t\tThe host sends this command to check the remote card\n\t\tis still present in the field. The response to this command\n\t\tindicates the result of the Presence Check procedure. This\n\t\tcommand is only valid for ISO14443-4 compliant card, otherwise\n\t\terror ANY_E_CMD_NOT_SUPPORTED is returned as response.",0x30,CDE,Generic_Reader_RF_gate,"No parameter",0,0},
    {"NXP_WR_ACTIVATE_NEXT\n\t\tThe host sends this command to activate next remote card\n\t\t(in case of several cards in the field). The response to\n\t\tthis command indicates the result of the activation procedure\n\t\tof next card (when the response is sent, next remote card\n\t\thas been activated). Parameter in the response\n\t\tindicates if there is still other cards in the field (previous\n\t\tactivated cards will not be visible). By successive use of this\n\t\tcommand, host is able to successively activate all cards within\n\t\ta technology.If this command is sent when no more other card is\n\t\tpresent in the field (no collision detected) this command will\n\t\treturn an error.",0x31,CDE,Generic_Reader_RF_gate,"No parameter",0,0},
    {"NXP_WR_ACTIVATE_ID\n\t\tThe host sends this command to select a previously detected\n\t\tremote card (by means of NXP_WR_ACTIVATE_NEXT command) using\n\t\tthe ID of the card.",0x32,CDE,Generic_Reader_RF_gate,"Parameters x:ID N",0,0xff},
    
    {"NXP_NFCI_ATTREQUEST\n\t\tThe host sends this command to check the target is still\n\t\tpresent in the field (see[4] §12.6.3 Attention Target present)\n\t\tThe response to this command indicates the result of the\n\t\tAttention procedure.",0x12,CDE,Generic_NFC_IP1_gate,"No parameter",0,0},
    {"NXP_NFCI_CONTINUE_ACTIVATION\n\t\tThe host sends this command to allow NFCIP-1 activation\n\t\twhen a NFC target has been discovered in Type A Reader\n\t\tRF gate or type F Reader RF gate.Type A: In\n\t\tcase the proprietary NXP_AUTO_ACTIVATION registry entry\n\t\tof the Type A Reader RF gate has been set to 0 (stop activation\n\t\tafter SAK is received),the EVT_TARGET_DISCOVERED will be\n\t\tsent to theType A Reader application gate even\n\t\tif the remote target is an NFC device. Type F: In case a\n\t\tpipe has been opened on the Type F Reader RF gate, the\n\t\tactivation procedure is stopped after\n\t\tthe POLL_REQ and the EVT_TARGET_DISCOVERED will be sent to the\n\t\tType F Reader application gate even if the remote\n\t\ttarget is an NFC device. Then, in order to be able\n\t\tto communicate with the remote NFC target, the host has\n\t\tto send the command NXP_NFCI_CONTINUE_ACTIVATION to the\n\t\tNFCIP1 Initiator RF gate to indicate\n\t\tto the CLF to continue NFCIP1 activation (ATR/PSL exchange).\n\t\tThe response to this command, sent as soon as the\n\t\tactivation is finished, indicates the result of the\n\t\tactivation procedure.",0x13,CDE,Generic_NFC_IP1_gate,"No parameter",0,0},    
    /********************************STS***********************************/
    //picture 1
    {"ANY_OK\n\t\tCommand completed successfully (with optional parameters)",0x00,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ANY_E_NOT_CONNECTED\n\t\tThe destination host is not connected",0x01,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ANY_E_CMD_PAR_UNKNOWN\n\t\tThe format of the command parameters is wrong",0x02,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ANY_E_NOK\n\t\tCommand was rejected and/or not completed",0x03,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ADM_E_NO_PIPES_AVAILABLE\n\t\tNo more dynamic pipes can be created",0x04,STS,_All_gate_,"No parameter*(Please refer previous command to analyze)",0,0},
    {"ANY_E_REG_PAR_UNKNOWN\n\t\tThe registry parameter identifier is either unknown to the\n\t\tregistry or an optional registry parameter is not implemented",0x05,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ANY_E_PIPE_NOT_OPENED\n\t\tThe pipe is not open",0x06,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ANY_E_CMD_NOT_SUPPORTED\n\t\tThe command is not supported by the gate",0x07,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ANY_E_INHIBITED\n\t\tCommand is inhibited due to failure of lower layer\n\t\tidentity check",0x08,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ANY_E_TIMEOUT\n\t\tAn application level time-out occurred",0x09,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ANY_E_REG_ACCESS_DENIED\n\t\tPermission denied to write/read a value to/from a registry",0x0a,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    {"ANY_E_PIPE_ACCESS_DENIED\n\t\tPermission denied to create a pipe due to\n\t\ta WHITELIST violation",0x0b,STS,_All_gate_,"No parameter(Please refer previous command to analyze)",0,0},
    //0x0C~0x3F RFU

    {"WR_RF_ERROR\n\t\tThe target has returned an RF error (WOP)",0x10,STS,Generic_Reader_RF_gate,"No parameter(Please refer previous command to analyze)",0,0},
    /********************************EVT***********************************/
    {"EVT_HCI_END_OF_OPERATION\n\t\tThis event shall be sent by a host to the host controller when\n\t\tthe host requires no more activity on the HCI interface\n\t\tand its underlying layer at the time\n\t\tthe event is sent. This event has no parameters.",0x01,EVT,Managemnet_gate,"No parameter",0,0},
    {"EVT_POST_DATA\n\t\tThis event shall be used to send and receive data with a\n\t\tparameter that contains a block of data of variable size",0x02,EVT,Managemnet_gate,"No parameter",0,0},
    {"EVT_HOT_PLUG\n\t\tThis event shall be sent by the host controller to any other\n\t\tconnected host to notify the connection or disconnection of\n\t\ta host to the host controller. This event has no parameters.",0x03,EVT,Managemnet_gate,"No parameter",0,0},

    {"EVT_READER_REQUESTED\n\t\tWith this event the host signals to the CLF that a reader\n\t\tapplication gate is requesting to activate the RF polling\n\t\t(turn on the RF carrier).The method and the algorithm\n\t\tused by the reader RF gates to achieve target\n\t\tpolling are implementation dependant and out of\n\t\tthe scope of the present document.\n\t\tThis event has no parameters and may be sent over any\n\t\topen pipe of any reader RF gate.",0x10,EVT,Generic_Reader_RF_gate,"No parameter",0,0},
    {"EVT_END_OPERATION\n\t\tThe host sends this event to inform the CLF Controller of the\n\t\tend of operation.Upon reception of the event the CLF controller\n\t\tshall turn the RF field OFF if the EVT_TARGET_DISCOVERED\n\t\thas been previously sent to that specific host,otherwise\n\t\tthe CLF may turn the RF field OFF and move forward\n\t\tin its internal processing.",0x11,EVT,Generic_Reader_RF_gate,"No parameter",0,0},
    {"EVT_TARGET_DISCOVERED\n\t\tThis event signals to the reader application gate the\n\t\texistence of an RF target in the field of the activated RF\n\t\ttechnology. The event notifies the reader application gate\n\t\tabout the availability of the RF information in the registry.\n\t\tThe activation of the target is completed and the associated\n\t\tregistry may inform the application about the RF feature\n\t\tof the application.",0x10,EVT,Generic_Reader_RF_gate,"Parameter 1:Status",0,1},

    {"EVT_SEND_DATA\n\t\tSending of data to the CLF",0x10,EVT,Generic_Card_RF_gate,"Parameter 2:data that are sent + RF error indicator",0,2},
    {"EVT_FIELD_ON\n\t\tThis event signals that the CLF detected an RF carrier\n\t\tfrom an external reader. This event shall be sent by\n\t\tthe host controller within 2ms after the detection of\n\t\tan RF field. if SWP is in DEACTIVATED state, the CLF shall\n\t\tactivate the interface instead of sending the EVT_FIELD_ON.",0x11,EVT,Generic_Card_RF_gate,"No parameter",0,0},
    {"EVT_CARD_DEACTIVATED\n\t\tThis event signals that the CLF is deactivated as defined in\n\t\tISO/IEC 14443-3 [6] for type B or ISO/IEC 14443-4 [7]\n\t\tfor type A.",0x12,EVT,Generic_Card_RF_gate,"No parameter",0,0},
    {"EVT_CARD_ACTIVATED\n\t\tThis event signals that the CLF is activated as defined\n\t\tin ISO/IEC 14443-3 [6] for type B or ISO/IEC 14443-4 [7]\n\t\tfor type A.",0x13,EVT,Generic_Card_RF_gate,"No parameter",0,0},
    {"EVT_FIELD_OFF\n\t\tThis event signals that the CLF detected the loss of the RF\n\t\tcarrier from the external reader.",0x14,EVT,Generic_Card_RF_gate,"No parameter",0,0},
    
    {"EVT_CONNECTIVITY\n\t\tThis event notifies the terminal host that it shall send a\n\t\t(HCI connectivity event) as defined in TS 102 223 [3].",0x10,EVT,Generic_Connectivity_gate,"No parameter",0,0},
    {"EVT_TRANSACTION\n\t\tThis event notifies the terminal host that it shall launch\n\t\tan application associated to an NFC application in a UICC\n\t\thost identified by the AID on the parameter list. NOTE:\n\t\tThis method to launch an application does not replace the\n\t\tlaunch application supported by a proactive session as defined\n\t\tin TS 102 223 [3]. This event sends a list of parameters\n\t\tcoded in BER-TLV fields as defined in TS 101 220 [9].\n\t\tThe tag and length ofthese fields are defined.\n\t\tDescription Tag Length\n\t\tAID '81' 5 to 16\n\t\tPARAMETERS '82' 0 to 255",0x12,EVT,Generic_Connectivity_gate,"No parameter",0,0},
    {"EVT_OPERATION_ENDED\n\t\tThis event notifies the terminal host that the interaction\n\t\twith the hosts listed in the command PRO_HOST_REQUEST is\n\t\tended before the activation duration time limit.",0x13,EVT,Generic_Connectivity_gate,"No parameter",0,0},
    {"EVT_STANDBY\n\t\tThis event should be used by the terminal host or the host\n\t\tcontroller to notify the host that it shall stop\n\t\tany ongoing communication with the other hosts and the\n\t\thost controller after 100 ms. The terminal host shall\n\t\ttake care about not interrupting ongoing\n\t\tcontactless transactions.",0x10,EVT,Generic_Connectivity_gate,"No parameter",0,0},

    {"NXP_EVT_SET_AUTONOMOUS\n\t\tThis event is sent by the host to put the system in\n\t\tAutonomous mode (Event is set implicitly in case PVDD is\n\t\tswitched off). Having this mode set next SWP activation\n\t\twill be done using the low power notification towards the UICC.\n\t\tMode is reset by sending a RSET command.",0x01,EVT,Generic_System_Management_gate,"No parameter",0,0},
    {"NXP_EVT_INFO_PMUVCC\n\t\tThis event is sent to the host to inform about PmuVcc\n\t\tsignal switch which can then impact SimVcc (implies internal\n\t\treactivation of the SWP link).",0x11,EVT,Generic_System_Management_gate,"Parameter 1:Status",0,1},
    {"NXP_EVT_INFO_EXT_RF_FIELD\n\t\tThis event is sent to the host to inform about external\n\t\tRF field presence. This notification is sent, if at\n\t\tleast one Card Emulation is enabled.",0x12,EVT,Generic_System_Management_gate,"Parameter 1:Presence",0,1},
    {"NXP_EVT_INFO_MEM_VIOLATION4\n\t\tThis event is sent to the host to inform about PNx\n\t\tinternal memory violation.This violation could have\n\t\tbeen caused by:\n\t\t- EEPROM memory corruption(protected by CRC, see\n\t\tNXP_INFO_EEPROM_ERR registry entry)\n\t\t- Wrong memory access (cause internal PNx reboot)",0x13,EVT,Generic_System_Management_gate,"Parameter 1:Status",0,1},
    {"NXP_EVT_INFO_TEMP_OVERHEAT\n\t\tThis event is sent to the host to inform about temperature\n\t\toverheat. This issue is triggering the internal\n\t\tprotection mode.",0x14,EVT,Generic_System_Management_gate,"No parameter",0,0},
    {"NXP_EVT_INFO_LLC_ERR\n\t\tThis event is sent to the host to inform LLC error counter\n\t\tis reached on SWP.",0x15,EVT,Generic_System_Management_gate,"No parameter",0,0},
    
    {"NXP_EVT_CLK_ACK\n\t\tThis event is sent by the host to acknowledge the clock request\n\t\t(if PlClockAck EEPROM area has been previously set to 2)",0x01,EVT,Generic_Polling_Loop_Management_gate,"No parameter",0,0},
    {"NXP_EVT_CLK_REQUEST\n\t\tThis event is sent to the host to inform that the input\n\t\tclock is requested (if PlClockRequest EEPROM area has been\n\t\tpreviously set to 2).",0x02,EVT,Generic_Polling_Loop_Management_gate,"Parameter 1:Status",0,1},
    {"NXP_EVT_ACTIVATE_RDPHASE\n\t\tThis event is sent to the host to inform that the SE\n\t\tis requesting for the activation of the reader phases\n\t\tin the polling loop (see NXP_PL_RDPHASES registry entry).",0x03,EVT,Generic_Polling_Loop_Management_gate,"Parameters 1:Technology",0,1},
    {"NXP_EVT_DEACTIVATE_RDPHASE\n\t\tThis event is sent to the host to inform that the SE\n\t\tis no more requesting for the activation of the reader phases\n\t\tin the polling loop (see NXP_PL_RDPHASES registry entry).",0x04,EVT,Generic_Polling_Loop_Management_gate,"No parameter",0,0},

    {"NXP_EVT_SWP_SWITCH_MODE\n\t\tThis event is sent by the host to temporary activate/deactivate\n\t\tSWP link.Warning: PNx cannot go back to Standby mode if\n\t\t?Default mode? is not set.It means that ?Off mode? &\n\t\t?On mode? will keep PNx in ActiveBAT.",0x03,EVT,Generic_SWP_Management_gate,"Parameters 1:Mode",0,1},
    {"NXP_EVT_SWP_PROTECTED\n\t\tThis event is sent to the host, when protected mode\n\t\thas been enabled (see related NXP_SWP_PROTECTION_MODE\n\t\tregistry entry),to inform a RF-SWP transaction has been blocked",0x04,EVT,Generic_SWP_Management_gate,"No parameter",0,0},

    {"NXP_EVT_SE_START_OF_TRANSACTION\n\t\tThis event is sent to the host to inform that a\n\t\ttransaction starts.",0x01,EVT,Generic_NFC_WI_Management_gate,"No parameter",0,0},
    {"NXP_EVT_SE_END_OF_TRANSACTION\n\t\tThis event is sent to the host to inform that the\n\t\ttransaction ends.",0x02,EVT,Generic_NFC_WI_Management_gate,"No parameter",0,0},
    {"NXP_EVT_SE_SWITCH_MODE\n\t\tThis event is sent by the host to temporary switch NFC-WI\n\t\tmode.Warning: PNx cannot go back to Standby mode if\n\t\t?Default mode? is not set. It means that ?Wired mode?,\n\t\t?Virtual mode? & ?Off mode? will keep PNx in ActiveBAT.",0x03,EVT,Generic_NFC_WI_Management_gate,"Parameters 1:Mode",0,1},
    {"NXP_EVT_SE_TRANSACTION\n\t\tThis event is sent to the Host to inform that a\n\t\ttransaction is on-going (triggered by reception of\n\t\ta select AID).",0x04,EVT,Generic_NFC_WI_Management_gate,"No parameter",0,0},

    {"NXP_EVT_NFC_SND_DATA\n\t\tThis event conveys payload from the host to the CLF.\n\t\tIn Initiator mode this leads to a DEP_REQ (information PFB).\n\t\tIn Target mode this leads to a DEP_RES (information PFB).",0x01,EVT,Generic_NFC_IP1_gate,"No parameter",0,0},
    {"NXP_EVT_NFC_ACTIVATED\n\t\tThis event is sent to the host to notify that NFCIP-1\n\t\tprotocol has been activated. Meaning that, in initiator\n\t\tmode it has activated a remote NFC target, and in\n\t\ttarget mode it has been activated by a remote\n\t\tNFC initiator",0x02,EVT,Generic_NFC_IP1_gate,"Parameter 1:Mode",0,1},
    {"NXP_EVT_NFC_DEACTIVATED\n\t\tThis event is sent by the NFC target to its host to\n\t\tnotify that the connection has been closed.The NFCIP-1\n\t\tlink deactivation might have several reasons:\n\t\t- The remote NFC Initiator has explicitly performed a\n\t\tlink deactivation\n\t\t- The NFCIP-1 link has detected an error and it was not\n\t\table to recover the communication on target side",0x03,EVT,Generic_NFC_IP1_gate,"Parameter x: MI 1 + Data N",0,0xff},
    {"NXP_EVT_NFC_RCV_DATA\n\t\tThis event conveys payload from the CLF to the host. In\n\t\tInitiator mode this event has been generated due\n\t\tto a DEP_RES. In Target mode this event has been\n\t\tgenerated because of a DEP_REQ.",0x04,EVT,Generic_NFC_IP1_gate,"Parameter x: RF error indicator 1 + MI 1 + Data N",0,0xff},
    {"NXP_EVT_NFC_CONTINUE_MI\n\t\tThis event is sent to the host to indicate the remote\n\t\tpeer acknowledged the previous Meta-chained packet\n\t\tand then the host can send the next one.",0x05,EVT,Generic_NFC_IP1_gate,"No parameter",0,0},
};
//#define CRC_A 0
#define CRC_B 1
#define I_Frame_Mask            0x80
#define S_Frame_Mask            0xC0
#define U_Frame_Mask            0xE0

static void _hci_byte_Crc(
    uint8_t     crcByte, 
    uint16_t    *pCrc
)
{
    crcByte = (crcByte ^ (uint8_t)((*pCrc) & 0x00FF));
    crcByte = (crcByte ^ (uint8_t)(crcByte << 4));
    *pCrc = (*pCrc >> 8) ^ ((uint16_t)crcByte << 8) ^
                ((uint16_t)crcByte << 3) ^
                ((uint16_t)crcByte >> 4);
}

void _hci_Crc(
    uint8_t     *pData, 
    uint8_t     length,
    uint8_t     *pCrc1, 
    uint8_t     *pCrc2
)
{
    uint8_t     crc_byte = 0, 
                index = 0;
    uint16_t    crc = 0;

#ifdef CRC_A
        crc = 0x6363; /* ITU-V.41 */
#else
        crc = 0xFFFF; /* ISO/IEC 13239 (formerly ISO/IEC 3309) */
#endif /* #ifdef CRC_A */

    do 
    {
        crc_byte = pData[index];
        _hci_byte_Crc(crc_byte, &crc);
        index++;
    } while (index < length);

#ifndef INVERT_CRC
    crc = ~crc; /* ISO/IEC 13239 (formerly ISO/IEC 3309) */
#endif /* #ifndef INVERT_CRC */

    *pCrc1 = (uint8_t) (crc & 0xFF);
    *pCrc2 = (uint8_t) ((crc >> 8) & 0xFF);
    return;
}

char ConvertHexChar(char ch)     
{   
  if((ch>='0')&&(ch<='9'))   
  return   ch-0x30;   
  else   if((ch>='A')&&(ch<='F'))   
  return   ch-'A'+10;   
  else   if((ch>='a')&&(ch<='f'))   
  return   ch-'a'+10;   
  else   return   (-1);   
}

void SetColor(unsigned short color)
{
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hcon,color);
}
#define RELEASE 1

static void usage(void)
{
	printf( "Usage:\n"
	"\tDescription:[Length] + [Header] + [Payload] + [CRC16]\n"
	"\tFor example input [05 f9 04 00 c3 e5]\n"
    "\tSupported to ETSI TS 102 622 V10.2.0.\n"
#if defined RELEASE
	"\tIf you meet some problem, please contact nfcsphinx@gamil.com\n\n");
#else
	"\tIf you meet some problem, please contact xiaohua.wang@nxp.com\n"
	"\tCopyright (c) 2011, NXP Semiconductors N.V.\n\n");
#endif
}

static void error(_ERROR_TYPING_ arg)
{
    SetColor(12);
	if(arg == ERROR_TYPE_)
	printf( "\tError:Argument incorrect!!!\n");
	else if(arg == ERROR_MAX_ || arg == ERROR_MIN_)
	printf( "\tError:Over 32 argument or less than 3 argument!!!\n");
    else if(arg == ERROR_LEN_)
    printf( "\tError:Length incorrect!!!\n");
    else if(arg == ERROR_CRC_)
    printf( "\tError:CRC incorrect!!!\n");
    else if(arg == ERROR_SF_)
    printf( "\tError:S-Frames incorrect!!!\n");
    else if(arg == ERROR_OPT_)
    printf( "\tError:Type and optional incorrect!!!\n");
    else if(arg == ERROR_WIN_)
    printf( "\tError:Windows size incorrect!!!\n");
    else if(arg == ERROR_BAU_)
    printf( "\tError:Baud rate incorrect!!!\n");
	else
	printf( "\tError:Unknow!!!\n");

	printf( "\tPlease input again!!!\n");
    SetColor(7);
    return;
}
//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char* argv[])
{
	uint8_t crc1 = 0x0;
	uint8_t crc2 = 0x0;
	uint8_t str[200] = {0x0};
	uint16_t i = 0x0;
    uint16_t j = 0x0;
    uint16_t k = 0x0;
    uint16_t flag[10];//maximum 10
    _HCI_TYPE_ type = _HCI_TYPE_UNKNOW_;
    uint16_t address = 0x0;
    uint8_t vaule =0x0;

    memset(flag,0xffff,sizeof(flag));
	usage();
	if(argc > 34 || argc < 3)//32###
	{error(ERROR_MAX_);return 0;}
/*
08-16 19:13:03.199: D/NFC-HCI(324): > SEND: 20 A4 05 10 00 84 E4 00 00 18 32 C3 8A B9 51 6B 17 3C B4 D2 55 E0 2F 28 FC 64 F4 65 70 E2 E9 33 09               
08-16 19:13:03.203: D/NFC-HCI(324): > SEND: 07 AC 85 05 C4 1C 09 7B       
08-16 19:13:03.207: D/NFC-HCI(324): > RECV: 03 
08-16 19:13:03.210: D/NFC-HCI(324): > RECV: C6 15 86     
08-16 19:13:09.636: D/NFC-HCI(324): > RECV: 06 
08-16 19:13:09.636: D/NFC-HCI(324): > RECV: A6 85 80 01 5C B5   
08-16 19:13:09.636: E/NFC JNI(324): TRANSCEIVE error
08-16 19:13:09.636: D/NFC-HCI(324): > SEND: 03 C5 8E B4               
08-16 19:13:09.640: W/System.err(649): java.io.IOException: Exchange APDU failed
*/
	for(i=0;i<argc - 1;i++)
	{
		if((((char) *(argv[i+1]+2)) != 0x0) || ConvertHexChar((char) *argv[i+1]) == -1 || ConvertHexChar((char) *(argv[i+1]+1)) == -1)
		{error(ERROR_TYPE_);return 0;}
        
        str[i] = ((ConvertHexChar((char) *argv[i+1])&0x0f) << 0x4) | (ConvertHexChar((char) *(argv[i+1]+1))&0x0f);
        if(str[0] != argc - 2)
        {error(ERROR_LEN_);return 0;}

        if((i%8) == 0 && i != 0)
        {
            printf("\n\t");
        }
        if(i == 0)
        {
            SetColor(6);
            printf("\t0x%02x ",str[i]);
        }
        else
        {
            printf("0x%02x ",str[i]);
        }
    }
    printf("\n");

	_hci_Crc(str,argc - 3,&crc1,&crc2);//reduce crc
    if(str[argc - 3] != crc1 || str[argc - 2] != crc2)
    {error(ERROR_CRC_);return 0;}

	SetColor(3);
	//printf("\tCRC:[0x%x,0x%x]\n",crc1,crc2);
    
    printf("\t--0x%02x  Indicates package valid range",str[0]);
    SetColor(4);
    switch(str[1] & U_Frame_Mask)
    {
    case U_Frame_Mask:
        SetColor(2);
        if(str[1] == 0xf9)
        {
            printf("\n\t--0x%02x  U-Frames RSET(Reset)",str[1]);
        }
        else if(str[1] == 0xe6)
        {
            printf("\n\t--0x%02x  U-Frames UA(Unnumbered Acknowledgment)",str[1]);
        }
        SetColor(6);
        break;

    case S_Frame_Mask:
        printf("\n\t--0x%02x  S-Frames",str[1]);
        //s_data = str[1] & 0xf8;
        if((str[1] & 0xf8) == 0xc0)
        {
            printf("\n\t  0x%02x  RR(Receive Ready)",str[1] & 0xf8);
        }
        else if((str[1] & 0xf8) == 0xc8)
        {
            printf("\n\t  0x%02x  REJ(Reject)",str[1] & 0xf8);
        }
        else if((str[1] & 0xf8) == 0xd0)
        {
            printf("\n\t  0x%02x  RNR(Receive Not Ready)",str[1] & 0xf8);
        }
        else if((str[1] & 0xf8) == 0xd8)
        {
            printf("\n\t  0x%02x  SREJ*(Selective Reject,PNx does support)",str[1] & 0xf8);
        }
        else
        {printf("\n");error(ERROR_SF_);return 0;}
        printf("\n\t  0x%02x  NR(Number receive)",str[1] & 0x7);
        break;

    default:
        printf("\n\t--0x%02x  I-Frames",str[1]);
        printf("\n\t  0x%02x  NS(Number Send)",(str[1] >> 3) & 0x7);
        printf("\n\t  0x%02x  NR(Number Receive)",str[1] & 0x7);
        break;
    }
    SetColor(6);
    if(str[0] > 0x3 && str[1] != 0xf9)
    {
        //cb, pid
        printf("\n\t--0x%02x  CB and pID",str[2]);
        if((str[2] & 0x80) >> 7 == 0x1)
        {printf("\n\t  0x%02x  Single message or end fragment",(str[2] & 0x80) >> 7);}
        else
        {printf("\n\t  0x%02x  Fragments message",(str[2] & 0x80) >> 7);}
        // To do here
        if((str[2] & 0x7f) == 0x00)
        {printf("\n\t  0x%02x  Link management gate(Static)",str[2] & 0x7f);}
        else if((str[2] & 0x7f) == 0x01)
        {printf("\n\t  0x%02x  Administration gate(Static)",str[2] & 0x7f);}
        else if((str[2] & 0x7f) >= 0x02 && (str[2] & 0x7f) <= 0x6f)
        {printf("\n\t  0x%02x  Other gates(Dynamic,0x02~0x6F)",str[2] & 0x7f);}
        else
        {printf("\n\t  0x%02x  RFU(Dynamic,0x70~0x7F,* PID bit7~bit1)",str[2] & 0x7f);}

        // type cde/sts/evt
        SetColor(2);
        printf("\n\t--0x%02x  Type and optional",str[3]);
        if((str[3] & 0xc0) >> 6 == 0x0)
        {printf("\n\t  0x%02x  Commands type(CDE)",(str[3] & 0xc0) >> 6);type = CDE;}
        else if((str[3] & 0xc0) >> 6 == 0x1)
        {printf("\n\t  0x%02x  Events type(EVT)",(str[3] & 0xc0) >> 6);type = EVT;}
        else if((str[3] & 0xc0) >> 6 == 0x2)
        {printf("\n\t  0x%02x  Responses commands type(STS)",(str[3] & 0xc0) >> 6);type = STS;}
        else
        {printf("\n");error(ERROR_OPT_);return 0;}
        // Optional
        
        for(i=0;i<sizeof(Check_List_) / sizeof(Check_List_[0]);i++)
        {
            if(type == Check_List_[i].type && (str[3] & 0x3f) == Check_List_[i].vaule && (Check_List_[i].payload_len == argc - 7 || (Check_List_[i].payload_len == 0xff && (argc - 7 >=0x1))))
            {
                if(k == 0)
                {
                    printf("\n\t  0x%02x  %s",str[3] & 0x3f,Check_List_[i].gate);
                }
                else
                {
                    printf("\n\t        *%s",Check_List_[i].gate);
                }
                flag[k] = i;
                k++;
                for(j=0;j<sizeof(Gate_Ref) / sizeof(Gate_Ref[0]);j++)
                {
                    if(Check_List_[i].gType == Gate_Ref[j].gType)
                    {
                        printf("\n\t        %s",Gate_Ref[j].gate);
                        //break;
                    }
                }
                //break;
            }
        }
        if(flag[0] == 0xffff)
        {
            printf("\n\t  0x%02x  Unknow optional",str[3] & 0x3f);
        }

        if(str[0] > 0x5)
        {
            for(i=4;i<argc - 3;i++)
            {
                //To do
                if(i == 4)
                {
                    printf("\n\t--Data  0x%02x",str[i]);
                }
                else
                {
                    printf("%02x",str[i]);
                }
            }
            for(i=0;i<sizeof(flag)/sizeof(flag[0]);i++)
            {
                if(flag[i] != 0xffff)
                {
                    if(i == 0)
                    {printf("\n\t        %s",Check_List_[flag[i]].parameter);}
                    else
                    {printf("\n\t        *%s",Check_List_[flag[i]].parameter);}
                    //gid,hid check
                    if(Check_List_[flag[i]].vaule == 0x10 && Check_List_[flag[i]].payload_len == 3)
                    {
                        uint8_t h = 0;
                        uint8_t p = 0;
                        for(h=0;h<sizeof(Gate_Table)/sizeof(Gate_Table[0]);h++)
                        {
                            if(str[4] == Gate_Table[h].Gid)
                            {
                                printf("\n\t  0x%02x  %s",str[4],Gate_Table[h].gate);
                                p = 0x1;
                                break;
                            }
                        }
                        if(p != 0x1)
                        {
                            printf("\n\t  0x%02x  RFU",str[4]);
                        }
                        for(h=0;h<sizeof(_Host_Table_)/sizeof(_Host_Table_[0]);h++)
                        {
                            if(str[5] == _Host_Table_[h].hid)
                            {
                                printf("\n\t  0x%02x  %s",str[5],_Host_Table_[h].host);
                                p = 0x2;
                                break;
                            }
                        }
                        if(p != 0x2)
                        {
                            printf("\n\t  0x%02x  RFU",str[5]);
                        }
                        for(h=0;h<sizeof(Gate_Table)/sizeof(Gate_Table[0]);h++)
                        {
                            if(str[6] == Gate_Table[h].Gid)
                            {
                                printf("\n\t  0x%02x  %s",str[6],Gate_Table[h].gate);
                                p = 0x3;
                                break;
                            }
                        }
                        if(p != 0x3)
                        {
                            printf("\n\t  0x%02x  RFU",str[6]);
                        }
                    }
                    //registry check
                    if(Check_List_[flag[i]].registry_ != 0x0)
                    {
                        //To do 
                        uint8_t g = 0;
                        for(j=0;j<sizeof(Registry_Table)/sizeof(Registry_Table[0]);j++)
                        {
                            if((str[4] == Registry_Table[j].iden_) && (Check_List_[flag[i]].vaule == Registry_Table[j]._r_w_ || Registry_Table[j]._r_w_ == _r_w_) && (Registry_Table[j].len_ == argc - 8 || Registry_Table[j].len_ == 0xff))
                            {
                                if(g == 0)
                                {
                                    g = 0x1;
                                    printf("\n\t  0x%02x  %s(Default:0x%02x)",Registry_Table[j].iden_,Registry_Table[j].gate,Registry_Table[j].default_);
                                    printf("\n\t        %s",Registry_Table[j]._comment_);
                                }
                                else
                                {
                                    printf("\n\t        *%s(Default:0x%02x)",Registry_Table[j].gate,Registry_Table[j].default_);
                                    printf("\n\t        *%s",Registry_Table[j]._comment_);
                                }
                            }
                        }
                    }
                }
            }
            if(((str[3] & 0x3f) == 0x3e && argc - 7 == 3) || ((str[3] & 0x3f) == 0x3f && argc - 7 == 4))
            {
                ((uint8_t*)&address)[1] = str[5];
                ((uint8_t*)&address)[0] = str[6];
                uint8_t check = 0x0;
                for(i=0;i<sizeof(Register_Table)/sizeof(Register_Table[0]);i++)
                {
                    if(address == Register_Table[i].address)
                    {
                        printf("\n\t  %02x  %s(Default:0x%02x)",Register_Table[i].address,Register_Table[i].name,Register_Table[i].default_);
                        if((str[3] & 0x3f) == 0x3f)
                        {printf("\n\t    %02x  Configuration vaule",str[7]);}
                        printf("\n\t        %s",Register_Table[i].comment);
                        check = 0xff;
                        break;
                    }
                }
                if(check == 0x0)
                {
                    printf("\n\t  %02x  ##(Register hide)",address);
                    if((str[3] & 0x3f) == 0x3f)
                    {printf("\n\t    %02x  Configuration vaule",str[7]);}
                }
            }
        }
    }
    
    if(str[1] == 0xf9)
    {
        //windows size
        SetColor(2);
        if(str[2] >= 0x02 && str[2] <= 0x04)
        {printf("\n\t--0x%02x  Window size",str[2]);}
        else
        {printf("\n");error(ERROR_WIN_);return 0;}

        printf("\n\t--0x%02x  Endpoint capabilities",str[3]);
        if((str[3] & 0x01) == 0x01)
        {printf("\n\t  0x%02x  Support of Selective Reject S-frame (SREJ)",str[3] & 0x01);}
        else
        {printf("\n\t  0x%02x  Not supported of Selective Reject S-frame (SREJ)",str[3] & 0x01);}

        if(str[0] == 0x6)
        {
            printf("\n\t--0x%02x  HSU baudrate",str[4]);
            if(str[4] == 0x00)
            {printf("\n\t  0x%02x  9 600 baud",str[4]);}
            else if(str[4] == 0x01 || str[4] == 0x03 || str[4] == 0x04 || str[4] == 0x08)
            {printf("\n\t  0x%02x  RFU",str[4]);}
            else if(str[4] == 0x02)
            {printf("\n\t  0x%02x  28 800 baud",str[4]);}
            else if(str[4] == 0x05)
            {printf("\n\t  0x%02x  115 200 baud",str[4]);}
            else if(str[4] == 0x06)
            {printf("\n\t  0x%02x  230 400 baud",str[4]);}
            else if(str[4] == 0x07)
            {printf("\n\t  0x%02x  460 800 baud",str[4]);}
            else
            {printf("\n");error(ERROR_BAU_);return 0;}
        }

    }
    SetColor(8);
    printf("\n\t--0x%02x  CRC code 1",str[argc - 3]);
    printf("\n\t--0x%02x  CRC code 2",str[argc - 2]);

	SetColor(7);
    for(i=1;i<sizeof(flag)/sizeof(flag[0]);i++)
    {
        if(flag[i] != 0xffff)
        {
            printf("\n\t* Indicate the options,Please refer previous command to analyze.");
            break;
        }
    }
	return 0;
}


