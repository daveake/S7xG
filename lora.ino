#include <SPI.h>


byte currentMode = 0x81;
unsigned long UpdateClientAt=0;

#define REG_FIFO                    0x00
#define REG_FIFO_ADDR_PTR           0x0D 
#define REG_FIFO_TX_BASE_AD         0x0E
#define REG_FIFO_RX_BASE_AD         0x0F
#define REG_RX_NB_BYTES             0x13
#define REG_OPMODE                  0x01
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_PACKET_SNR              0x19
#define REG_PACKET_RSSI             0x1A
#define REG_RSSI_CURRENT            0x1B
#define REG_DIO_MAPPING_1           0x40
#define REG_DIO_MAPPING_2           0x41
#define REG_MODEM_CONFIG            0x1D
#define REG_MODEM_CONFIG2           0x1E
#define REG_MODEM_CONFIG3           0x26
#define REG_PAYLOAD_LENGTH          0x22
#define REG_IRQ_FLAGS_MASK          0x11
#define REG_HOP_PERIOD              0x24
#define REG_FREQ_ERROR              0x28
#define REG_DETECT_OPT              0x31
#define REG_DETECTION_THRESHOLD     0x37

// MODES
// MODES
#define RF98_MODE_RX_CONTINUOUS     0x85
#define RF98_MODE_TX                0x83
#define RF98_MODE_SLEEP             0x80
#define RF98_MODE_STANDBY           0x81

#define PAYLOAD_LENGTH              80

// Modem Config 1
#define EXPLICIT_MODE               0x00
#define IMPLICIT_MODE               0x01

#define ERROR_CODING_4_5            0x02
#define ERROR_CODING_4_6            0x04
#define ERROR_CODING_4_7            0x06
#define ERROR_CODING_4_8            0x08

#define BANDWIDTH_7K8               0x00
#define BANDWIDTH_10K4              0x10
#define BANDWIDTH_15K6              0x20
#define BANDWIDTH_20K8              0x30
#define BANDWIDTH_31K25             0x40
#define BANDWIDTH_41K7              0x50
#define BANDWIDTH_62K5              0x60
#define BANDWIDTH_125K              0x70
#define BANDWIDTH_250K              0x80
#define BANDWIDTH_500K              0x90

// Modem Config 2

#define SPREADING_6                 0x60
#define SPREADING_7                 0x70
#define SPREADING_8                 0x80
#define SPREADING_9                 0x90
#define SPREADING_10                0xA0
#define SPREADING_11                0xB0
#define SPREADING_12                0xC0

#define CRC_OFF                     0x00
#define CRC_ON                      0x04


// POWER AMPLIFIER CONFIG
#define REG_PA_CONFIG               0x09
#define PA_MAX_BOOST                0x8F
#define PA_LOW_BOOST                0x81
#define PA_MED_BOOST                0x8A
#define PA_MAX_UK                   0x88
#define PA_OFF_BOOST                0x00
#define RFO_MIN                     0x00

// LOW NOISE AMPLIFIER
#define REG_LNA                     0x0C
#define LNA_MAX_GAIN                0x23  // 0010 0011
#define LNA_OFF_GAIN                0x00


void SetParametersFromLoRaMode(int LoRaMode)
{
  Settings.LowDataRateOptimize = 0;
  
  if (LoRaMode == 7)
  {
    Settings.ImplicitOrExplicit = EXPLICIT_MODE;
    Settings.ErrorCoding = ERROR_CODING_4_5;
    Settings.Bandwidth = BANDWIDTH_20K8;
    Settings.SpreadingFactor = SPREADING_7;
  }
  else if (LoRaMode == 6)
  {
    Settings.ImplicitOrExplicit = IMPLICIT_MODE;
    Settings.ErrorCoding = ERROR_CODING_4_5;
    Settings.Bandwidth = BANDWIDTH_41K7;
    Settings.SpreadingFactor = SPREADING_6;
  }
  else if (LoRaMode == 5)
  {
    Settings.ImplicitOrExplicit = EXPLICIT_MODE;
    Settings.ErrorCoding = ERROR_CODING_4_8;
    Settings.Bandwidth = BANDWIDTH_41K7;
    Settings.SpreadingFactor = SPREADING_11;
  }
  else if (LoRaMode == 4)
  {
    Settings.ImplicitOrExplicit = IMPLICIT_MODE;
    Settings.ErrorCoding = ERROR_CODING_4_5;
    Settings.Bandwidth = BANDWIDTH_250K;
    Settings.SpreadingFactor = SPREADING_6;
  }
  else if (LoRaMode == 3)
  {
    Settings.ImplicitOrExplicit = EXPLICIT_MODE;
    Settings.ErrorCoding = ERROR_CODING_4_6;
    Settings.Bandwidth = BANDWIDTH_250K;
    Settings.SpreadingFactor = SPREADING_7;
  }
  else if (LoRaMode == 2)
  {
    Settings.ImplicitOrExplicit = EXPLICIT_MODE;
    Settings.ErrorCoding = ERROR_CODING_4_8;
    Settings.Bandwidth = BANDWIDTH_62K5;
    Settings.SpreadingFactor = SPREADING_8;
  }
  else if (LoRaMode == 1)
  {
    Settings.ImplicitOrExplicit = IMPLICIT_MODE;
    Settings.ErrorCoding = ERROR_CODING_4_5;
    Settings.Bandwidth = BANDWIDTH_20K8;
    Settings.SpreadingFactor = SPREADING_6;
  }
  else if (LoRaMode == 0)
  {
    Settings.ImplicitOrExplicit = EXPLICIT_MODE;
    Settings.ErrorCoding = ERROR_CODING_4_8;
    Settings.Bandwidth = BANDWIDTH_20K8;
    Settings.SpreadingFactor = SPREADING_11;
    Settings.LowDataRateOptimize = 0x08;
  }
}

// initialize the library with the numbers of the interface pins

void SetupLoRa()
{
  Settings.Frequency = 434.275;

  SetParametersFromLoRaMode(1);

  setupRFM98();
}

void UpdateClient(void)
{
  if (millis() >= UpdateClientAt)
  {
    int CurrentRSSI;

    if (Settings.Frequency > 525)
    {
      CurrentRSSI = readRegister(REG_RSSI_CURRENT) - 157;
    }
    else
    {
      CurrentRSSI = readRegister(REG_RSSI_CURRENT) - 164;
    }
    
    HostPort.print("CurrentRSSI=");
    HostPort.println(CurrentRSSI);
    
    UpdateClientAt = millis() + 1000;
  }
}

double FrequencyReference(void)
{
  switch (Settings.Bandwidth)
  {
    case  BANDWIDTH_7K8:  return 7800;
    case  BANDWIDTH_10K4:   return 10400; 
    case  BANDWIDTH_15K6:   return 15600; 
    case  BANDWIDTH_20K8:   return 20800; 
    case  BANDWIDTH_31K25:  return 31250; 
    case  BANDWIDTH_41K7:   return 41700; 
    case  BANDWIDTH_62K5:   return 62500; 
    case  BANDWIDTH_125K:   return 125000; 
    case  BANDWIDTH_250K:   return 250000; 
    case  BANDWIDTH_500K:   return 500000; 
  }
}


double FrequencyError(void)
{
  int32_t Temp;
  double T;
  
  Temp = (int32_t)readRegister(REG_FREQ_ERROR) & 7;
  Temp <<= 8L;
  Temp += (int32_t)readRegister(REG_FREQ_ERROR+1);
  Temp <<= 8L;
  Temp += (int32_t)readRegister(REG_FREQ_ERROR+2);
  
  if (readRegister(REG_FREQ_ERROR) & 8)
  {
    Temp = Temp - 524288;
  }

  T = (double)Temp;
  T *=  (16777216.0 / 32000000.0);
  T *= (FrequencyReference() / 500000.0);

  return -T;
} 

int receiveMessage(unsigned char *message)
{
  int i, Bytes, currentAddr;

  int x = readRegister(REG_IRQ_FLAGS);
  // printf("Message status = %02Xh\n", x);
  
  // clear the rxDone flag
  // writeRegister(REG_IRQ_FLAGS, 0x40); 
  writeRegister(REG_IRQ_FLAGS, 0xFF); 
   
  // check for payload crc issues (0x20 is the bit we are looking for
  if((x & 0x20) == 0x20)
  {
    HostPort.println("CRC Failure");
    // reset the crc flags
    writeRegister(REG_IRQ_FLAGS, 0x20); 
    Bytes = 0;
  }
  else
  {
    currentAddr = readRegister(REG_FIFO_RX_CURRENT_ADDR);
    Bytes = readRegister(REG_RX_NB_BYTES);
    // printf ("%d bytes in packet\n", Bytes);

    // printf("RSSI = %d\n", readRegister(REG_RSSI) - 137);
  
    writeRegister(REG_FIFO_ADDR_PTR, currentAddr);   
    // now loop over the fifo getting the data
    for(i = 0; i < Bytes; i++)
    {
      message[i] = (unsigned char)readRegister(REG_FIFO);
    }
    message[Bytes] = '\0';

    // writeRegister(REG_FIFO_ADDR_PTR, 0);  // currentAddr);   
  } 
  
  return Bytes;
}


void SetFrequency(char *Line)
{
  float Frequency;

  Frequency = atof(Line);

  if (Frequency > 0)
  {
    ReplyOK();

    Settings.Frequency = Frequency;
    
    HostPort.printf("Frequency=%.3f\r\n", Frequency);

    startReceiving();
  }
  else
  {
    ReplyBad();
  }
}


void SetLoRaMode(char *Line)
{
  int Mode;

  Mode = atoi(Line);

  if ((Mode >= 0) && (Mode <= 7))
  {
    ReplyOK();

    SetParametersFromLoRaMode(Mode);
    
    HostPort.print("Mode=");
    HostPort.println(Mode);

    startReceiving();
  }
  else
  {
    ReplyBad();
  }
}

void SetBandwidth(char *Line)
{
  if (strcmp(Line, "7K8") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_7K8;
    startReceiving();
  }
  else if (strcmp(Line, "10K4") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_10K4;
    startReceiving();
  }
  else if (strcmp(Line, "15K6") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_15K6;
    startReceiving();
  }
  else if (strcmp(Line, "20K8") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_20K8;
    startReceiving();
  }
  else if (strcmp(Line, "31K25") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_31K25;
    startReceiving();
  }
  else if (strcmp(Line, "41K7") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_41K7;
    startReceiving();
  }
  else if (strcmp(Line, "62K5") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_62K5;
    startReceiving();
  }
  else if (strcmp(Line, "125K") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_125K;
    startReceiving();
  }
  else if (strcmp(Line, "250K") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_250K;
    startReceiving();
  }
  else if (strcmp(Line, "500K") == 0)
  {
    ReplyOK();
    Settings.Bandwidth = BANDWIDTH_500K;
    startReceiving();
  }
  else
  {
    ReplyBad();
  }
}

void SetErrorCoding(char *Line)
{
  int Coding;

  Coding = atoi(Line);

  if ((Coding >= 5) && (Coding <= 8))
  {
    ReplyOK();
    Settings.ErrorCoding = (Coding-4) << 1;
    startReceiving();
  }
  else
  {
    ReplyBad();
  }
}

void SetSpreadingFactor(char *Line)
{
  int Spread;

  Spread = atoi(Line);

  if ((Spread >= 6) && (Spread <= 12))
  {
    ReplyOK();
    Settings.SpreadingFactor = Spread << 4;
    startReceiving();
  }
  else
  {
    ReplyBad();
  }
}

void SetImplicit(char *Line)
{
  int Implicit;

  Implicit = atoi(Line);

  ReplyOK();
  
  Settings.ImplicitOrExplicit = Implicit ? IMPLICIT_MODE : EXPLICIT_MODE;
  startReceiving();
}

void SetLowOpt(char *Line)
{
  int LowOpt;

  LowOpt = atoi(Line);

  ReplyOK();
  
  Settings.ImplicitOrExplicit = LowOpt ? 0x08 : 0;
  
  startReceiving();
}

void CheckLoRa()
{
  CheckRx();
  
  UpdateClient();
}

void CheckRx()
{
  if (digitalRead(LORA_DIO0))
  {
    unsigned char Message[256];
    int Bytes, SNR, RSSI, i;
    long Altitude;

    Bytes = receiveMessage(Message);
    
    HostPort.print("FreqErr="); HostPort.println(FrequencyError()/1000.0);

    SNR = readRegister(REG_PACKET_SNR);
    SNR /= 4;
    
    if (Settings.Frequency > 525)
    {
      RSSI = readRegister(REG_PACKET_RSSI) - 157;
    }
    else
    {
      RSSI = readRegister(REG_PACKET_RSSI) - 164;
    }
    
    if (SNR < 0)
    {
      RSSI += SNR;
    }
    
    HostPort.print("PacketRSSI="); HostPort.println(RSSI);
    HostPort.print("PacketSNR="); HostPort.println(SNR);
    

    HostPort.print("Packet size = "); HostPort.println(Bytes);
    HostPort.println((char *)Message);

    // Telemetry='$$LORA1,108,20:30:39,51.95027,-2.54445,00141,0,0,11*9B74

    if (Bytes == 0)
    {
      // NOTHING TO DO
    }
    else if (Message[0] == '$')
    {
      HostPort.print("Message=");
      HostPort.println((char *)Message);
    }
    else if (Message[0] == '%')
    {
      char *ptr, *ptr2;

      Message[0] = '$';
      
      ptr = (char *)Message;
      do
      {
        if ((ptr2 = strchr(ptr, '\n')) != NULL)
        {
          *ptr2 = '\0';
          HostPort.print("Message=");
          HostPort.println(ptr);
          ptr = ptr2 + 1;
        }
      } while (ptr2 != NULL);
    }
    else
    {
      HostPort.print("Hex=");
      for (i=0; i<Bytes; i++)
      {
        if (Message[i] < 0x10)
        {
          HostPort.print("0");
        } 
        HostPort.print(Message[i], HEX);
      }
      HostPort.println();
    }
  }
}


/////////////////////////////////////
//    Method:   Change the mode
//////////////////////////////////////
void setMode(byte newMode)
{
  if(newMode == currentMode)
    return;  
  
  switch (newMode) 
  {
    case RF98_MODE_TX:
      writeRegister(REG_LNA, LNA_OFF_GAIN);  // TURN LNA OFF FOR TRANSMITT
      writeRegister(REG_PA_CONFIG, PA_MAX_UK);
      writeRegister(REG_OPMODE, newMode);
      currentMode = newMode; 
      break;
      
      case RF98_MODE_RX_CONTINUOUS:
      writeRegister(REG_PA_CONFIG, PA_OFF_BOOST);  // TURN PA OFF FOR RECIEVE??
      writeRegister(REG_LNA, LNA_MAX_GAIN);  // LNA_MAX_GAIN);  // MAX GAIN FOR RECIEVE
      writeRegister(REG_OPMODE, newMode);
      currentMode = newMode; 
      break;
      
    case RF98_MODE_SLEEP:
      writeRegister(REG_OPMODE, newMode);
      currentMode = newMode; 
      break;
      
    case RF98_MODE_STANDBY:
      writeRegister(REG_OPMODE, newMode);
      currentMode = newMode; 
      break;
    default: return;
  } 
  
  if(newMode != RF98_MODE_SLEEP)
  {
    delay(10);
  }
   
  return;
}


/////////////////////////////////////
//    Method:   Read Register
//////////////////////////////////////

byte readRegister(byte addr)
{
  select();
  SPI.transfer(addr & 0x7F);
  byte regval = SPI.transfer(0);
  unselect();
  return regval;
}

/////////////////////////////////////
//    Method:   Write Register
//////////////////////////////////////

void writeRegister(byte addr, byte value)
{
  select();
  SPI.transfer(addr | 0x80); // OR address with 10000000 to indicate write enable;
  SPI.transfer(value);
  unselect();
}

/////////////////////////////////////
//    Method:   Select Transceiver
//////////////////////////////////////
void select() 
{
  digitalWrite(LORA_NSS, LOW);
}

/////////////////////////////////////
//    Method:   UNSelect Transceiver
//////////////////////////////////////
void unselect() 
{
  digitalWrite(LORA_NSS, HIGH);
}

void SetLoRaFrequency()
{
  unsigned long FrequencyValue;
  double Temp;
  
  Temp = Settings.Frequency * 7110656 / 434;
  FrequencyValue = (unsigned long)(Temp);

  HostPort.print("FrequencyValue is ");
  HostPort.println(FrequencyValue);

  writeRegister(0x06, (FrequencyValue >> 16) & 0xFF);    // Set frequency
  writeRegister(0x07, (FrequencyValue >> 8) & 0xFF);
  writeRegister(0x08, FrequencyValue & 0xFF);
}

void SetLoRaParameters()
{
  writeRegister(REG_MODEM_CONFIG, Settings.ImplicitOrExplicit | Settings.ErrorCoding | Settings.Bandwidth);
  writeRegister(REG_MODEM_CONFIG2, Settings.SpreadingFactor | CRC_ON);
  writeRegister(REG_MODEM_CONFIG3, 0x04 | Settings.LowDataRateOptimize);                  // 0x04: AGC sets LNA gain
  writeRegister(REG_DETECT_OPT, (readRegister(REG_DETECT_OPT) & 0xF8) | ((Settings.SpreadingFactor == SPREADING_6) ? 0x05 : 0x03));  // 0x05 For SF6; 0x03 otherwise
  writeRegister(REG_DETECTION_THRESHOLD, (Settings.SpreadingFactor == SPREADING_6) ? 0x0C : 0x0A);    // 0x0C for SF6, 0x0A otherwise

}

void startReceiving()
{
  setMode(RF98_MODE_SLEEP);
  writeRegister(REG_OPMODE,0x80);  
  setMode(RF98_MODE_SLEEP);

  SetLoRaFrequency();
  
  SetLoRaParameters();
  
  writeRegister(REG_PAYLOAD_LENGTH, 255);
  writeRegister(REG_RX_NB_BYTES, 255);
  
  writeRegister(REG_FIFO_RX_BASE_AD, 0);
  writeRegister(REG_FIFO_ADDR_PTR, 0);
  
  // Setup Receive Continous Mode
  setMode(RF98_MODE_RX_CONTINUOUS);
}

void setupRFM98(void)
{
  // initialize the pins
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, HIGH);
  delay(10);          // Module needs this before it's ready  
  
  pinMode(LORA_NSS, OUTPUT);
  pinMode(LORA_DIO0, INPUT);

  SPI.setMISO(LORA_MISO);
  SPI.setMOSI(LORA_MOSI);
  SPI.setSCLK(LORA_SCK);

  SPI.begin();
  
  startReceiving();
  
  HostPort.println("LoRa Setup Complete");
}
