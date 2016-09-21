/*
 * ESP8266 Manager 
 * 
 * Copyright (c) 2016, Serge Beland (sbeland@gmail.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of <Serge Beland> nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>


char *ServerHost = "sbelandiot.com"; // where your php/html files are hosted, everything must be under the root directory

#define DEBUG // comment it out to remove debugging display on serial monitor 
              // if you use, then esp will wait that you open the serial monitor window before starting

#ifdef DEBUG
#define DebugInit() Serial.begin(115200)
#define Debugln(...) Serial.println(__VA_ARGS__)
#define Debug(v) Serial.print(v)
#define Debugf(...) Serial.printf(__VA_ARGS__)
#else
#define DebugInit()
#define Debug(...)
#define Debugln(...)
#define Debugf(...)
#endif

#define STATE_AP_MODE            0
#define STATE_WAIT_INPUT         1   
#define STATE_GET_INFO           2
#define STATE_SEND_MAIL_STARTED  3
#define STATE_WAIT_FOR_SENSOR    4
#define STATE_PENDING_MODE       5
#define STATE_UPDATE_INFO        6

#define SENDMAIL_OPTION_STARTED  0
#define SENDMAIL_OPTION_SENSOR1  1

#define SSID_AD            0
#define SSID_PASSWORD_AD   49
#define ESP_PASSWORD_AD    114
#define EMAIL_AD           131

char Ssid [49];
char SsidPassword [65];
char EspPassword [17];
char Email [65];

char EspName [17];
int NbOfSsid;
int state;                    // different state in loop
unsigned long StartTime = 0;  // keep the time passed in seconds since the module started
bool PendingMode = 0;         // use to say if this iot is activated on web server (so we can keep going) or not (so we must wait)
unsigned long TimeSet = 0;    // can be use to measure time elapse in millisecond between 2 points by using SetTime and TimeDiff
unsigned long UpdateTimeSet = 0; // is used by the update info interval
unsigned long UpdateInterval = 60000;
char *EspDefaultPW = "admin123";

ESP8266WebServer server (80);  // use for AP mode to setup the esp


/*
 * replace {v} with title
 * String page = Header;
 * page.replace ("{v}", "yourtitle");
 * 
 /*
 * to add in the header of html to refresh the browser page every x seconds
 * <meta http-equiv='refresh' content='5'/>  
 */
 // I myself inspired from this code: https://github.com/tzapu/WiFiManager
char *Header = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>"
"<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>"
"<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>"
"</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";

char *Footer = "</body></html>";


/* 
 *  This function list all available AP on serial monitor
 *  it return the number of networks found
 *  you can access each network from WiFi.SSID (index) where index < number of network
 */
int ListAP (void)
{   
   Debugln ("ListAP\nScanning...wait");
     
   // WiFi.scanNetworks will return the number of networks found
   NbOfSsid = WiFi.scanNetworks();
   
   if (NbOfSsid == 0)
   {
      Debugln ("\nNo networks found!!!");
      return 0;
   }

   Debugf ("%d networks found\n\n", NbOfSsid);
   
   for (int i = 0; i < NbOfSsid; ++i)
   {
      // Print SSID and RSSI for each network found
      Debug (i + 1);
      Debug (": ");
      Debug (WiFi.SSID(i));
      Debug (" (");
      Debug (WiFi.RSSI(i));
      Debug (")");
      Debugln ((WiFi.encryptionType (i) == ENC_TYPE_NONE) ? "   " : "*  ");
      //Debugln (WiFi.remoteIP ());
      delay(10);
   }
   Debugln ("\nScan done");

   return NbOfSsid;  
}


void GetEEpromInfo ()
{
   EEPROM.begin(512);

   EEPROM.get (SSID_AD, Ssid);
   EEPROM.get (SSID_PASSWORD_AD, SsidPassword);
   EEPROM.get (ESP_PASSWORD_AD, EspPassword);
   EEPROM.get (EMAIL_AD, Email);

   Ssid [sizeof (Ssid) - 1] = 0;
   SsidPassword [sizeof (SsidPassword) - 1] = 0;
   EspPassword [sizeof (EspPassword) - 1] = 0;
   Email [sizeof (Email) - 1] = 0;
   
   Debugln ("\nEEPROM INFO READED");
   Debugf ("SSID: %s\n", Ssid);
   Debugf ("SsidPassword: %s\n", SsidPassword);
   Debugf ("ESP Password: %s\n", EspPassword);
   Debugf ("Email: %s\n\n", Email);
}

/*
 * Write in eeprom 4 variables: ssid, password, email and esp password
 */
void SetEEpromInfo ()
{
   EEPROM.begin(512);
   
   EEPROM.put (SSID_AD, Ssid);
   EEPROM.put (SSID_PASSWORD_AD, SsidPassword);
   EEPROM.put (ESP_PASSWORD_AD, EspPassword);
   EEPROM.put (EMAIL_AD, Email);
   EEPROM.commit ();

   Debugln ("\nEEPROM INFO WRITTEN");
   Debugf ("SSID: %s\n", Ssid);
   Debugf ("SsidPassword: %s\n", SsidPassword);
   Debugf ("ESP Password: %s\n", EspPassword);
   Debugf ("Email: %s\n\n", Email);
}
   

/*
 * Start wifi and try to connect inside the TimeOutInSec
 * return 1 if success
 *        0 if no success
 */
int ConnectToWifi (char *s, char *p, int TimeOutInSec)
{
   WiFi.begin (s, p);

   int t = 0;  

   Debugf ("Trying to connect to wifi with:\nSsid: %s\nPassword: %s\n", s, p);   
   
   while (WiFi.status () != WL_CONNECTED && t++ < TimeOutInSec) 
   {
      delay (1000);
      Debug (".");
   }

   Debugln ();
   
   if (WiFi.status () != WL_CONNECTED)
      return 0;
   
   Debugf ("\nWiFi connected to: %s\n", s);  
   Debugf ("with password: %s\n", p);
   Debug ("IP address: ");
   Debugln (WiFi.localIP ());
     
   return 1;
}


int TryToConnectToWifi (int nboftry)            
{
   // try to connect nboftry times, delay (count x 2) sec between each try
   char count = 0;
   
   for (int i = 0; i < nboftry; i++)
   {
      if (ConnectToWifi (Ssid, SsidPassword, 30))
         return 1;
      if (++count >= nboftry)
      {        
         Debugln ("Giving up to try to connect to wifi, return to AP mode now");
         WiFi.disconnect ();
         return 0;
      }
      delay (count * 2000);         
   }
   return 0;
}


/*
 * return 1 if good answer
 *  0 if not good answer
 * -1 if not connected to wifi
 * -2 if server did not answer, timeout reached
 */
int GetRequestExt (char *ahost, char *request, char *data, int aport, char *answer, int TimeOutInSec, char *mem, int memsize)
{
   WiFiClient client;   
   
   Debugf ("\nGet Request\nConnecting to: %s\nRequest: %s\nPort: %d\nData: %s\n", ahost, request, aport, data);   

   if (mem)
      memset (mem, 0, memsize);
  
   if (!client.connect(ahost, aport)) 
   {
      Debugln ("Connection failed");
      return -1;
   } 

   client.print ("GET /");
   client.print (request);
   client.print (data);
   client.println (" HTTP/1.1");
   client.print ("Host: ");
   client.println (ahost);
   client.println ("Cache-Control: no-cache");   
   client.println ("Connection: Close");
   client.println ();
   client.println ();  

   unsigned long timeout = millis();
   int to = TimeOutInSec * 1000;
   String line;
   
   while (client.available () == 0) 
      if (millis() - timeout > to) 
      {
         Serial.println(">>> Client Timeout !");
         client.stop();
         return -2;
      }

   char gotanswer = 0;
   int nbofchar = 0;
   
   // Read all the lines of the reply from server and print them to Serial
   char *p;
   char ret302 = 0;
   //String redirect;
   
   while (client.available ())
   {
      line = client.readStringUntil ('\r');      
      Debug (line);
      if (strstr (line.c_str (), "Location: http:") || 
         (strstr (line.c_str (), "HTTP/") && strstr (line.c_str (), "30")))
      {
         Debugln ("\nRedirection detected!");
         ret302++;
      }
      
      if (answer && !gotanswer && (p = strstr (line.c_str (), answer)) != NULL)
      {
         if (mem)
            strncpy (mem, line.c_str(), memsize - 1);         
         Debugf ("\n\n*** GotAnswer: %s ***\n", p);
         Debugln (line);
         gotanswer = 1;
      }
   }

/*   if (ret302 >= 2 && !gotanswer)
   {
    code to manage redirection, for example: hotspot with login page require username + password
   }
*/
   Debugln ("\nClosing connection");   
   if (answer == NULL || gotanswer)
      return 1;   
   return 0;
}


/*
 * return 1 if good answer or answer is NULL and we got reply from server
 * if answer is NULL, mem will not be filled with reply from server
 *  0 if not good answer
 * -1 if not connected to wifi
 * -2 if server did not answer, timeout reached
 */
int PostRequestExt (char *ahost, char *request, char *data, int aport, char *answer, int TimeOutInSec, char *mem, int memsize)
{
   WiFiClient client;   
   
   Debugf ("\nPost Request\nConnecting to: %s\nRequest: %s\nPort: %d\n", ahost, request, aport);   
   Debugf ("Data: %s\n", data);

   if (mem)
      memset (mem, 0, memsize);

   if (!client.connect(ahost, aport)) 
   {
      Debugln ("Connection failed");
      return -1;
   } 

   client.print ("POST /");
   client.print (request);
   client.println (" HTTP/1.1");
   client.print ("Host: ");
   client.println (ahost);
   client.println ("Cache-Control: no-cache");
   client.println ("Content-Type: application/x-www-form-urlencoded");
   client.print ("Content-Length: ");
   client.println (strlen (data));
   client.println ("Connection: Close");
   client.println ();
   client.println ();
   client.println (data);
  
   unsigned long timeout = millis();
   int to = TimeOutInSec * 1000;
   String line;
   
   while (client.available () == 0) 
      if (millis() - timeout > to) 
      {
         Serial.println(">>> Client Timeout !");
         client.stop();
         return -2;
      }

   char gotanswer = 0;
   int nbofchar = 0;
   
   // Read all the lines of the reply from server, copy in mem and print them to Serial
   char *p;

   if (answer)
      Debugf ("\nWaiting for answer: %s\n\n", answer);
   
   while (client.available ())
   {
      line = client.readStringUntil ('\r');
      Debug (line);
      if (answer && !gotanswer && (p = strstr (line.c_str (), answer)) != NULL)
      {
         if (mem)
            strncpy (mem, line.c_str(), memsize - 1);         
         Debugf ("\nGotAnswer: %s\n", p);
         Debugln (line);
         gotanswer = 1;
      }
   }

   Debugln ("\r\nClosing connection");   
   if (answer == NULL || gotanswer)
      return 1;   
   return 0;
}


void handleRoot() 
{
   Debugln ("Root");
   // I myself inspired from this code: https://github.com/tzapu/WiFiManager
   char *buf2 = "</center><br><form action=\"/wifilist\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"wifinoscan\" method=\"get\"><button>Configure WiFi (No Scan)</button></form><br/><form action=\"/info\" method=\"get\"><button>Info</button></form><br/><form action=\"/reset\" method=\"post\"><button>Reset</button></form>";

   String page = Header;
   page.replace ("{v}", EspName);
   page += "<center>";
   page += EspName;
   page += buf2;
   page += Footer;
   server.send(200, "text/html", page);
}

// I myself inspired from this code: https://github.com/tzapu/WiFiManager
int getRSSIasQuality(int RSSI) 
{
   int quality = 0;
   
   if (RSSI <= -100)   
      quality = 0;
   else if (RSSI >= -50) 
      quality = 100;
   else 
      quality = 2 * (RSSI + 100);
   
   return quality;
}

// I myself inspired from this code: https://github.com/tzapu/WiFiManager
void handleWifiList ()
{
   Debugln ("Wifi selection");
   ListAP ();
  
   char *buf = "<div><a href='#p' onclick='c(this)'></a>&nbsp;<span class='q {i}'></span></div>";              
   char *itemvar = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";               
   char *submit = "<form method='get' action='wifisave'>"
                  "<input id='s' name='s' length=32 placeholder='SSID'><br/>"
                  "<input id='p' name='p' length=64 type='password' placeholder='password'><br/>"
                  "<input id='e' name='e' length=128 placeholder='youremailaddress@domain.com'><br/><br>"
                  "<button type='submit'>save</button></form><br/><div class=\"c\"><a href=\"/wifi\">Scan</a></body></html>";

   String page = Header;
   page.replace ("{v}", EspName);   
   page += "<center>";
   page += EspName;
   page += "</center>";
   page += buf;

   String item;
   int quality;
 
   for (int i = 0; i < NbOfSsid; i++)
   {
      quality = getRSSIasQuality (WiFi.RSSI(i));

    //  if (_minimumQuality == -1 || _minimumQuality < quality) 
      {
         item = itemvar;
         String rssiQ;
         rssiQ += quality;
         item.replace("{v}", WiFi.SSID(i));
         item.replace("{r}", rssiQ);
         if (WiFi.encryptionType(i) != ENC_TYPE_NONE) 
            item.replace("{i}", "l");
         else 
            item.replace("{i}", "");        
         page += item;
      }   
   }
   
   page += submit;
   page += Footer;
   server.send (200, "text/html", page);          
}


void handleWifiSave ()
{
   Debugln ("Wifi save");
 
   char s [33];
   char p [65];
   char e [129];

   strcpy (s, server.arg("s").c_str());
   strcpy (p, server.arg("p").c_str());
   strcpy (e, server.arg("e").c_str());
     
   Debugf ("Received from client\nSsid: %s\nSsidPassword: %s\nEmail: %s\n", s, p, e);     
   
   int ret = ConnectToWifi (s, p, 30);
   String page = Header;
   page.replace ("{v}", EspName);
   page += EspName;
   if (ret)
      page += "<br>Connection successfull!<br>Info saved!<br>";
   else 
   {
      page += "<br>Unable to connect with this info:<br>";
      Debugln ("\nUnable to connect to wifi");
   }
   
   page += "Ssid: ";
   page += s;
   page += "<br>";
   
   page += "SsidPassword: ";
   page += p;
   page += "<br>";

   page += "Email: ";
   page += e;
   page += "<br>";

   if (!ret)
   {
      page += "Go back and rescan<br>";
      page += Footer;           
      server.send(200, "text/html", page);   
      state = STATE_AP_MODE;
      return;   
   }
   
   page += "Check your email, you should receive an email soon!<br>";
   page += Footer;
   server.send(200, "text/html", page);   

   strcpy (Ssid, s);
   strcpy (SsidPassword, p);
   strcpy (Email, e);
   
   SetEEpromInfo ();
   state = STATE_GET_INFO;   
}


void handleReset ()
{
   Debugln ("Reset");
   
   String page = Header;
   page.replace ("{v}", EspName);
   page += EspName;
   page += " will reset in a few seconds";
   if (Ssid [0] && SsidPassword [0])
   {
      page += " and try to reconnect to wifi";
      state = STATE_SEND_MAIL_STARTED;
   }
   else 
   {
      page += " and restart in Access Point mode";
      state = STATE_AP_MODE;
   }
   page += Footer;
   server.send(200, "text/html", page);
         
   ESP.reset();
   delay(2000);   
}


void handleInfo ()
{
   Debugln ("Show Info");
   
   String page = Header;
   page.replace("{v}", "Info");
   
   page += "<dl>";
   
   page += "<dt>Chip ID</dt><dd>";
   page += ESP.getChipId();
   page += "</dd>";
   
   page += "<dt>Flash Chip ID</dt><dd>";
   page += ESP.getFlashChipId();
   page += "</dd>";
   
   page += "<dt>IDE Flash Size</dt><dd>";
   page += ESP.getFlashChipSize();
   page += " bytes</dd>";
   
   page += "<dt>Real Flash Size</dt><dd>";
   page += ESP.getFlashChipRealSize();
   page += " bytes</dd>";
   
   page += "<dt>Soft AP IP</dt><dd>";
   page += WiFi.softAPIP().toString();
   page += "</dd>";
   
   page += "<dt>Soft AP MAC</dt><dd>";
   page += WiFi.softAPmacAddress();
   page += "</dd>";

   page += "<dt>Station MAC</dt><dd>";
   page += WiFi.macAddress();
   page += "</dd>";

   page += "<dt>Memory Stored Info</dt><dd>";
   page += "Ssid: ";
   page += Ssid;
   page += "<br>";
   page += "SsidPassword: ";
   page += SsidPassword;
   page += "<br>";
   page += "Email: ";
   page += Email;
   page += "<br>";
   page += "</dd>";
   
   page += "</dl>";
   page += Footer;
   
   server.send(200, "text/html", page);
}


bool CheckPassword (char *p)
{
   for (int i = 0; p [i] != 0; i++)
      if (!((p [i] >= '0' && p [i] <= '9') ||
         (p [i] >= 'a' && p [i] <= 'z') ||
         (p [i] >= 'A' && p [i] <= 'Z')))
         return 0;
   return 1;
}
  
void StartInAPMode ()
{   
   if (EspPassword [0] == 0 || strlen (EspPassword) < 8 || CheckPassword (EspPassword) == 0)
   {
      strcpy (EspPassword, EspDefaultPW);  
      SetEEpromInfo ();
   }

   if (CheckPassword (SsidPassword) == 0)
   {
      strcpy (SsidPassword, "");  
      SetEEpromInfo ();
   }
   
   WiFi.softAP (EspName, EspPassword);     
   IPAddress myIP = WiFi.softAPIP();
   
   Debug ("\nStarting Access Point\nAP IP address: ");
   Debugln (myIP);
   server.on("/", handleRoot);
   server.on("/reset", handleReset);
   server.on("/info", handleInfo);
   server.on("/wifilist", handleWifiList);
   server.on("/wifisave", handleWifiSave);
   server.begin();
   Debugln ("HTTP server started");  
   Debugf ("Must connect at: %s\nPassword: %s\n", EspName, EspPassword);
}


char encode[] =
{
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};

long encode_base64 (const void *pdata, long len, char *code)
{
   unsigned char *s, *end;
   unsigned char *data = (unsigned char *) pdata;
   unsigned int x;
   long n;
   int i, j;

   end = data + len - 3;
   n = 0;

   for (s = data; s < end;)
   {
      x = *s++ << 24;
      x |= *s++ << 16;
      x |= *s++ << 8;

      *(code++) = encode[x >> 26];
      x <<= 6;
      *(code++) = encode[x >> 26];
      x <<= 6;
      *(code++) = encode[x >> 26];
      x <<= 6;
      *(code++) = encode[x >> 26];
      n += 4;
   }

   end += 3;

   x = 0;
   for (i = 0; s < end; i++)
      x |= *s++ << (24 - 8 * i);

   for (j = 0; j < 4; j++)
   {
      if (8 * i >= 6 * j)
      {
         *(code++) = encode [x >> 26];
         x <<= 6;
         n++;
      }
      else
      {
         *(code++) = '=';
         n++;
      }
   }

   *code = 0;
   return n;
}

void SetTime ()
{
   TimeSet = millis ();
}


unsigned long TimeDiff ()
{
   return (millis () - TimeSet);   
}

void SetUpdateTime ()
{
   UpdateTimeSet = millis ();
}

unsigned long UpdateTimeDiff ()
{
   return (millis () - UpdateTimeSet);   
}

int GetInfo (bool create)
{
   char *request = "getinfo.php?";
   int port = 80;
   char mem [256];
   char *sep = "!#";
   char code [128];
   String data;

   if (create)
      data = "create=1&iot=";
   else data = "create=0&iot=";
   encode_base64 (EspName, strlen (EspName), code);      
   data += code; 

   data += "&email=";
   encode_base64 (Email, strlen (Email), code);      
   data += code; 

   data += "&modpw=";
   encode_base64 (EspPassword, strlen (EspPassword), code);      
   data += code; 

   char *p = (char*) data.c_str ();
           
   int ret = GetRequestExt (ServerHost, request, p, port, "&Inf0", 30, mem, sizeof (mem));  
   if (ret == -1 || ret == -2)
      return 0;
          
   Debugf ("Inf0 return in mem: %s\n", mem);
   if (strstr (mem, "&pending"))
      PendingMode = 1;
   else PendingMode = 0;

   p = strstr (mem, "&interval=");
   if (p)
   {
      p += 10;
      UpdateInterval = (atoi (p) * 1000);
      Debugf ("UpdateInterval set to: %d\n", UpdateInterval);
   }

   if (UpdateInterval < 30000)
   {
      Debugln ("UpdateInterval below 30 sec, set to 1 min.");
      UpdateInterval = 60000; // 1 minute
   }
   return ret;
}


int SendMail (char Option)
{
   //($use64, $mailtoe, $subjecte, $bodye, $fn, $buf) = split ('``!#`', $array);   
   char *request = "smf.php";   
   int port = 80;
   char mem [256];
   char *sep = "!#";
   char code [128];
   String data;

   encode_base64 (Email, strlen (Email), code);      
   data = code; // mailto
   data += sep;

   // subject
   switch (Option)
   {
      case SENDMAIL_OPTION_STARTED:   
         sprintf (mem, "%s: Started!", EspName);
         break;
      case SENDMAIL_OPTION_SENSOR1:
          sprintf (mem, "%s: Sensor Alert!", EspName);
         break;
   }
   
   encode_base64 (mem, strlen (mem), code);      
   data += code;
   data += sep;

   // body
   switch (Option)
   {
      case SENDMAIL_OPTION_STARTED:   
         strcpy (mem, "Info from web server has been readed\r\nModule updated!\r\n");
         break;
      case SENDMAIL_OPTION_SENSOR1:
         strcpy (mem, "Movement has been detected!");
         break;
   }
   
   encode_base64 (mem, strlen (mem), code);      
   data += code;

   char *p = (char*) data.c_str ();

   // answer is Mai1~S3nt
   //                 | is number 3
   //              | is number 1
   int ret = PostRequestExt (ServerHost, request, p, port, "Mai1~S3nt", 30, mem, sizeof (mem));   
   if (ret == -1 || ret == -2)
      return 0;
   return ret;

}

int count = 0;
int CountRequest = 0;
int PendingRequest = 0;
int MissedSendMail = 0;
void loop() 
{   
   int ret;
   
   switch (state)
   {
      case STATE_AP_MODE:
         // if no connection or no ssid or no password
         // then enter in AP mode, list AP and let the user select an AP from our page so 
         // he can select an AP, type a password and an email address                        
         StartInAPMode ();
         SetTime ();
         state = STATE_WAIT_INPUT;
         break;

      case STATE_WAIT_INPUT:
         // wait the user to select an AP from a web page
         // the handleRoot function will set state to STATE_GET_INFO once we get ssid+password

         // if we have already a ssid & password and 5 minutes have elapsed
         // then try to connect again, maybe wifi came back
         if (Ssid [0] && SsidPassword [0] && TimeDiff () > 180000) 
         {
            Debugln ("\nWas waiting for new AP info, but now time passed\nMust retry to connect to wifi again");
            if (TryToConnectToWifi (1))
            {
               state = STATE_GET_INFO;
               break;
            } 
            state = STATE_AP_MODE;          
            SetTime ();            
         }
         else server.handleClient();         
         break;

      case STATE_GET_INFO:
         if (WiFi.status () != WL_CONNECTED)
            if (!TryToConnectToWifi (3))
            {
               state = STATE_AP_MODE;
               break;
            }           
         if ((ret = GetInfo (1)) == 1)         
         {
            if (PendingMode)
            {
               state = STATE_PENDING_MODE;
               Debugln ("Going to: STATE_PENDING_MODE");
               PendingRequest = 0;
               SetTime ();
               break;
            }
            else
            {
               state = STATE_SEND_MAIL_STARTED;                        
               Debugln ("Going to: STATE_SEND_MAIL_STARTED");
               CountRequest = 0;
               break;
            }
         }
         if (++CountRequest > 2)
         {
            CountRequest = 0;
            if (ret == 0)
            {
               state = STATE_WAIT_FOR_SENSOR;
               SetUpdateTime (); 
            }
            else state = STATE_AP_MODE;
            MissedSendMail++;
            Debugln ("Giving up to try to get info from server, return to SENSOR MODE now");
         }
         delay (CountRequest * 3000);
         break;

      case STATE_PENDING_MODE: // will check 15 times only (15 minutes), after will recreate the file on server
         if (TimeDiff () > 60000) // 1 min
         {            
            SetTime ();
            if (GetInfo (0))
            {
               if (!PendingMode)
               {
                  state = STATE_SEND_MAIL_STARTED;                        
                  Debugln ("Got it activated\nGoing to STATE_SEND_MAIL_STARTED");
                  CountRequest = 0;
                  break;
               }
               Debugln ("Still in STATE_PENDING_MODE, user did not activate yet");
            }
            if (++PendingRequest >= 15)
            {
              state = STATE_GET_INFO;
              Debugln ("Did not get activated after 15 tries, giving up\nGoing to STATE_GET_INFO");
              break;
            }
            Debugln ("Waiting to get activated by the user");
         }
         break;
         
      case STATE_SEND_MAIL_STARTED:
         if (WiFi.status () != WL_CONNECTED)
            if (!TryToConnectToWifi (3))
            {
               state = STATE_AP_MODE;
               break;
            }
         if ((ret = SendMail (SENDMAIL_OPTION_STARTED)) == 1)
         {
            state = STATE_WAIT_FOR_SENSOR;            
            SetUpdateTime (); 
            CountRequest = 0;
            Debugln ("WiFi Disconnect\nGoing in WaitForSensor mode now");
            WiFi.disconnect ();
            break;
         }
         if (++CountRequest > 2)
         {
            CountRequest = 0;
            if (ret == 0)
            {
               state = STATE_WAIT_FOR_SENSOR;
               SetUpdateTime (); 
            }
            else state = STATE_AP_MODE;
            MissedSendMail++;
            Debugln ("Giving up to try to send an email, return to SENSOR MODE now");
         }
         delay (CountRequest * 3000);
         break;

      case STATE_UPDATE_INFO:
         if (UpdateTimeDiff () > UpdateInterval)
         {
            Debugln ("Going to update info on/from server");
            if (WiFi.status () != WL_CONNECTED)
               TryToConnectToWifi (1);
            if (WiFi.status () == WL_CONNECTED)
               GetInfo (0);
         }
         
         SetUpdateTime (); 
         state = STATE_WAIT_FOR_SENSOR;         
         break;
         
      case STATE_WAIT_FOR_SENSOR: 
         if (UpdateTimeDiff () > UpdateInterval)
         {
            state = STATE_UPDATE_INFO;
            break;
         }
         // code to do: check for whatever is connected to the esp   
         break;
   }
}

void setup () 
{  
   DebugInit ();   
   delay (100);
   Debugln ();

   StartTime = millis ();
   Debug ("\nChip ID: ");
   Debugln (ESP.getChipId ());
   
   sprintf (EspName, "ESPINO-%X", ESP.getChipId ());   
   Debugf ("EspName: %s\n", EspName);
   
   GetEEpromInfo ();   
   if (EspPassword [0] == 0)
      strcpy (EspPassword, EspDefaultPW);  
   if (Ssid [0] == 0 || SsidPassword [0] == 0 || Email [0] == 0)
      state = STATE_AP_MODE;
   else state = STATE_GET_INFO;    
}

