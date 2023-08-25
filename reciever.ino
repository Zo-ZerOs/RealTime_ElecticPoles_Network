#include <SPI.h>
#include <LoRa.h>
#include <FileIO.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <string.h>
#define ss 2
#define rst 35
#define dio0 32
#define AppName "RealTimeWeb"
const char* ssid     = AppName;
const char* password = "12345678";
String One = "Loading";
String Two = "Loading";
String LoraData = "";



WebServer server(80);

void StartAP(){
 
  Serial.printf("\nConfiguring access point...\n");
 
  WiFi.softAP(ssid, password);
  Serial.printf("AP IP address: %s\n",WiFi.softAPIP().toString().c_str());
}

void handleRoot() {
 String s PROGMEM = R"=====(<!DOCTYPE html>
<html lang="en">
   <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <meta http-equiv="X-UA-Compatible" content="ie=edge">
      <link rel="stylesheet" href="style.css">
      <title>ESP32:Dashboard</title>
   </head>
   <body>
      <div id="container">
         <header class="header">
            <div>
               <h2>KVIS</h2>
            </div>
         </header>
         <main class="main">
            <div class="main-overview">
               <div class="overviewcard">
                  <div class="overviewcard__icon">1</div>
                  <div class="overviewcard__info">
                     <span id="1">N/A</span>
                  </div>
               </div>
               <div class="overviewcard">
                  <div class="overviewcard__icon">2</div>
                  <div class="overviewcard__info">
                     <span id="2">N/A</span>
                  </div>
               </div>
            </div>
         </main>
         <footer class="footer">
         <div class="footer__copyright">2021 </div>
         <div class="footer__signature">KVIS ESP32 WebServer</div>
         </footer>
      </div>
      <script>
      getData('info');
setInterval(function() {
getData('info');
}, 100);
function onoff(el) {
var request = new XMLHttpRequest();
request.open('POST', 'onoff', false);
request.send(el.checked);
if (request.response) alert(request.response);
else alert('UnSuccess!!');
}
function getData(uri) {
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
var jj = JSON.parse(this.responseText);
for (var i = 0; i < jj.length; i++) {
j = jj[i];
if (document.getElementById(j.name)) {
j.name === 'led'
? (document.getElementById(j.name).checked = j.val)
: (document.getElementById(j.name).innerHTML = j.val);
}
if (document.getElementById('label_' + j.name)) {
document.getElementById('label_' + j.name).innerHTML = j.name;
}
}
}
};
xhttp.open('GET', uri, true);
xhttp.send();
}
      </script>
   </body>
</html>
</html>)=====";
 
server.send(200, "text/html", s);
}

void HandleCss() {
  String css PROGMEM = R"=====(html,
body {
    margin: 0;
    max-height: 100%;
    color: #000000;
     font-family: 'Open Sans', Helvetica, sans-serif;
    max-width:100vw;
}
#container {
    display: flex;
    flex-direction: column;
}
.header {
    align-items: center;
    justify-content: space-between;
    padding-top: 10px 15px;
    background-color: #624185;
    color:lightgreen;
/*     background-color: #648ca6; */
    width: 100%;
}
.header h2 {
    margin-left: 15px;
}
.main {
  background-color: #fefece;
/*     background-color: #8fd4d9; */
    padding-top: 20px;
    overflow: auto;
    flex: 1;
    min-width: 100%;
    max-height: 100vh;
   padding-bottom:397px;
}
.main-header {
    display: flex;
    justify-content: space-between;
    margin: 20px;
    padding: 20px;
    height: 50px;
    background-color: #e3e4e6;
    color: slategray;
}
.main-overview {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(265px, 1fr));
    grid-auto-rows: 94px;
    grid-gap: 20px;
    margin: 20px;
}
.overviewcard {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 20px;
/*     background-color: #d3d3; */
  background-color: #ffa345;
}
.main-cards {
    column-count: 1;
    column-gap: 20px;
    margin: 20px;
}
.card {
    display: flex;
    flex-direction: column;
    align-items: center;
    width: 100%;
    background-color: #82bef6;
/*   background-color: #ffa345; */
    margin-bottom: 20px;
    -webkit-column-break-inside: avoid;
    padding: 24px;
    box-sizing: border-box;
}
.card: first-child {
    height: 485px;
}
.card: nth-child(2) {
    height: 200px;
}
.card: nth-child(3) {
    height: 265px;
}
.footer {
    
    grid-area: footer;
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 10px;
   
    background-color: #624185;
    color: white ;
/*     background-color: #f2ed73; */
  )=====";
  server.send(200, "text/css", css);
  }

void HandleInfo () {
  String data = "[";
  data += "{\"name\": \"1\",\"val\":\"" + One + "\"},";
  data += "{\"name\": \"2\",\"val\":\"" + Two + "\"}";
  data += "]";
  
  server.send(200, "application/json", data);
} 

void StartWeb(){
  server.on("/", handleRoot);
  server.on("/style.css", HandleCss);
  //server.on("/index.js", HandleJs);
  server.on("/info", HandleInfo);
  Serial.printf("HTTP server started at: \n\t - http://%s\n\t - http://%s.local\n",WiFi.softAPIP().toString().c_str(),WiFi.softAPgetHostname());
  server.begin();
  
  Serial.printf("HTTP server started at: http://%s\n", WiFi.softAPIP().toString().c_str());
}

void StartDNS() {
  if (!MDNS.begin(ssid)) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");
}

void setup(){
  Serial.begin(9600);
  StartAP();
  StartWeb();
  StartDNS();
  pinMode(12, OUTPUT);
  while (!Serial);

  Serial.println("LoRa Receiver");
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}
void loop(){
  
//  Serial.print(FindOne());
//  Serial.print("____\n");
//  Serial.print(FindTwo());
//  Serial.print("____\n");
  server.handleClient();
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(300);                       // wait for a second
    digitalWrite(12, LOW);    // turn the LED off by making the voltage LOW
    delay(300);                       // wait for a second
    // read packet

    LoraData = "";
    while (LoRa.available()) {
      
      LoraData += (char)LoRa.read();
//      Serial.println(LoraData);
      if(LoraData.endsWith("!")){
        pinMode(14, OUTPUT);
        digitalWrite(14, HIGH);
        delay(100);
        digitalWrite(14,LOW);
        delay(100);
        digitalWrite(14, HIGH);
        delay(100);
        digitalWrite(14,LOW);
        delay(100);
        digitalWrite(14, HIGH);
        delay(500);
        digitalWrite(14,LOW);
      }
      if(LoraData.startsWith("1")){
          One = LoraData;
          One.remove(0, 3);
      }
      else if (LoraData.startsWith("2")){
          Two = LoraData;
          Two.remove(0, 3);
      }
    }
      
    // print RSSI of packet
    //Serial.print("' with RSSI ");
    Serial.print(LoraData);
    Serial.println();
    LoraData = "";
  }
}
