#include <WiFi.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <Servo.h>

#define TOP 0
#define BOT 1
#define ON 1
#define OFF 0

// https://github.com/RoboticsBrno/ServoESP32
// https://randomnerdtutorials.com/solved-failed-to-connect-to-esp32-timed-out-waiting-for-packet-header/
// https://www.bneware.com/blogPost/esp32_arduino_ide
// https://blog.naver.com/roboholic84/220583130285
// https://arsviator.blogspot.com/2018/11/esp32-static-ip-using-static-ip-address.html
 
const char* ssid = "iptime";
const char* password = "pppppppp";

// set static ip
IPAddress local_IP(192, 168, 0, 120);
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 3, 3);
 
AsyncWebServer server(5700);

Servo servo[2];

const int servopin_top = 25;
const int servopin_bot = 27;
const int rotate_idle = 55;
//int rotate[2][2] = {{0, 110},
//                    {110, 0}};
int rotate[2] = {5, 105};

String index_html = "";

void init_servo(){
  servo[0].attach(servopin_top);
  servo[1].attach(servopin_bot);

  servo[0].write(rotate_idle);
  servo[1].write(rotate_idle);
}

void init_wifi(){
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)){
    Serial.println("STA failed to configure");
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void init_server(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
  });
  
  server.on("/bot/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
    run_motor(BOT, ON);
  });

  server.on("/bot/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
    run_motor(BOT, OFF);
  });

  server.on("/top/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
    run_motor(TOP, OFF);
    run_motor(TOP, ON);
  });

  server.on("/top/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
    run_motor(TOP, OFF);
    run_motor(TOP, ON);
  });

 
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "The content you are looking for was not found.");
  });
}

void setup(){
  Serial.begin(115200);
  
  init_index_html();
  init_servo();
  init_wifi();
  init_server();
  
  server.begin();
}
 
void loop(){}

void run_motor(int locate, int action){
  servo[locate].write(rotate[action]);
  delay(1000);
  servo[locate].write(rotate_idle);
}

void init_index_html(){
  // Display the HTML web page
  index_html += "<!DOCTYPE html><html>\n";
  index_html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  index_html += "<link rel=\"icon\" href=\"data:,\">\n";
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  index_html += "<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}\n";
  index_html += ".buttonoff { background-color: #195B6A; border: none; color: white; padding: 16px 40px;\n";
  index_html += "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}\n";
  index_html += ".buttonon {background-color: #77878A; border: none; color: white; padding: 16px 40px;\n";
  index_html += "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}\n";
  index_html += "div{position: relative;display: inline-block;text-align: center;}\n";
  index_html += "</style></head>\n";
              
  // Web Page
  index_html += "</head><body><h1>Remote Light switch Controller!!!</h1>\n";
  index_html += "<div>\n";
  
  index_html += "<div>\n";
  index_html += "<p>Top Button</p>\n";
  index_html += "<p><a href=\"/top/off\"><button class=\"buttonoff\">OFF</button></a></p>\n";
  index_html += "<p><a href=\"/top/on\"><button class=\"buttonon\">ON</button></a></p>\n";
//  index_html += "<p>ON: " + String(rotate[TOP][ON]) + " OFF: " + String(rotate[TOP][OFF]) + "</p>\n";
  index_html += "</div>\n";
  
  index_html += "<div>\n";
  index_html += "<p>Bottom Button</p>\n";
  index_html += "<p><a href=\"/bot/off\"><button class=\"buttonoff\">OFF</button></a></p>\n";
  index_html += "<p><a href=\"/bot/on\"><button class=\"buttonon\">ON</button></a></p>\n";
//  index_html += "<p>ON: " + String(rotate[BOT][ON]) + " OFF: " + String(rotate[BOT][OFF]) + "</p>\n";
  index_html += "</div>\n";
  
  index_html += "</div>\n";
  index_html += "</body></html>\n";     
      
  // The HTTP response ends with another blank line
  index_html += "\n";
}
