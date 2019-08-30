#include <ESP8266WiFi.h>  // Including ESP8266 library
#define vib A0        // sensor input from A0 pin of ESP8266

 char ssid[] = "ESP8266";  //Replace with SSID of horspot of local server
 char pass[] = "12345678"; // Replace with password of hotspot of local server
 IPAddress server(192,168,4,15);   // IP address of local server
 WiFiClient client;

void setup()
{
    Serial.begin(9600);     // Serial communication at baud rate of 9600 for debugging purpose
    delay(10);
    pinMode(vib, INPUT);    // Input of vibration sensor
    WiFi.mode(WIFI_STA);   // ESP8266 as station mode
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
    Serial.print("LocalIP:"); Serial.println(WiFi.localIP());  // IP address of local server
    Serial.println("MAC:" + WiFi.macAddress());
    Serial.print("Gateway:"); Serial.println(WiFi.gatewayIP());
    Serial.print("AP MAC:"); Serial.println(WiFi.BSSIDstr());  // MAC address of access point
}

void loop()
{
    int val = analogRead(vib);  // Reads the sensor value
    if(client.connect(server,80)) //connects to local server
    {  
      client.print("4\r");    // Before sending the data sends ESP8266 ID as 4
      Serial.print("Vibration sensor value: ");
      Serial.println(val);
      String data = String(val);    // Converting integer data into string type
      data += "\r";         // Add end delimiter "r" in the data
      client.print(data);  // sends sensor data to local server
      Serial.println("Sent to Local server..!! ");
      delay(15000);              // After each transmission wait for 15 seconds
      client.stop();
   }
}
