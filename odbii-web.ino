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
  send_odb_cmd("ATZ", "reset", read_line); // full reset
  delay(2); Odb.flush();

  send_odb_cmd("ATI", "ID", read_line); // identification

  send_odb_cmd("AT/N", "serial", read_line); // serial numbers
  send_odb_cmd("ATRV", "voltage", read_line); // voltage

  send_odb_cmd("0100", "01-20 PIDs supported", read_line);
  send_odb_cmd("0120", "21-40 PIDs supported", read_line);
  send_odb_cmd("0140", "41-60 PIDs supported", read_line);
  send_odb_cmd("0160", "61-80 PIDs supported", read_line);
  send_odb_cmd("0180", "81-A0 PIDs supported", read_line);


  send_odb_cmd("0101", "monitor status", read_line);
  send_odb_cmd("0103", "fuel status", read_line);
  send_odb_cmd("0104", "engine load", read_line);
  send_odb_cmd("0105", "coolant temp", read_line);
  send_odb_cmd("0106", "short term fuel trim b1", read_line);
  send_odb_cmd("0107", "long term fuel trim b1", read_line);
  send_odb_cmd("0108", "short term fuel trim b2", read_line);
  send_odb_cmd("0109", "long term fuel trim b2", read_line);
  send_odb_cmd("010A", "fuel pressure", read_line);
  send_odb_cmd("010B", "intake pressure", read_line);
  send_odb_cmd("010C", "engine rpm", read_line);
  send_odb_cmd("010D", "vehicle speed", read_line);
  send_odb_cmd("010E", "timing advance", read_line);
  send_odb_cmd("010F", "intake air temp", read_line);
  send_odb_cmd("0110", "MAF airflow rate", read_line);
  send_odb_cmd("0111", "throttle position", read_line);
  send_odb_cmd("0113", "O2 sensors present", read_line);
  send_odb_cmd("011C", "ODB standards", read_line);
  send_odb_cmd("011D", "O2 sensors present in 4 banks", read_line);
  send_odb_cmd("011F", "runtime", read_line);
  send_odb_cmd("0133", "barometric pressure", read_line);
  send_odb_cmd("0142", "Control module voltage", read_line);
  send_odb_cmd("0146", "ambient air temp", read_line);
  send_odb_cmd("014F", "max val for sensor ratios", read_line);
  send_odb_cmd("0150", "max val airflow rate", read_line);
  send_odb_cmd("0151", "fuel type", read_line);
  send_odb_cmd("015C", "Engine Oil Temp", read_line);
  send_odb_cmd("015E", "Engine Fuel rate", read_line);
  send_odb_cmd("0151", "fuel type", read_line);


  send_odb_cmd("0113", "o2 present?", read_line); // O2 sensors present
  send_odb_cmd("ATPC", "close session", read_line);

  Serial.println("Sleeping for 5 seconds..");
  delay(5000);
}

// Returns length of line read
int get_odb_line(char *buf, int buffer_len) {
  int i = 0;
  int data = '\0';
  // Stop one before buffer_len in case it's a long line
  while (i < buffer_len - 1) {
    while (!data) {
      data = Odb.available();
      delay(100);
    }
    data = Odb.read();

    if (data == '\r') {
      break;
    } else {
      buf[i++] = data;
    }
  }
  buf[i++] = '\0';
  return i;
}


int send_odb_cmd(const char *odb_cmd, const char *comment, char *buf) {
  Serial.print("[ "); Serial.println(odb_cmd);
  Odb.print(odb_cmd); Odb.print("\r");

  while(get_odb_line(odb_buffer, BUF_LEN) && odb_buffer[0])  {
    if(!strncmp(odb_buffer, SEARCHING, strlen(SEARCHING))) { // read another line
      Serial.println("  Searching..(wait 2s) ");
    } else if(!strncmp(odb_buffer, NODATA, strlen(NODATA))) { // read another line
      Serial.println("  No Data");
    } else if(odb_buffer[0] == 0) { // If we get 0, that means we got a \r\r, EOT
      return strlen(buf);
    } else {
      strncpy(buf, odb_buffer, BUF_LEN);
      Serial.print(comment); Serial.print(" <= "); Serial.println(odb_buffer);
      Serial.print(comment); Serial.print(" <- "); Serial.println(buf);
    }
  }
  return 0;
}

