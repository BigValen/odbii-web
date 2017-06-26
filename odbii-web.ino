#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

 
const char* ssid = "BinCity";
const char* password = "we love wifly";
 
int blueRX   = D3;  //grey
int blueTX   = D2;  //white

WiFiServer server(80);
SoftwareSerial Odb(blueRX, blueTX);
 
void setup() {
  Serial.begin(115200);
  Odb.begin(38400);


  /*
  Let's leave this out, until the serial communication works

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
  */
 
}
 
void loop() {
  Serial.flush();
  Odb.flush();
    
  char return_values[20];

  Serial.println("ATRV");
  Odb.println("ATRV");
  while(!Odb.available()) {
    delay(100);
  }
  while(Odb.available()) {
    char data = Odb.read();
    Serial.print(data);
  }
  Serial.println(" - alive!");

  int num_bytes = send_odb_cmd("0113", return_values, 1); // O2 sensors present
  char o2sensors_present = return_values[0];
  Serial.printf("Got expected 1 byte, %x (O2 sensors present, 2 banks)\n", o2sensors_present);



  // First 8 oxygen sensors
  for (int i=0 ; i < 8 ; i++) {
    if (1 << i & o2sensors_present) {
      char cmd_buf[8];
      snprintf(cmd_buf, sizeof(cmd_buf), "011%x", 4 + i);   
      num_bytes = send_odb_cmd(cmd_buf, return_values, 2); // O2 sensors present
      Serial.printf("Got 2 bytes 0x%x%x; sensor[%d], %fV, short fuel trim = %f\n", 
        return_values[0], return_values[1], i,
        float(return_values[0]/200), // Voltage
        float(((return_values[1] * 100) / 128) - 100)); // 

      snprintf(cmd_buf, sizeof(cmd_buf), "012%x", 4 + i); 
      num_bytes = send_odb_cmd(cmd_buf, return_values, 4);
      Serial.printf("Got 4 bytes, 0x%x%x%x; sensor[%d] %fV, fuel air ratio= %f\n",
        return_values[0], return_values[1], return_values[2], return_values[3], i, 
        float(((256 * return_values[2]) + return_values[3]) * 8 / 65536), // Voltage
        float(((256 * return_values[0]) + return_values[1]) * 8 / 65536)); // Fuel-air
    } else {
      Serial.printf("Skipping sensor %d, not present");
    }
  }
  num_bytes = send_odb_cmd("011D", return_values, 1);
  Serial.printf("Got expected 1 byte, %x (o2 present, 4 banks)\n", return_values[0]);


  if ( send_odb_cmd("0106", return_values, 1) == 1) {
    Serial.printf("Got 1 bytes, short-term fuel trim, bank 1 %2.2f\n", return_values[0]);
  }
  if ( send_odb_cmd("0107", return_values, 1) == 1) {
    Serial.printf("Got 1 bytes, long-term fuel trim, bank 1 %2.2f\n", return_values[0]);
  }
  if ( send_odb_cmd("0108", return_values, 1) == 1) {
    Serial.printf("Got 1 bytes, short-term fuel trim, bank 2 %2.2f\n", return_values[0]);
  }
  if ( send_odb_cmd("0109", return_values, 1) == 1) {
    Serial.printf("Got 1 bytes, long-term fuel trim, bank 2 %2.2f\n", return_values[0]);
  }

}

int send_odb_cmd(char *odb_cmd, char *return_values, int expected_bytes) {
  int return_bytes = 0;
  Serial.printf("sending: %s\n", odb_cmd);
  Odb.println(odb_cmd);
  while(! Odb.available()) {
    delay(50); // wait 50ms until there is data
  }
  
  while(Odb.available()) {
    char data = Odb.read();
    if (data == '\r') {
      if (return_bytes != expected_bytes) {
        Serial.printf("Did not get bytes as expected: %d != %d", expected_bytes, return_bytes);
      }
      return return_bytes;
    }
    if ( return_bytes >= sizeof(return_values)) {
      return_values[return_bytes]=0;
      Serial.printf("Got too much data: [%s]\n", return_values);
    }
    return_values[return_bytes++] = data;
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
