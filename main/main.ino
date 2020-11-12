#include <WiFi.h>
#include <Servo.h>
#define TOP 0
#define BOT 1
#define ON 0
#define OFF 1

// https://github.com/RoboticsBrno/ServoESP32
// https://randomnerdtutorials.com/solved-failed-to-connect-to-esp32-timed-out-waiting-for-packet-header/
// https://www.bneware.com/blogPost/esp32_arduino_ide
// https://blog.naver.com/roboholic84/220583130285
// https://arsviator.blogspot.com/2018/11/esp32-static-ip-using-static-ip-address.html
////////// motor
Servo servo[2];

const int servopin_top = 27;
const int servopin_bot = 25;

const int rotate_idle = 45;
int rotate[2][2] = {{0, 110},
                    {110, 0}};

const int _delay = 1000;

///////// server
const char* ssid = "-----";
const char* password = "-----";

// set static ip
IPAddress local_IP(192, 168, 123, 105);
IPAddress gateway(192, 168, 123, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 3, 3);

WiFiServer server(80);

// Variable to store the HTTP request
String header;

void setup() {
  Serial.begin(115200);
  
  // put your setup code here, to run once:
  servo[TOP].attach(servopin_top);
  servo[BOT].attach(servopin_bot);

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
  int isclick = 0, pos = 0, stat = 0, value = 0;
  servo[TOP].write(rotate_idle);
  servo[BOT].write(rotate_idle);

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

            if (header.indexOf("GET /top") >= 0){              
              pos = TOP;
              if (header.indexOf("GET /top/on") >= 0) {
                stat = ON;
                isclick = (header.indexOf("GET /top/on/set") >= 0) ? 2 : 1;
              }else{
                stat = OFF;
                isclick = (header.indexOf("GET /top/off/set") >= 0) ? 2 : 1;
              }
            }else if (header.indexOf("GET /bot") >= 0){              
              pos = BOT;
              if (header.indexOf("GET /bot/on") >= 0) {
                stat = ON;
                isclick = (header.indexOf("GET /bot/on/set") >= 0) ? 2 : 1;
              }else{
                stat = OFF;
                isclick = (header.indexOf("GET /bot/off/set") >= 0) ? 2 : 1;
              }
            }

            switch (isclick){
              case 1:
                servo[pos].write(rotate[pos][stat]);
                delay(_delay);
                servo[pos].write(rotate_idle);
                delay(_delay);
  
                isclick = 0;
                break;
             case 2:
                rotate[pos][stat] = get_value(16 + stat);
                isclick = 0;
                break;
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
            client.println("div{position: relative;display: inline-block;text-align: center;}");
            client.println("</style></head>");
                     
            // Web Page
            client.println("</head><body><h1>Remote Light switch Controller!!!</h1>");
//            client.println("<div style=\"position: absolute; top: 50%; left: 50%;\">");
            client.println("<div>");// style=\"float: left;\">");
            
            client.println("<div>");
            client.println("<p>Top Button</p>");
            client.println("<p><a href=\"/top/off\"><button class=\"buttonoff\">OFF</button></a></p>");
            client.println("<p><a href=\"/top/on\"><button class=\"buttonon\">ON</button></a></p>");
            client.println("<p>ON: " + String(rotate[TOP][ON]) + " OFF: " + String(rotate[TOP][OFF]) + "</p>");
            client.println("</div>");

            client.println("<div>");
            client.println("<p>Bottom Button</p>");
            client.println("<p><a href=\"/bot/off\"><button class=\"buttonoff\">OFF</button></a></p>");
            client.println("<p><a href=\"/bot/on\"><button class=\"buttonon\">ON</button></a></p>");
            client.println("<p>ON: " + String(rotate[BOT][ON]) + " OFF: " + String(rotate[BOT][OFF]) + "</p>");
            client.println("</div>");
            
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

int get_value(int start_idx){
  String temp = "";
  int idx = start_idx;
  for (idx; idx < start_idx+4; idx++){
    if (header[idx] != ' '){
      temp += header[idx];
    }else break;
  }
  Serial.println("value is " + temp);
  return temp.toInt();
}

// 왜인지 모르지만 한번만 사용 가능함 ㅜㅜ
Servo switching(Servo servo, int rotate){
  Serial.println("start rotate.");
  servo.write(rotate);
  delay(_delay);
  servo.write(rotate_idle);
  delay(_delay);
  Serial.println("end rotate.");

  return servo;
}
