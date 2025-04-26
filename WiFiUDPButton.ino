#include <WiFi.h>
#include <NetworkUdp.h>

// WiFi network name and password:
const char *networkName = "";
const char *networkPswd = "";

const int udpPort = 5150;
IPAddress IP;
//Are we currently connected?
boolean connected = false;

//The udp library class
NetworkUDP udp;

#define KO1 18
#define KO2 19
#define K1 23

void setup() {
  // Initialize hardware serial:
  Serial.begin(115200);

  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);

  pinMode(K1, OUTPUT);
  pinMode(KO1, INPUT_PULLUP); 
  pinMode(KO2, INPUT_PULLUP); 
  digitalWrite(K1,LOW);
}


const uint8_t KIn[]={ KO1,KO2 };
bool NoKey=true;
 
void loop() {
  //only send data when connected
  if (connected) {
    
    int key=0;
      for( int j=0;j<2;j++ )
          {
      if(digitalRead(KIn[j])==LOW)
              {
                 key=j+1; break; 
              }
          }
     if(key==0){ NoKey=true; }
     else if( NoKey )
     {
         NoKey=false;
         Serial.printf("Send Key %d to IP address: ",key);
         Serial.println(IP);
            //Send a packet
            udp.beginPacket(IP, udpPort);
            udp.printf("Key%dPress", key);
            udp.endPacket();
     }
  }
  delay(100);
}

void connectToWiFi(const char *ssid, const char *pwd) {
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);  // Will call WiFiEvent() from another thread.

  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

// WARNING: WiFiEvent is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event) {
    
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
       IP=WiFi.localIP();
       IP[3]=255;
      //When connected set
      Serial.print("WiFi connected! IP address: ");
      Serial.println(WiFi.localIP());
      //initializes the UDP state
      //This initializes the transfer buffer
      udp.begin(udpPort);
      connected = true;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      connected = false;
      break;
    default: break;
  }
}
