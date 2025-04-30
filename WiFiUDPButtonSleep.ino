#include <WiFi.h>
#include <NetworkUdp.h>
#include "driver/rtc_io.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc.h"

const char *networkName = "";
const char *networkPswd = "";

const int udpPort = 5150;
IPAddress IP;
NetworkUDP udp;

#define KO1 GPIO_NUM_25
#define KO2 GPIO_NUM_32
#define K1 GPIO_NUM_33
RTC_DATA_ATTR int Key=0;

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO) 
uint64_t bitmask = BUTTON_PIN_BITMASK(KO1) | BUTTON_PIN_BITMASK(KO2);

void setup() {
  Serial.begin(9600);
  
  print_wakeup_reason();
  esp_sleep_enable_ext1_wakeup_io(bitmask, ESP_EXT1_WAKEUP_ANY_HIGH);
  rtc_gpio_pulldown_en(KO1);
  rtc_gpio_pullup_dis(KO1);
  rtc_gpio_pulldown_en(KO2);
  rtc_gpio_pullup_dis(KO2);
  pinMode(K1, OUTPUT);
  digitalWrite(K1,HIGH);
  gpio_hold_en(K1);

  connectToWiFi(networkName, networkPswd);
}


void print_GPIO_wake_up(){
  uint64_t GPIO_reason = esp_sleep_get_ext1_wakeup_status();
  Key=(log(GPIO_reason))/log(2);
  Serial.print("GPIO that triggered the wake up: GPIO ");
  Serial.println(Key, 0);
}

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     
      Serial.println("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      print_GPIO_wake_up();
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("Wakeup caused by ULP program");
      break;
    default:
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
      break;
  }
}
 
void loop() {}

void connectToWiFi(const char *ssid, const char *pwd) {
  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent); 
  WiFi.begin(ssid, pwd);
  Serial.println("Waiting for WIFI connection...");
}

void SendKeyPress()
{
  Serial.printf("Key %d Press",Key);
  Serial.println("");
  int k=0;
  if( Key==25 ){ k=1; }
  else if( Key==32 ){ k=2; }
  if(k)
  {
    Serial.printf("Send Key %d to IP address: ",k);
    Serial.println(IP);
    udp.beginPacket(IP, udpPort);
    udp.printf("Key%dPress", k);
    udp.endPacket(); 
  } 
  Key=0;
  delay(500);
  esp_deep_sleep_start();
}

void WiFiEvent(WiFiEvent_t event) {   
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
       IP=WiFi.localIP();
       IP[3]=255;
       Serial.print("WiFi connected! IP address: ");
       Serial.println(WiFi.localIP());
       udp.begin(udpPort);
       SendKeyPress();
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      
    break;
    default: break;
      }
 }
