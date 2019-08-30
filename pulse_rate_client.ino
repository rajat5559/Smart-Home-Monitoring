#define pulsePin A0    // Pulse sensor input pin A0
#include<ESP8266WiFi.h>   // Including ESP8266 library

 char ssid[] = "ESP8266";  // Replace with SSID of hotspot of local server
 char pass[] = "12345678"; // Replace with password of hotspot of local server
 
 IPAddress server(192,168,4,15);  // IP address of local server
 WiFiClient client;

 int rate[10];             // array to hold last ten IBI value       
 unsigned long sampleCounter = 0;  // used to determine pulse timing
 unsigned long lastBeatTime = 0;  // used to find IBI
 unsigned long lastTime = 0, N;  
 int BPM = 0;    // int that holds raw analog in 0. updated every 2mS
 int IBI = 0;   // int that holds time interval between beats! Must be seeded!
 int P = 512;   // used to find peak in pulse wave, seeded
 int T = 512;   // used to find trough in pulse wave, seeded
 int thresh = 512;  // used to find instant moment of heart beat, seeded
 int amp = 100;     // used to hold amplitude of pulse waveform, seeded            
 int Signal;        // holds incoming raw data
 boolean Pulse = false; // "True" when heartbeat is detected. "False" when not a "live beat".
 boolean firstBeat = true; // used to seed rate array so we startup with reasonable BPM          
 boolean secondBeat = true; // used to seed rate array so we startup with reasonable BPM
 boolean QS = false;  // Becomes true when ESP8266 finds a beat.

void setup()
{
  Serial.begin(9600);  // Serial communication at baud rate of 9600 for debugging purpose
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
  if (QS == true)  //if ESP8266 finds a beat
  { 
      if (client.connect(server, 80))  // Connect to local server
        {  
          client.print("3\r");  // before sending data first send ESP8266 ID as 3
          String pulseRate = String(BPM);  // Convert into string
          pulseRate +="\r";   // Add "r" as end delimiter
          Serial.print("Pulse rate: ");
          Serial.print(BPM);
          Serial.println(" BPM.");
          client.print(pulseRate);   // send data to local server
          Serial.println("Sent to local server..");
       }
       QS = false;
       client.stop();
       delay(15000);
     } 
     else if(millis() >= (lastTime + 2)) 
     {
       readPulse(); 
       lastTime = millis();
     } 
}

void readPulse() 
{
   Signal = analogRead(pulsePin);  //Read pulse sensor value            
   sampleCounter += 2;              // Keeps track of the time in mS             
   int N = sampleCounter - lastBeatTime; // Monitor the time since the last beat to avoid noise
   detectSetHighLow();  // find the peak and trough of the pulse wave
   // Now it's time to look for the heart beat
   // signal surges up in value every time there is a pulse
   if(N > 250) // avoid high frequency noise
   {    
     if((Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3))
        pulseDetected();
   }
   if (Signal < thresh && Pulse == true) 
   {  
      Pulse = false;
      amp = P - T;
      thresh = amp / 2 + T;  
      P = thresh;
      T = thresh;
   }
   if (N > 2500) 
   {
      thresh = 512;
      P = 512;
      T = 512;
      lastBeatTime = sampleCounter;
      firstBeat = true;            
      secondBeat = true;           
    }
}

void detectSetHighLow() 
{
    if(Signal < thresh && N > (IBI/5)* 3) // avoid dichrotic noise by waiting 3/5 of last IBI
      {  
        if (Signal < T) 
        {   // T is the trough                               
          T = Signal; // Keep track of lowest point in pulse wave
        }
      }
    if (Signal > thresh && Signal > P) // thresh condition helps avoid noise 
      {
        P = Signal;  // P is the peak
      } // Keep track of highest point in pulse wave
}

void pulseDetected() 
{
    Pulse = true;             // set the pulse flag when there is a pulse              
    IBI = sampleCounter - lastBeatTime; // time between beats in mS
    lastBeatTime = sampleCounter;       //keep track of time for next pulse    
    if (firstBeat) // if it's the first time beat is found
    {                         
       firstBeat = false;  //clear firstBeat flag 
       return;                            
     }
    if (secondBeat) // if this is second beat 
    {                   
        secondBeat = false;   // clear secondBeat flag
        for (int i = 0; i <= 9; i++) 
        {  
          rate[i] = IBI;
        }
     }
     word runningTotal = 0; // clear the runningTotal variable          
     for (int i = 0; i <= 8; i++) //Shift data in the rate array 
     {          
      rate[i] = rate[i + 1];    // and drop the oldest IBI value      
      runningTotal += rate[i];   // add up the 9 oldest IBI value     
     }
     rate[9] = IBI;                // add the latest IBI to the rate array      
     runningTotal += rate[9];      //add the latest IBI to runningTotal      
     runningTotal /= 10;           // average the last 10 IBI values      
     BPM = 60000 / runningTotal;   // how many beats can fit into a minute? that's BPM!      
     QS = true;
     if (client.connect(server, 80))  //Connects to local server
     {  
        client.print("3\r");            //before sending the data sends ESP8266 ID as 3
        String pulseRate = String(BPM);  // Converting integer data into string
        pulseRate +="\r";                // Add end Delimiter "r" in the data
        Serial.print("Pulse rate: ");
        Serial.print(BPM);
        Serial.println(" BPM.");
        client.print(pulseRate);        //sends data to locals server
        Serial.println("Sent to local server..");
     }
     client.stop();
     delay(15000);   // Wait for 15 seconds after each transmission
}
