#include <SoftwareSerial.h>

#define BUF_LEN 80
#define LEN_COMMANDS 20

const char* SEARCHING = "SEARCHING";
const char* NODATA = "NO DATA";
const int blueRX   = 3;  //grey
const int blueTX   = 2;  //white

SoftwareSerial Odb(blueRX, blueTX);
char read_line[BUF_LEN];
char odb_buffer[BUF_LEN];


void setup() {
  Serial.begin(9600);
  Serial.flush();
  Serial.println("\nStarting Softserial to ODBII");

  Odb.begin(38400);
  Serial.println("Booted now");
  Odb.flush();
}

void loop() {
  send_reset();
  char commandlist[LEN_COMMANDS];
  char commandbuf[6];
  send_odb_cmd("ATE0", "echo off", read_line, 100);
 // send_odb_cmd("ATSP0", "defaults", read_line, 1000); // hard-code parameters
  //send_odb_cmd("ATZ", "atz", read_line, 100); // reset
  //send_odb_cmd("ATI", "ID", read_line, 500); // identification
  //send_odb_cmd("AT/N", "serial", read_line, 500); // serial numbers
  send_odb_cmd("ATRV", "voltage", read_line, 100); // voltage

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


  send_odb_cmd("0100", "01-20 PIDs supported", read_line, 100);
  // should be  "00 03 07 08 09 12 17 24 28 29"
  int pid_count = decode_bytes(read_line, commandlist);
  for(int i = 0; i < pid_count; i++) {
    snprintf(commandbuf, sizeof(commandbuf), "%02d%02d", 0x01, commandlist[i]);
    send_odb_cmd(commandbuf, "command in 01", read_line, 100);
  }

  Serial.println("Pausing....");
  delay(60000);

  send_odb_cmd("0200", "21-40 PIDs supported", read_line, 100);
  send_odb_cmd("0140", "41-60 PIDs supported", read_line, 100);
  send_odb_cmd("0160", "61-80 PIDs supported", read_line, 100);
  send_odb_cmd("0180", "81-A0 PIDs supported", read_line, 100);
  Serial.println("Sleeping for 25 seconds..");
delay(25000);
}

int send_odb_cmd(const char *odb_cmd, const char *comment, char *buf, int pause) {
  
  Serial.print("> "); Serial.print(odb_cmd); Serial.print("   # "); Serial.println(comment);
  Odb.print(odb_cmd); Odb.print("\r");

  // read & discard the first two \r
  //get_odb_line(odb_buffer, BUF_LEN);
  //get_odb_line(odb_buffer, BUF_LEN);

  while(int got = get_odb_line(odb_buffer, BUF_LEN))  {
    //Serial.print("got ");
    //Serial.println(got);
    if(!strncmp(odb_buffer, SEARCHING, strlen(SEARCHING))) {
      Serial.println("  Searching..(waiting 2s) ");
      delay(2000);
    } else if(!strncmp(odb_buffer, NODATA, strlen(NODATA))) {
      Serial.println("  No Data");
    } else if(got <= 2) {
      Serial.println("");
    } else {
      strncpy(buf, odb_buffer, BUF_LEN);
      //Serial.print("copying into odbbuffer- got ");
      //Serial.print(got);
      //Serial.print(" bytes. Content: [");
      Serial.println(odb_buffer);
      //Serial.println("]");
    }
  }
  delay(pause);
  //Odb.flush();

  return 0;
}

int send_reset() {
  Odb.println("ATWS");
  delay(2000);
  Odb.flush();
  return 0;
}

int get_odb_line(char *buf, int buffer_len) {
  int i = 0;
  int data = 0;

  int max_wait=1500;
  int waited = 0;

  // Stop one before buffer_len in case it's a long line
  while (i < buffer_len - 1) {
    while (!Odb.available()) {
      if (waited > max_wait) {
        buf[i++] = '\0';
        Odb.flush();
        //Serial.println("  read timed out!");
        return 0;
      } else {
        waited += 50;
        delay(50);
      }
    }

    data = Odb.read();
    if (data == '\r') {
      //Serial.print('!');
      if (i==0) { i = 1; buf[0] = '\0'; }
      break;
    } else {
      buf[i++] = data;
    }
  }
  buf[i++] = '\0';
  //Serial.print("read "); Serial.println(i); Serial.print("<"); Serial.print(buf); Serial.println(">");
  return i;
}

// take a string that have to be decoded, return the count of commands, store actual pids in commandlist
int decode_bytes(char *hexstring, char *commandlist){
// "41 00 98 3B C0 13 " - first are always "41 ", second pair are the PID major number.

  int i = 5; int nonspaced=0; int commandcount=0;
  Serial.print("enabled pids: ");
  while(hexstring[++i] != 0) {
    int digit;
    if(hexstring[i] >= 48 && hexstring[i] <=57) { // it's a number
      digit = hexstring[i] - 48;
    } else if(hexstring[i] == 32) {
      continue;
    } else if(hexstring[i] >= 65 && hexstring[i] <=70) { // it's a hexletter
      digit = hexstring[i] - 65;
    } else {
      printf("Got garbage [%c] is a %d\n ", hexstring[i], hexstring[i]);
      continue;
    }
    for(int j=0; j<4; j++) {
      if(digit & (1 << j)) {
        int pid = (nonspaced * 4) + j;
        Serial.print(hexstring[3]);
        Serial.print(hexstring[4]);
        if ( pid < 10) Serial.print("0");
        Serial.print(pid);
        Serial.print(" ");
        commandlist[commandcount++] = pid;
      }
    }
    nonspaced++;
  }
  Serial.println("");
  return commandcount;
}


