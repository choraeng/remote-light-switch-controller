#include <WiFi.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <Servo.h>

// https://github.com/RoboticsBrno/ServoESP32
// https://randomnerdtutorials.com/solved-failed-to-connect-to-esp32-timed-out-waiting-for-packet-header/
// https://www.bneware.com/blogPost/esp32_arduino_ide
// https://blog.naver.com/roboholic84/220583130285
// https://arsviator.blogspot.com/2018/11/esp32-static-ip-using-static-ip-address.html
 
const char* ssid = "----";
const char* password = "----";
 
AsyncWebServer server(80);
 
void setup(){
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/bot/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello World");
    Serial.println("rcv: turn on light");
  });
 
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "The content you are looking for was not found.");
  });
 
  server.begin();
}
 
void loop(){}
