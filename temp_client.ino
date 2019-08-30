#include<DHT.h>    //Including temperature and Humidity sensor library
#include<ESP8266WiFi.h>   //Including ESP8266 library
#define DHTPIN 0    // D3 pin of ESP8266

 char ssid[] = "ESP8266";   //Replace with ssid of hotspot of local server
 char pass[] = "12345678";   // Replace with password of hotspot of local server

 IPAddress server(192,168,4,15); // IP address of local server
 WiFiClient client;


 DHT dht(DHTPIN, DHT11);  // Data of DHT11 sensor in D3 pin of ESP8266

void setup()
{
    Serial.begin(9600); //serial communication at baud rate of 9600 for debugging purpose
    delay(10);
    dht.begin();  // start Temperature and Humidity sensor
    WiFi.mode(WIFI_STA);    // ESP8266 mode as station mode
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
    float h = dht.readHumidity();   // Read Humidity value from sensor
    float t = dht.readTemperature();  // Read temp value from sensor
    if(isnan(h) || isnan(t))
    {
        Serial.println("Failed to read from DHT sensor"); // Error message
        return;  //exit from code.
    }
    if(client.connect(server,80))  // Connect to local server
    { 
        client.print("1\r");   // before sending the data first send ESP8266 ID as 1
        String temp = String(t);
        temp += "\r";         // Add “r” as end delimiter 
        client.print(temp);  //  send temperature to local server
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print(" degree celcius, Humidity: ");
        Serial.print(h);
        Serial.print("%. ");
        String humidity = String(h);
        humidity += "\r";      // Add “r” in data as end delimiter
        client.print(humidity);  // send to Local server
        Serial.println("Sent to local server ");
        delay(15000);     // delay of 15sec after each transmission
    }
    client.stop();
}  
