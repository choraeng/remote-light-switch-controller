#include <WiFi.h>
#include <Servo.h>
#define OFF 0
#define ON 1

// https://github.com/RoboticsBrno/ServoESP32
// https://randomnerdtutorials.com/solved-failed-to-connect-to-esp32-timed-out-waiting-for-packet-header/
// https://www.bneware.com/blogPost/esp32_arduino_ide
// https://blog.naver.com/roboholic84/220583130285
// https://arsviator.blogspot.com/2018/11/esp32-static-ip-using-static-ip-address.htmlhttps://arsviator.blogspot.com/2018/11/esp32-static-ip-using-static-ip-address.html
////////// motor
Servo servo_top, servo_bot;

const int servopin_top = 27;
const int servopin_bot = 25;

const int rotate_idle = 45;
const int rotate_off  = 110;
const int rotate_on   = 00;

const int _delay = 1000;

///////// server
const char* ssid = "-----";
const char* password = "-----";

// set static ip
IPAddress local_IP(0,0,0,0);
IPAddress gateway(0,0,0,0);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 3, 3);

WiFiServer server(80);

const int state_top = 0;
const int state_bot = 0;

// Variable to store the HTTP request
String header;

void setup() {
  Serial.begin(115200);
  
  // put your setup code here, to run once:
  servo_top.attach(servopin_top);
  servo_bot.attach(servopin_bot);

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
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  servo_top.write(rotate_idle);
  servo_bot.write(rotate_idle);

  WiFiClient client = server.available();

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /top/on") >= 0){
              Serial.println("top on");
              //switching(servo_top, rotate_on);
              servo_top.write(rotate_on);
              delay(_delay);
              servo_top.write(rotate_idle);
              delay(_delay);
            }else if (header.indexOf("GET /top/off") >= 0){
              Serial.println("top off");
//              servo_top = switching(servo_top, rotate_off);
              servo_top.write(rotate_off);
              delay(_delay);
              servo_top.write(rotate_idle);
              delay(_delay);
            }else if (header.indexOf("GET /bot/on") >= 0){
              Serial.println("bot on");
//              servo_bot = switching(servo_bot, rotate_on);
              servo_bot.write(rotate_off);
              delay(_delay);
              servo_bot.write(rotate_idle);
              delay(_delay);
            }else if (header.indexOf("GET /bot/off") >= 0){
              Serial.println("bot off");
//              servo_bot = switching(servo_bot, rotate_off);
              servo_bot.write(rotate_on);
              delay(_delay);
              servo_bot.write(rotate_idle);
              delay(_delay);
            }
            

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".buttonoff { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonon {background-color: #77878A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println("</style></head>");
                     
            // Web Page
            client.println("</head><body><h1>Remote Light switch Controller!!!</h1>");
            client.println("<p>Top Button</p>");
            client.println("<div style=\"float: left;\">");
            client.println("<p><a href=\"/top/off\"><button class=\"buttonoff\">OFF</button></a></p>");
            client.println("<p><a href=\"/top/on\"><button class=\"buttonon\">ON</button></a></p>");
            client.println("</div>");

            client.println("<p>Bottom Button</p>");
            client.println("<div style=\"float: left;\">");
            client.println("<p><a href=\"/bot/off\"><button class=\"buttonoff\">OFF</button></a></p>");
            client.println("<p><a href=\"/bot/on\"><button class=\"buttonon\">ON</button></a></p>");
            client.println("</div>");
            client.println("</body></html>");     
             
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

  
}

Servo switching(Servo servo, int rotate){
  Serial.println("start rotate.");
  servo.write(rotate);
  delay(_delay);
  servo.write(rotate_idle);
  delay(_delay);
  Serial.println("end rotate.");

  return servo;
}
