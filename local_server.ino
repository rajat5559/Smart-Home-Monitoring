#include <ESP8266WiFi.h>   //Including ESP8266 library
#include<ESP8266WebServer.h>  //Including ESP8266WebServer library for web server
#include<ThingSpeak.h>    //Including ThingSpeak library

 IPAddress IP(192,168,4,15);  //Static IP address of local server
 IPAddress gateway(192,168,4,1); //Gateway of the network 
 IPAddress mask(255, 255, 255, 0); //Subnet mask of the network
 WiFiClient client;
 WiFiServer server(80);  //server at port 80
 
 unsigned long myChannelNumber = 814287;  //Replace with channelID of ThingSpeak channel ID
 const char * myWriteAPIKey = "EK4LTPHWW4GGEOVP"; //Replace with WriteAPIKey of the channel to upload the data

 const char* softAPssid = "ESP8266";    //SSID of the hotspot of ESP8266 acting as local server
 const char* password = "12345678";     //Password of the hotspot of ESP8266 acting as local server

 const char* wifissid = "hello";  //Replace with SSID of WIF router providing internet access
 const char* pass = "12345678";         //Password of WIFI router providing internet access

void setup() 
{
   WiFi.mode(WIFI_AP_STA);   //ESP8266 mode as station mode and access point mode both at the same time
   Serial.begin(9600);     //Serial communication at baud rate of 9600 for debugging purpose
   delay(100);   //delay of 100 ms
   Serial.println(WiFi.getMode());  //print the operating mode of ESP to the serial monitor
   Serial.print("Configuring SoftAP....");
   Serial.println(WiFi.softAPConfig(IP,gateway,mask)? "Ready" : "Failed");  //print IP address, gateway and subnet mask
   delay(10);
   Serial.println("Setting SoftAP...");
   Serial.println(WiFi.softAP(softAPssid,password));   // print SSID and Passwaord of ESP8266 access point
   delay(10);
   Serial.println(WiFi.softAPIP());
   delay(500);
   WiFi.begin(wifissid,pass);  //Connect ESP8266 with WiFi router providing internet access
   while(WiFi.status()!=WL_CONNECTED)
   {
      Serial.print(".");
      delay(500);
   }
   Serial.print("Connected to Wifi with ssid ");  //print message
   Serial.println(wifissid);
   Serial.print("WiFi IP address: ");
   Serial.println(WiFi.localIP()); // WIFI router IP address
   ThingSpeak.begin(client);   //connect client with ThingSpeak server
   server.begin(); //Start local server
}

void loop()
{
    Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
    WiFiClient client = server.available();           //Waiting for the incoming data if client is ready to send 
    if (!client) {return;}
    String select_fun = client.readStringUntil('\r');  //Reads the ESP8266 ID (clients)
    if(select_fun=="1")   //If ESP1 sends the data 
    {   
        String temp = client.readStringUntil('\r');  //Reads the temperature value
        String Humidity = client.readStringUntil('\r');  //Reads the humidity value
        //Upload the temp value to ThingSpeak server as first field of channel
        ThingSpeak.writeField(myChannelNumber, 1,temp, myWriteAPIKey); 
        delay(15000);     //Wait for 15 sec after one entry
        //Upload the humidity value to ThingSpeak server as second field of channel
        ThingSpeak.writeField(myChannelNumber, 2,Humidity, myWriteAPIKey); 
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.print(" degree celcius, Humidity: ");
        Serial.print(Humidity);
        Serial.print("%. ");
        Serial.println("Sent to ThingSpeak Server...");
    }
    if(select_fun=="2")  //If ESP2 sends the data
    {  
        String LDRval = client.readStringUntil('\r');   //Reads light sensor value
        //Upload the light sensor value to ThingSpeak server as third field of channel
        ThingSpeak.writeField(myChannelNumber, 3,LDRval, myWriteAPIKey);  
        Serial.print("LDR sensor data value: ");
        Serial.println(LDRval);
        Serial.println("Sent to ThingSpeak Server...");
    }
    if(select_fun=="3")  //If ESP3 sends the data
    {   
        String pulseRate = client.readStringUntil('\r');  //Reads pulse rate
        //Upload the pulse rate to ThingSpeak server as fourth field of channel
        ThingSpeak.writeField(myChannelNumber, 4,pulseRate, myWriteAPIKey);
        Serial.print("Pulse rate: ");
        Serial.print(pulseRate);
        Serial.println(" BPM. Sent to ThingSpeak Server..");
    }
    if(select_fun=="4")  //If ESP4 sends the data
    {  
        String Vibval = client.readStringUntil('\r');  //Reads vibration sensor data
        //Upload the vibration sensor data value to ThingSpeak server as fivth field of channel
        ThingSpeak.writeField(myChannelNumber, 5,Vibval, myWriteAPIKey);
        Serial.print("Vibration Sensor data: ");
        Serial.print(Vibval);
        Serial.println(" Sent to ThingSpeak server..");
    }
    delay(15000); //waits for 15 secs after each transmission
}
