#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library

#define SS_PIN 15
#define RST_PIN 16
#define ONBOARD_LED 2


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
 
MFRC522::MIFARE_Key key;

String tag = "";
String apiUrl = "http://api.callmebot.com/text.php";
String serverUrl = "http://143.244.134.183";
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void setup(){
  
Serial.begin(9600); // Init SPI bus
SPI.begin(); //Init MFRC522
rfid.PCD_Init(); //Init MFRC522
  
pinMode(ONBOARD_LED, OUTPUT); 

  //Internet  
  wifiMulti.addAP("Candy", "wifiwifi");
  wifiMulti.addAP("Homies", "localhost");
  wifiMulti.addAP("Galaxy", "12345678");
  
  while(wifiMulti.run() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Waiting for connection...");
  }
    
    blinkLed(ONBOARD_LED);
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

// Allocating keyByte
for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
}

// Initialize a NTPClient to get time
timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
timeClient.setTimeOffset(19800);

}

void loop(){

delay(2000);
// Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverUrl + "/read";
      //------------------------------------
      // Send HTTP GET request
      //------------------------------------
      String id = httpGet(http, client, serverPath);
      blinkLed(ONBOARD_LED);
      //------------------------------------
      //RFID
      //------------------------------------
      
      // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
      if ( ! rfid.PICC_IsNewCardPresent()) return;

      // Verify if the NUID has been readed
      if ( ! rfid.PICC_ReadCardSerial()) return;
      blinkLed(ONBOARD_LED);
      
      // Prints the PICC type
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.println(rfid.PICC_GetTypeName(piccType));

      //Update Time
      timeClient.update();
      String formattedTime = timeClient.getFormattedTime();
      String weekDay = weekDays[timeClient.getDay()];
      String month = months[10-1];

      if(id!=0){
        String card = printDec(rfid.uid.uidByte, rfid.uid.size);
        String tagPath = serverUrl + "/card?id="+String(id)+"&card="+String(card);
        httpGet(http, client, tagPath);
      }
      
      // Call Telegram
      String apiPath = apiUrl + "?user=@Lozerx&text="+month+"+"+formattedTime+"+"+weekDay;
      httpGet(http, client, apiPath);
      blinkLed(ONBOARD_LED);
      
      // Check is the PICC of Classic MIFARE type
      if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return;
      }
     
      // Halt PICC
      rfid.PICC_HaltA();

      // Stop encryption on PCD
      rfid.PCD_StopCrypto1();
      //-------------------------------------
      
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

// Blinks the in-built LED
void blinkLed(int led){
  digitalWrite(led, LOW);
  delay(300);
  digitalWrite(led, HIGH);
}

String httpGet(HTTPClient& http, WiFiClient& _client, String path){
  http.begin(_client, path.c_str());
  int httpResponseCode = http.GET();
    
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println(response);
        return response;
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
 }
 
 String printDec(byte *buffer, byte bufferSize) {
  tag = "";
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
    tag += String(buffer[i]);
  }
  return tag;
}
