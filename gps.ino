/* ========================================================================== */
/*   gps.ino                                                                  */
/*                                                                            */
/*   Serial ode for Sony GPS on S7xG                                          */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* ========================================================================== */

char Hex(char Character)
{
  char HexTable[] = "0123456789ABCDEF";
  
  return HexTable[Character];
}

int GPSChecksumOK(char *Buffer, int Count)
{
  unsigned char XOR, i, c;

  XOR = 0;
  for (i = 1; i < (Count-4); i++)
  {
    c = Buffer[i];
    XOR ^= c;
  }

  return (Buffer[Count-4] == '*') && (Buffer[Count-3] == Hex(XOR >> 4)) && (Buffer[Count-2] == Hex(XOR & 15));
}

float FixPosition(float Position)
{
  float Minutes, Seconds;
  
  Position = Position / 100;
  
  Minutes = trunc(Position);
  Seconds = fmod(Position, 1);

  return Minutes + Seconds * 5 / 3;
}

void ProcessLine(char *Buffer, int Count)
{
  int Satellites, date;
  char ns, ew;
  char TimeString[16], LatString[16], LongString[16], Temp[4];
  
  if (GPSChecksumOK(Buffer, Count))
  {
    Satellites = 0;
  
    if (strncmp(Buffer+3, "GGA", 3) == 0)
    {
      int lock;
      char hdop[16], Altitude[16];
      
      if (sscanf(Buffer+7, "%16[^,],%16[^,],%c,%[^,],%c,%d,%d,%[^,],%[^,]", TimeString, LatString, &ns, LongString, &ew, &lock, &Satellites, hdop, Altitude) >= 1)
      { 
        // $GPGGA,124943.00,5157.01557,N,00232.66381,W,1,09,1.01,149.3,M,48.6,M,,*42
        Temp[0] = TimeString[0]; Temp[1] = TimeString[1]; Temp[2] = '\0';
        GPS.Hours = atoi(Temp);
        Temp[0] = TimeString[2]; Temp[1] = TimeString[3]; Temp[2] = '\0';
        GPS.Minutes = atoi(Temp);
        Temp[0] = TimeString[4]; Temp[1] = TimeString[5]; Temp[2] = '\0';
        GPS.Seconds = atoi(Temp);
        GPS.SecondsInDay = (unsigned long)GPS.Hours * 3600L + (unsigned long)GPS.Minutes * 60L + (unsigned long)GPS.Seconds;

        if (Satellites >= 4)
        {
          GPS.Latitude = FixPosition(atof(LatString));
          if (ns == 'S') GPS.Latitude = -GPS.Latitude;
          GPS.Longitude = FixPosition(atof(LongString));
          if (ew == 'W') GPS.Longitude = -GPS.Longitude;
          GPS.Altitude = (unsigned int)atof(Altitude);
        }
        
        GPS.Lock = lock;
        GPS.Satellites = Satellites;
        GPS.GotTime = 1;
      }
      else
      {
        GPS.GotTime = 0;
      }
      
      if (ShowGPS)
      {
        HostPort.printf("GPS=%02d:%02d:%02d,%.5f,%.5f,%05ld\r\n", GPS.Hours, GPS.Minutes, GPS.Seconds,
                                                                  GPS.Latitude, GPS.Longitude, GPS.Altitude); 
      }
    }
  }
  else
  {
    HostPort.println("Bad checksum");
  }
}


void SetupGPS(void)
{
  GPSPort.begin(115200);

  // drive GNSS RST pin low
  pinMode(GNSS_RST, OUTPUT);
  digitalWrite(GNSS_RST, LOW);

  // activate 1.8V<->3.3V level shifters
  pinMode(GNSS_LS,  OUTPUT);
  digitalWrite(GNSS_LS,  HIGH);

  // keep RST low to ensure proper IC reset
  delay(200);

  // release
  digitalWrite(GNSS_RST, HIGH);

  // give Sony GNSS few ms to warm up
  delay(100);

  // Leave pin floating
  pinMode(GNSS_RST, INPUT);

  // GGA + GSA + RMC
  GPSPort.write("@BSSL 0x25\r\n");
  delay(250);
  
  // GPS + GLONASS
  GPSPort.write("@GNS 0x3\r\n");
  delay(250);  
}

void CheckGPS(void)
{
  static char Line[100];
  static int Length=0;
  char Character;

  while(GPSPort.available())
  { 
    Character = GPSPort.read();    

    if (Character == '$')
    {
      Line[0] = Character;
      Length = 1;
    }
    else if (Length > 90)
    {
      Length = 0;
    }
    else if ((Length > 0) && (Character != '\r'))
    {
      Line[Length++] = Character;
      if (Character == '\n')
      {
        Line[Length] = '\0';
        ProcessLine(Line, Length);
        Length = 0;
      }
    }
  }
}
