#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

 
const char* ssid = "BinCity";
const char* password = "we love wifly";
 
int blueRX   = D3;  //grey
int blueTX   = D2;  //white

WiFiServer server(80);
SoftwareSerial bluetooth(blueRX, blueTX);
 
void setup() {
  Serial.begin(115200);
  bluetooth.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");

 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
 // Check if there is anything coming in from Serial
  if (Serial.available() > 0) {
    while ( auto inByte = Serial.read()) {
       Serial.write(inByte);
       bluetooth.write(inByte);
    }
  }
  // Check if there is anything coming from Bluetooth
  if (bluetooth.available() > 0 ) {
    Serial.write('-');
    while ( auto outByte = bluetooth.read()) {
       Serial.write(outByte);
    }
  }
}
  
  /*
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(5);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');

  Serial.println(request);
  client.flush();
 
 
  if (request.indexOf("/") != -1)  {
  
  
  }
  
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html><header><title>NixieController!</title><link rel=\"icon\" type=\"image/png\" href=\"https://www.arduino.cc/favicon.ico\"></header>");

  client.print("<p>Nixie Tube is now: ");
  client.println("<br><br><form method=\"get\">");
  client.println("<a href=\"/LED=ON\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"><button>Turn Off </button></a><br />");  
  client.println("Nixie: <input type=\"text\" name=\"nixie\"  maxlength=\"4\"></form></html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}
  */
