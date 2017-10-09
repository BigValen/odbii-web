#include <SoftwareSerial.h>

#define BUF_LEN 80
#define LEN_COMMANDS 20

const char* SEARCHING = "SEARCHING";
const char* NODATA = "NO DATA";
const int blueRX   = 3;  //grey
const int blueTX   = 2;  //white

SoftwareSerial Odb(blueRX, blueTX);
char readline[BUF_LEN];
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
 // print_text_odb_cmd("ATSP0", "defaults", readline0); // hard-code parameters
  //print_text_odb_cmd("ATZ", "atz", readline); // reset
  //print_text_odb_cmd("ATI", "ID", readline, 500); // identification
  //print_text_odb_cmd("AT/N", "serial", readline, 500); // serial numbers
  print_text_odb_cmd((char *)"ATRV", "voltage", readline); // voltage

  print_hex_odb_cmd(0x0101, "monitor status", readline);
  print_hex_odb_cmd(0x0103, "fuel status", readline);
  print_hex_odb_cmd(0x0104, "engine load", readline);
  print_hex_odb_cmd(0x0105, "coolant temp", readline);
  print_hex_odb_cmd(0x0106, "short term fuel trim b1", readline);
  print_hex_odb_cmd(0x0107, "long term fuel trim b1", readline);
  print_hex_odb_cmd(0x0108, "short term fuel trim b2", readline);
  print_hex_odb_cmd(0x0109, "long term fuel trim b2", readline);
  print_hex_odb_cmd(0x010A, "fuel pressure", readline);
  print_hex_odb_cmd(0x010B, "intake pressure", readline);
  print_hex_odb_cmd(0x010C, "engine rpm", readline);
  print_hex_odb_cmd(0x010D, "vehicle speed", readline);
  print_hex_odb_cmd(0x010E, "timing advance", readline);
  print_hex_odb_cmd(0x010F, "intake air temp", readline);
  print_hex_odb_cmd(0x0110, "MAF airflow rate", readline);
  print_hex_odb_cmd(0x0111, "throttle position", readline);
  print_hex_odb_cmd(0x0113, "O2 sensors present", readline);
  print_hex_odb_cmd(0x011C, "ODB standards", readline);
  print_hex_odb_cmd(0x011D, "O2 sensors present in 4 banks", readline);
  print_hex_odb_cmd(0x011F, "runtime", readline);

  print_hex_odb_cmd(0x0121, "distance with CEL", readline);
  print_hex_odb_cmd(0x0122, "fuel pressure", readline);
  print_hex_odb_cmd(0x0123, "fuel gauge pressure", readline);

  String comment = String("O2 sensor ");
  for(int i=1; i<= 8; i++) { // 8 O2 sensors
    char commentbuf[15];
    String comment = String("O2 sensor ") + String(i);
    comment.toCharArray(commentbuf, sizeof(commentbuf));
    print_hex_odb_cmd(0x1023 + i, commentbuf, readline);
  }

  print_hex_odb_cmd(0x012C, "EGR %", readline);
  print_hex_odb_cmd(0x012F, "fuel %", readline);

  for(int i=1; i<= 8; i++) { // 8 O2 sensors
    char commentbuf[15];
    String comment = String("O2 sensor ") + String(i);
    comment.toCharArray(commentbuf, sizeof(commentbuf));
    print_hex_odb_cmd(0x1034 + i, commentbuf, readline);
  }
  for(int i=1; i<= 4; i++) { // 8 O2 sensors
    char commentbuf[15];
    String comment = String("cat temp ") + String(i);
    comment.toCharArray(commentbuf, sizeof(commentbuf));
    print_hex_odb_cmd(0x103C + i, commentbuf, readline);
  }

  print_hex_odb_cmd(0x0133, "barometric pressure", readline);
  print_hex_odb_cmd(0x0142, "Control module voltage", readline);
  print_hex_odb_cmd(0x0146, "ambient air temp", readline);
  print_hex_odb_cmd(0x014F, "max val for sensor ratios", readline);
  print_hex_odb_cmd(0x0150, "max val airflow rate", readline);
  print_hex_odb_cmd(0x0151, "fuel type", readline);
  print_hex_odb_cmd(0x015C, "Engine Oil Temp", readline);
  print_hex_odb_cmd(0x015E, "Engine Fuel rate", readline);
  print_hex_odb_cmd(0x0151, "fuel type", readline);

  for(int i=1; i<= 4; i++) { // 8 O2 sensors
    char commentbuf[15];
    String comment = String("o2 sensor trim ") + String(i);
    comment.toCharArray(commentbuf, sizeof(commentbuf));
    print_hex_odb_cmd(0x1055 + i, commentbuf, readline);
  }
  print_hex_odb_cmd(0x015E, "fuel rate", readline);
  print_hex_odb_cmd(0x0164, "eng torq", readline);

  print_text_odb_cmd((char *)"050100", "mon ids", readline);

  for(int i=1; i<= 16; i++) { // 16 O2 monitor bank sensors
    char commentbuf[15], commandbuf[7];

    String comment = String("o2 mon bank ") + String(i);
    comment.toCharArray(commentbuf, sizeof(commentbuf));

    String command = String("0501") + String(i, HEX);
    command.toCharArray(commandbuf, sizeof(commandbuf));

    print_text_odb_cmd(commandbuf, commentbuf, readline);
  }

  for(int i=1; i<= 16; i++) { // 16 O2 monitor bank sensors
    char commentbuf[15], commandbuf[7];
    String comment = String("o2 mon bank ") + String(i);
    comment.toCharArray(commentbuf, sizeof(commentbuf));

    String command = String("0502") + String(i, HEX);
    command.toCharArray(commandbuf, sizeof(commandbuf));

    print_text_odb_cmd(commandbuf, commentbuf, readline);
  }


  print_hex_odb_cmd(0x0100, "01-20 PIDs supported", readline);
/*
  // should be  "00 03 07 08 09 12 17 24 28 29"
  int pid_count = decode_bytes(readline, commandlist);
  for(int i = 0; i < pid_count; i++) {
    snprintf(commandbuf, sizeof(commandbuf), "%02d%02d", 0x01, commandlist[i]);
    print_text_odb_cmd(commandbuf, "command in 01", readline);
  }

  Serial.println("Pausing....");
  delay(60000);
*/
  print_hex_odb_cmd(0x0200, "21-40 PIDs supported", readline);
  print_hex_odb_cmd(0x0140, "41-60 PIDs supported", readline);
  print_hex_odb_cmd(0x0160, "61-80 PIDs supported", readline);
  print_hex_odb_cmd(0x0180, "81-A0 PIDs supported", readline);

  print_text_odb_cmd((char *)"051000", "05 OBD Monitor IDs supported", readline);

  Serial.println("Sleeping for 25 seconds..");
  delay(25000);
}

void print_hex_odb_cmd(int odb_cmd, const char *comment, char *buf) {
  char commandbuf[5];
  snprintf(commandbuf, sizeof(commandbuf), "%04x", odb_cmd);
  //Serial.print(odb_cmd); Serial.print(" is "); Serial.println(commandbuf);
  print_text_odb_cmd(commandbuf, comment, buf);
}

void print_text_odb_cmd(char *odb_string, const char *comment, char *buf) {
  Serial.print("> "); Serial.print(odb_string); Serial.print("   # "); Serial.println(comment);
  char *response = send_odb_cmd(odb_string, comment, buf);
  //if(response[0] == '>') {
    //Serial.println(response+2); // Skip the arrow.
  //} else {
    Serial.println(response);
  //}
}

char *send_odb_cmd(char *odb_cmd, const char *comment, char *buf) {
  Odb.print(odb_cmd); Odb.print("\r");
  odb_buffer[0] = '\0';

  while(int got = get_odb_line(odb_buffer, BUF_LEN))  {
    if(!strncmp(odb_buffer, SEARCHING, strlen(SEARCHING))) {
      Serial.println("  Searching..(waiting 2s) ");
      delay(2000);
    } else if(!strncmp(odb_buffer, NODATA, strlen(NODATA))) {
      Serial.println("  No Data");
    } else if(got <= 2) {
      // forget it
    } else {
      strncpy(buf, odb_buffer, BUF_LEN);
      //Serial.print("Got: ");
      //Serial.println(odb_buffer);
    }
  }
  return buf;
}

// Reset, and disable local-echo
int send_reset() {
  Odb.println("ATWS");
  delay(2000);
  Odb.println("ATE0");
  delay(100);
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
      if (i==0) { i = 1; buf[0] = '\0'; }
      break;
    } else {
      buf[i++] = data;
    }
  }
  buf[i++] = '\0';
  return i;
}

// take a string that have to be decoded, return the count of commands, store actual pids in commandlist
int decode_bytes(char *hexstring, char *commandlist) {
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

int monitor_status(char *readline) {
// > 0101
// 41 01 00 86 E8 00
  send_odb_cmd((char *)"0101", "monitor status", readline);
  int CEL = readline[2] & 1 <<7;       // Byte A, bit 7
  int emissions = readline[2] & (127);  // Byte A, bits 6-0;
  int diesel_type = readline[3] & 1<<3; // Byte B, bit 3
  if(diesel_type) Serial.println("Diesel");
  if(CEL) Serial.println("Check Eng");
  if(emissions) { Serial.print("Emission fail: "); Serial.println(emissions); }
  return CEL;
}

int engine_load(char *readline) {
// > 0104
// 41 04 44
  send_odb_cmd((char *) "0104", "engine load", readline);
  unsigned int load = readline[2] / 2.55;
  Serial.print("Eng Load %: ");
  Serial.println(load);
  return load;
}

int coolant_temp(char *readline) {
// > 0105
// 41 05 73
  send_odb_cmd((char *)"0105", "coolant temp", readline);
  signed int temperature = readline[2] -  40;
  Serial.print("cool temp %: ");
  Serial.println(temperature);
  return(temperature);
}

int engine_rpm(char *readline) {
// > 010C
// 41 0C 0C 7F
  send_odb_cmd((char *)"010C", "engine rpm", readline);
  unsigned int rpm = (readline[2] * 256 + readline[3]) / 4;
  Serial.print("rpm: ");
  Serial.println(rpm);
  return(rpm);
}

int velocity(char *readline) {
// > 010D
// 41 0D 00
  send_odb_cmd((char *)"010C", "velocity", readline);
  Serial.print("km/h: ");
  Serial.println(readline[2]);
  return(readline[2]);
}

int throttle(char *readline) {
// > 0111
//   41 11 00
  send_odb_cmd((char *)"0111", "throttle %", readline);
  unsigned int throttle = readline[2] / 2.55;
  Serial.print("throttle %: ");
  Serial.println(throttle);
  return(throttle);
}

int runtime(char *readline) {
// > 011F
// 41 1F 01 7E
  send_odb_cmd((char *)"011F", "runtime", readline);
  unsigned int runtime = (readline[2] * 256 + readline[3]);
  Serial.print("sec since start: ");
  Serial.println(runtime);
  return(runtime);
}
