/**
 * CSE205 Assessment 1 
 * @author Yudi An
 * 1201562
 * Y3-ICS
 * Arduino Websocket Server
 */
 
#include <TimeAlarms.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <WebSocket.h>
#include <Time.h>  


// Enabe debug tracing to Serial port.
#define DEBUG

// Here we define a maximum framelength to 64 bytes. Default is 256.
#define MAX_FRAME_LENGTH 64
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x79, 0xD4 };
//byte ip[] = { 192, 168, 1 , 17 }; just for test
//boolean n =true;

IPAddress timeServer(17,83,253,7);
 
/* Set this to the offset (in seconds) to your local time
   This example is GMT - 6 */
const long timeZoneOffset = 0L;  
 
/* Syncs to NTP server every 15 seconds for testing,
   set to 1 hour or more to be reasonable */
unsigned int ntpSyncTime = 15;        
 
 
/* ALTER THESE VARIABLES AT YOUR OWN RISK */
// local port to listen for UDP packets
unsigned int localPort = 8888;
// NTP time stamp is in the first 48 bytes of the message
const int NTP_PACKET_SIZE= 48;      
// Buffer to hold incoming and outgoing packets
byte packetBuffer[NTP_PACKET_SIZE];  
// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;                    
// Keeps track of how long ago we updated the NTP server
unsigned long ntpLastUpdate = 0;    
// Check last time clock displayed (Not in Production)
time_t prevDisplay = 0;

// Do not alter this function, it is used by the system
int getTimeAndDate() {
   int flag=0;
   Udp.begin(localPort);
   sendNTPpacket(timeServer);
   delay(1000);
   if (Udp.parsePacket()){
     Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer
     unsigned long highWord, lowWord, epoch;
     highWord = word(packetBuffer[40], packetBuffer[41]);
     lowWord = word(packetBuffer[42], packetBuffer[43]);  
     epoch = highWord << 16 | lowWord;
     epoch = epoch - 2208988800 + timeZoneOffset;
     flag=1;
     setTime(epoch);
     ntpLastUpdate = now();
   }
   return flag;
}
 
// Do not alter this function, it is used by the system
unsigned long sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;                  
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket();
}

// Create a Websocket server
WebSocket wsServer;


//complete the function called by websocket.h
void onConnect(WebSocket &socket) {
  char* str = "Arduino connected!";
  socket.send(str,strlen(str));
}



// You must have at least one function with the following signature.
// It will be called by the server when a data frame is received.
void onData(WebSocket &socket, char* dataString, byte frameLength) {
  
#ifdef DEBUG
  Serial.print("Got data from user: ");
  Serial.write((unsigned char*)dataString, frameLength);
  Serial.println();
#endif
//if(Serial.available() ) 
//  {
//    processSyncMessage();
// }
time_t t = now(); 

String time = "c@Arduino|";
time += t;
time+="|";
Serial.print("hhh:");
Serial.print(time);
int str_len = time.length() + 1; 

// Prepare the character array (the buffer) 
char char_array[str_len];

// Copy it over 
time.toCharArray(char_array, str_len); 
if(strcmp(dataString, "hi") == 0){   
  dataString = "Welcome to come back! The inner temperature of our home is 26 degree";
  char* totalLine = (char *)malloc(strlen(char_array) + strlen(dataString) + 1);
//char* totalLine;
  strcpy(totalLine, char_array);
  strcat(totalLine, dataString);
  dataString = totalLine;
}
else if(strcmp(dataString, "how are you") == 0){
  dataString = "I am fine, thank you";
  char* totalLine = (char *)malloc(strlen(char_array) + strlen(dataString) + 1);
//char* totalLine;
  strcpy(totalLine, char_array);
  strcat(totalLine, dataString);
  dataString = totalLine;
  
}

else if(strcmp(dataString, "turn on light") == 0){
  dataString = "OK! Light will be truned on.";
  char* totalLine = (char *)malloc(strlen(char_array) + strlen(dataString) + 1);
//char* totalLine;
  strcpy(totalLine, char_array);
  strcat(totalLine, dataString);
  dataString = totalLine;
}
else if(strcmp(dataString, "thank you") == 0){
  dataString = "You are welcome.";
  char* totalLine = (char *)malloc(strlen(char_array) + strlen(dataString) + 1);
//char* totalLine;
  strcpy(totalLine, char_array);
  strcat(totalLine, dataString);
  dataString = totalLine;
}else{

  // Just echo back data with "arduino said "for fun.
char* str = "I can not understand: ";
char* totalLine = (char *)malloc(strlen(str) + strlen(dataString) + 1);
//char* totalLine;
  strcpy(totalLine, str);
  strcat(totalLine, dataString);
  dataString = totalLine;
  char* totalLine1 = (char *)malloc(strlen(char_array) + strlen(dataString) + 1);
//char* totalLine;
  strcpy(totalLine1, char_array);
  strcat(totalLine1, dataString);
  dataString = totalLine1;
 
}
 socket.send(dataString, strlen(dataString));
}

//void onDisconnect(WebSocket &socket) {
//  char* str = "bye";
//  socket.send(str,strlen(str));
//}

void setup() {
#ifdef DEBUG  
  Serial.begin(9600);
#endif
  Ethernet.begin(mac);//to accept local ip
  
  wsServer.registerConnectCallback(&onConnect);
  wsServer.registerDataCallback(&onData);
//  wsServer.registerDisconnectCallback(&onDisconnect);  
  wsServer.begin();
  
   int trys=0;
   while(!getTimeAndDate() && trys<10) {
     trys++;
   }
 
  delay(100); // Give Ethernet time to get ready
}

void loop() {
  // Should be called for each loop.
  wsServer.listen();
  /* use for test*/
  // if( now() != prevDisplay){
  //      prevDisplay = now();
  //      Serial.println(now());
  //    } 

  delay(100);

}
