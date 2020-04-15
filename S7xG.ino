
/*------------------------------------------------------------------------------------------------------*\
|                                                                                                        |
| S7xG Firmware for TTGO Watch                                                                           |
|                                                                                                        |
| IMPORANT - USE FOLLOWING ARDUINO SETTINGS                                                              |
|                                                                                                        |
| BOARD             - Nucleo-64                                                                          |
| BOARD PART NUMBER - Nucleo L073RZ                                                                      |
| U(S)ART SUPPORT   - Enabled (no generic serial)                                                        |
| USB SUPPORT       - None                                                                               |
| C RUNTIME LIBRARY - Newlib Nano + Float Point                                                          |
| UPLOAD METHOD     - STM32CubeProgrammer(SWD)                                                           |
|                                                                                                        |
\*------------------------------------------------------------------------------------------------------*/

#define   VERSION   "V1.00"

//------------------------------------------------------------------------------------------------------

#define GNSS_RST     PB2
#define GNSS_LS      PC6

#define LORA_RST     PB10
#define LORA_DIO0    PB11
#define LORA_MOSI    PB15
#define LORA_MISO    PB14
#define LORA_SCK     PB13
#define LORA_NSS     PB12

//------------------------------------------------------------------------------------------------------

#define COMMAND_BUFFER_LENGTH  70

//------------------------------------------------------------------------------------------------------

struct TSettings
{
  // LoRa
  float         Frequency;
  unsigned char ImplicitOrExplicit;             // 1=Implicit, 0=Explicit
  unsigned char ErrorCoding;
  unsigned char Bandwidth;
  unsigned char SpreadingFactor;
  unsigned char LowDataRateOptimize;

} Settings;

struct TGPS
{
  int           Hours, Minutes, Seconds;
  unsigned long SecondsInDay;					// Time in seconds since midnight
  float         Longitude, Latitude;
  long          Altitude;
  unsigned int  Satellites;
  byte          FixType;
  byte          psm_status;
  byte          Lock;
  byte          GotTime;
} GPS;


int ShowGPS=1;
int ShowLoRa=1;
unsigned char SSDVBuffer[256];
unsigned int SSDVBufferLength=0;

HardwareSerial Serial4(PC11, PC10);   // GPS
HardwareSerial Serial2(PA10, PA9);    // Host

#define GPSPort Serial4
#define HostPort Serial2

//------------------------------------------------------------------------------------------------------

void setup()
{
  // Serial ports
  
  HostPort.begin(115200);

  delay(100);
  HostPort.print("\r\n\nTTGO S7xG HAB Firmware ");
  HostPort.println(VERSION);
      
  SetupGPS();
  
  SetupLoRa();
}

void loop()
{  
  CheckHost();
  
  CheckGPS();

  CheckLoRa();
}

void ReplyOK(void)
{
  HostPort.println('*');
}

void ReplyBad(void)
{
  HostPort.println('?');
}


void ShowVersion(void)
{
  ReplyOK();
    
  HostPort.print("VER=");
  HostPort.println(VERSION);
}

void CheckHost(void)
{
  static char Line[COMMAND_BUFFER_LENGTH];
  static unsigned int Length=0;
  char Character;

  while (HostPort.available())
  { 
    Character = HostPort.read();

    if (Character == '~')
    {
      Line[0] = Character;
      Length = 1;
    }
    else if (Character == '\r')
    {
      Line[Length] = '\0';
      ProcessCommand(Line+1);
      Length = 0;
    }
    else if (Length >= sizeof(Line))
    {
      Length = 0;
    }
    else if (Length > 0)
    {
      Line[Length++] = Character;
    }
  }
}

void ProcessCommand(char *Line)
{
  char Command;

  Command = Line[0];
  Line++;
       
  if (Command == 'F')
  {
    SetFrequency(Line);
  }
  else if (Command == 'M')
  {
    SetLoRaMode(Line);
  }
  else if (Command == 'B')
  {
    SetBandwidth(Line);
  }
  else if (Command == 'E')
  {
    SetErrorCoding(Line);
  }
  else if (Command == 'S')
  {
    SetSpreadingFactor(Line);
  }
  else if (Command == 'I')
  {
    SetImplicit(Line);
  }
  else if (Command == 'L')
  {
    SetLowOpt(Line);
  }
  else if (Command == 'V')
  {
    ShowVersion();
  }
  else if (Command == 'G')
  {
    SetShowGPS(Line);
  }
  else
  {
    ReplyBad();
  }
}

void SetShowGPS(char *Line)
{
  int Coding;

  ShowGPS = atoi(Line);

  ReplyOK();
}
