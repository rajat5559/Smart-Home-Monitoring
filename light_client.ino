#include<ESP8266WiFi.h>   // Including ESP8266 library

 char ssid[]="ESP8266"; //Network ssid of hotspot of local server
 char pass[]="12345678"; // Password of hotspot of local server
 int val;
 int LDRpin = A0; //LDR Pin Connected to A0 pin
 IPAddress server(192,168,4,15);  // IP address of local server
 WiFiClient client;

void setup()
{
    Serial.begin(9600); // Serial communication at baud rate of 9600 for debugging purpose
    delay(10);
    WiFi.mode(WIFI_STA);  // ESP8266 in station mode
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid,pass);
    Serial.println();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("LocalIP:"); Serial.println(WiFi.localIP());
    Serial.println("MAC:" + WiFi.macAddress());
    Serial.print("Gateway:"); Serial.println(WiFi.gatewayIP());
    Serial.print("AP MAC:"); Serial.println(WiFi.BSSIDstr()); // MAC address of access point
}

void loop()
{
    val = analogRead(LDRpin);  // Reads the light sensor value
    if(client.connect(server,80))  // Connect to local server
    { 
        client.print("2\r");      
        Serial.print("LDR sensor value: ");
        Serial.println(val);
        String LDRval = String(val);
        LDRval += "\r";   // Add end delimiter 
        client.print(LDRval);  // Send to local server
        Serial.println("Sent to Local Server..");
        delay(15000);
    }
    client.stop();
}
