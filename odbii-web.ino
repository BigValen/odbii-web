#include <SoftwareSerial.h>

const char* ssid = "BinCity";
const char* password = "we love wifly";
const char* SEARCHING = "SEARCHING";
const char* NODATA = "NO DATA";

int blueRX   = D3;  //grey
int blueTX   = D2;  //white

WiFiServer server(80);
SoftwareSerial Odb(blueRX, blueTX);

#define BUF_LEN 120

void setup() {
  Serial.begin(115200);
  Odb.begin(38400);
}
 
void loop() {
  Serial.flush();
  Odb.flush();
  char read_line[BUF_LEN];
  send_odb_cmd("ATZ"); // full reset
  send_odb_cmd("ATI"); // identification
  send_odb_cmd("AT/N"); // serial numbers
  send_odb_cmd("ATRV"); // voltage

  send_odb_cmd("0100", "01-20 PIDs supported", results, 100);
  send_odb_cmd("0120", "21-40 PIDs supported", results, 100);
  send_odb_cmd("0140", "41-60 PIDs supported", results, 100);
  send_odb_cmd("0160", "61-80 PIDs supported", results, 100);
  send_odb_cmd("0180", "81-A0 PIDs supported", results, 100);


  send_odb_cmd("0101", "monitor status", results, 100);
  send_odb_cmd("0103", "fuel status", results, 100);
  send_odb_cmd("0104", "engine load", results, 100);
  send_odb_cmd("0105", "coolant temp", results, 100);
  send_odb_cmd("0106", "short term fuel trim b1", results, 100);
  send_odb_cmd("0107", "long term fuel trim b1", results, 100);
  send_odb_cmd("0108", "short term fuel trim b2", results, 100);
  send_odb_cmd("0109", "long term fuel trim b2", results, 100);
  send_odb_cmd("010A", "fuel pressure", results, 100);
  send_odb_cmd("010B", "intake pressure", results, 100);
  send_odb_cmd("010C", "engine rpm", results, 100);
  send_odb_cmd("010D", "vehicle speed", results, 100);
  send_odb_cmd("010E", "timing advance", results, 100);
  send_odb_cmd("010F", "intake air temp", results, 100);
  send_odb_cmd("0110", "MAF airflow rate", results, 100);
  send_odb_cmd("0111", "throttle position", results, 100);
  send_odb_cmd("0113", "O2 sensors present", results, 100);
  send_odb_cmd("011C", "ODB standards", results, 100);
  send_odb_cmd("011D", "O2 sensors present in 4 banks", results, 100);
  send_odb_cmd("011F", "runtime", results, 100);
  send_odb_cmd("0133", "barometric pressure", results, 100);
  send_odb_cmd("0142", "Control module voltage", results, 100);
  send_odb_cmd("0146", "ambient air temp", results, 100);
  send_odb_cmd("014F", "max val for sensor ratios", results, 100);
  send_odb_cmd("0150", "max val airflow rate", results, 100);
  send_odb_cmd("0151", "fuel type", results, 100);
  send_odb_cmd("015C", "Engine Oil Temp", results, 100);
  send_odb_cmd("015E", "Engine Fuel rate", results, 100);
  send_odb_cmd("0151", "fuel type", results, 100);

  send_odb_cmd("ATPC", "close session", results, 100);

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

  Serial.println("Sleeping for 5 seconds..");
  delay(5000);
}

// Returns length of line read
int get_odb_line(char *buffer, int buffer_len) {
  int i = 0;
  int data = '\0';
  // Stop one before buffer_len in case it's a long line
  while (i < buffer_len && data != '\r') {
    while (data < 1) data = Odb.read();
    if (data == '\r') {
      buffer[i] = '\0';
      return i;
    } else {
      buffer[i++] = data;
    }
  }
  buffer[i-1] = '\0';
  Serial.printf("buffer overflow at pos [%d]! [%s]\n", buffer_len, buffer);
  return buffer_len-1;
}

int send_odb_cmd(char *odb_cmd, char *comment, char *results, int pause) {
  Serial.printf("sending: %s\n", odb_cmd);
  delay(pause);
  char odb_buffer[BUF_LEN];
  while(get_odb_line(odb_buffer) && odb_buffer[0])  {
    Serial.printf("Read In: [%s]\n", odb_buffer);
    if !strncmp(odb_buffer, SEARCHING, sizeof(SEARCHING)) {
      Serial.println("  Searching..(wait 2s) ");
      delay(2000);
    } else if (!strncmp(odb_buffer, NODATA, sizeof(NODATA)) {
      Serial.println("  No Data");
    } else {
      Serial.printf("  %s <= [%s]\n", comment, results);
      strncpy(results, odb_buffer, BUF_LEN);
    }
  }
  return 0;
}

char *putOdbResponse(char *command){
  Odb.println(command);
  delay(500);
}
  
// The getOdbResponse function collects incoming data from the UART into the rxData buffer
// and only exits when a carriage return character is seen. Once the carriage return
// string is detected, the rxData buffer is null terminated (so we can treat it as a string)
// and the rxData index is reset to 0 so that the next string can be copied.
char *getOdbResponse(void){
  char inChar=0; 
  rxIndex = 0;
  while(inChar != '\r'){
    if(Odb.available() > 0){
      //Start by checking if we've received the end of message character ('\r').
      if(Odb.peek() == '\r'){
        //Clear the Serial buffer
        inChar=Odb.read();
        //Put the end of string character on our data string
        rxData[rxIndex]='\0';
        //Reset the buffer index so that the next character goes back at the beginning of the string.
        rxIndex=0;
        if(Odb.available() > 0 && Odb.peek() == '\r'){
          inChar=Odb.read();
        }
      } else {
        inChar = Odb.read();
        if(rxIndex > 0 || inChar != '>') { // Ignore prompts
          rxData[rxIndex++]=inChar;
        }
      }
    } else {
      delay(100);
      Serial.print(".");    
    }
  }
  if(strncmp(rxData, "SEARCHING...", sizeof(rxData))) {
    delay(500);
    getOdbResponse();
  }
  return rxData;
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
