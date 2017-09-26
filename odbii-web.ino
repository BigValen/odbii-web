#include <SoftwareSerial.h>

const char* ssid = "BinCity";
const char* password = "we love wifly";
const char* SEARCHING = "SEARCHING";
const char* NODATA = "NO DATA";

int blueRX   = D3;  //grey
int blueTX   = D2;  //white

//WiFiServer server(80);
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
  send_odb_cmd("ATZ", "reset", read_line, 100); // full reset
  send_odb_cmd("ATI", "ID", read_line, 100); // identification
  send_odb_cmd("AT/N", "serial", read_line, 100); // serial numbers
  send_odb_cmd("ATRV", "voltage", read_line, 100); // voltage

  send_odb_cmd("0100", "01-20 PIDs supported", read_line, 100);
  send_odb_cmd("0120", "21-40 PIDs supported", read_line, 100);
  send_odb_cmd("0140", "41-60 PIDs supported", read_line, 100);
  send_odb_cmd("0160", "61-80 PIDs supported", read_line, 100);
  send_odb_cmd("0180", "81-A0 PIDs supported", read_line, 100);


  send_odb_cmd("0101", "monitor status", read_line, 100);
  send_odb_cmd("0103", "fuel status", read_line, 100);
  send_odb_cmd("0104", "engine load", read_line, 100);
  send_odb_cmd("0105", "coolant temp", read_line, 100);
  send_odb_cmd("0106", "short term fuel trim b1", read_line, 100);
  send_odb_cmd("0107", "long term fuel trim b1", read_line, 100);
  send_odb_cmd("0108", "short term fuel trim b2", read_line, 100);
  send_odb_cmd("0109", "long term fuel trim b2", read_line, 100);
  send_odb_cmd("010A", "fuel pressure", read_line, 100);
  send_odb_cmd("010B", "intake pressure", read_line, 100);
  send_odb_cmd("010C", "engine rpm", read_line, 100);
  send_odb_cmd("010D", "vehicle speed", read_line, 100);
  send_odb_cmd("010E", "timing advance", read_line, 100);
  send_odb_cmd("010F", "intake air temp", read_line, 100);
  send_odb_cmd("0110", "MAF airflow rate", read_line, 100);
  send_odb_cmd("0111", "throttle position", read_line, 100);
  send_odb_cmd("0113", "O2 sensors present", read_line, 100);
  send_odb_cmd("011C", "ODB standards", read_line, 100);
  send_odb_cmd("011D", "O2 sensors present in 4 banks", read_line, 100);
  send_odb_cmd("011F", "runtime", read_line, 100);
  send_odb_cmd("0133", "barometric pressure", read_line, 100);
  send_odb_cmd("0142", "Control module voltage", read_line, 100);
  send_odb_cmd("0146", "ambient air temp", read_line, 100);
  send_odb_cmd("014F", "max val for sensor ratios", read_line, 100);
  send_odb_cmd("0150", "max val airflow rate", read_line, 100);
  send_odb_cmd("0151", "fuel type", read_line, 100);
  send_odb_cmd("015C", "Engine Oil Temp", read_line, 100);
  send_odb_cmd("015E", "Engine Fuel rate", read_line, 100);
  send_odb_cmd("0151", "fuel type", read_line, 100);

  send_odb_cmd("ATPC", "close session", read_line, 100);
  send_odb_cmd("0113", "o2 present?", read_line, 100); // O2 sensors present

  Serial.println("Sleeping for 5 seconds..");
  delay(5000);
}

// Returns length of line read
int get_odb_line(char *buffer, int buffer_len) {
  int i = 0;
  int data = '\0';
  // Stop one before buffer_len in case it's a long line
  while (i < buffer_len && data != '\r') {
    while (data < 1) {
      
      while(!Odb.available()) delay(10);
      data = Odb.read();
    }
    Serial.printf("got %c\n", data);
    if (data == '\r') {
          Serial.printf(" got cr\n");
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

int send_odb_cmd(const char *odb_cmd, const char *comment, char *read_line, int pause) {
  Serial.printf("sending: %s\n", odb_cmd);
  delay(pause);
  char odb_buffer[BUF_LEN];
  while(get_odb_line(odb_buffer, BUF_LEN) && odb_buffer[0])  {
    Serial.printf("Read In: [%s]\n", odb_buffer);
    if(!strncmp(odb_buffer, SEARCHING, strlen(SEARCHING))) {
      Serial.println("  Searching..(wait 2s) ");
      delay(2000);
    } else if (!strncmp(odb_buffer, NODATA, strlen(NODATA))) {
      Serial.println("  No Data");
    } else {
      Serial.printf("  %s <= [%s]\n", comment, read_line);
      strncpy(read_line, odb_buffer, BUF_LEN);
    }
  }
  return 0;
}


