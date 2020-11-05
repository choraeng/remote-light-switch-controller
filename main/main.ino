#include <WiFi.h>
#include <Servo.h>
#define OFF 0
#define ON 1
// https://github.com/RoboticsBrno/ServoESP32

////////// motor
Servo servo_top, servo_bot;

const int servopin_top = 13;
const int servopin_bot = 13;

const int rotate_idle = 00;
const int rotate_off  = 00;
const int rotate_on   = 00;

const int _delay = 1000;

///////// server
const char* ssid = "WIFI"
const char* password = "PASSWORD"

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

  servo_top.write(rotate_idle)

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
              switching(servo_top, rotate_on);
            }else if (header.indexOf("GET /top/off") >= 0){
              Serial.println("top off");
              switching(servo_top, rotate_off);
            }else if (header.indexOf("GET /bot/on") >= 0){
              Serial.println("bot on");
              switching(servo_bot, rotate_on);
            }else if (header.indexOf("GET /bot/off") >= 0){
              Serial.println("bot off");
              switching(servo_bot, rotate_off);
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
            client.println(".buttonon {background-color: #77878A;}</style></head>");
                     
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

void switching(Servo servo, int rotate){
  servo.write(rotate);
  delay(_delay);
  servo.write(rotate_idle);
  delay(_delay);
}
