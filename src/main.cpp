/**
 * waterweight
 * 
 **/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>

int pressureAnalogPin = 0; // Pin where our pressure pad is located.
int pressureReading;
char pressureReadingChar[5];
int lastPressureReading = 0;
int noPressure = 5; // Threshold for no pressure on the pad
int lightPressure = 50; // Threshold for light pressure on the pad
int mediumPressure = 200; // Threshold for medium pressure on the pad

const char* ssid = "ssid";
const char* pass = "password";

ESP8266WebServer webServer(80);

const int neoPixelDigitalPin = 13;
const int numberOfPixels = 1;
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixel(numberOfPixels, neoPixelDigitalPin, NEO_GRB + NEO_KHZ800);

String fontColor = "style=\"color:black\"";

void handleRoot() {
	char temp[400];
	int sec = millis() / 1000;
    int min = sec / 60;
    int hr = min / 60;
	
	snprintf(temp, 400,
    "<html>\
     <head>\
       <meta http-equiv='refresh' content='5'/>\
       <title>WaterWeight</title>\
       <style>\
         body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
       </style>\
       </head>\
       <body>\
         <h1>WaterWeight</h1>\
         <p>Uptime: %02d:%02d:%02d</p>\
		 <p>Pressure: %04d</p>\
       </body>\
     </html>",
    hr, min % 60, sec % 60, pressureReading);
    webServer.send(200, "text/html", temp);
}

void handleNotFound(){
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += webServer.uri();
	message += "\nMethod: ";
	message += (webServer.method() == HTTP_GET)?"GET":"POST";
	message += "\nArguments: ";
	message += webServer.args();
	message += "\n";
	for (uint8_t i = 0; i < webServer.args(); i++){
		message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
	}
	webServer.send(404, "text/plain", message);
}

void printWifiData() {
	// Print your IP address
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
	Serial.println(ip);

	// Print your MAC address
	byte mac[6];
	WiFi.macAddress(mac);
	Serial.print("MAC address: ");
	Serial.print(mac[5], HEX);
	Serial.print(":");
	Serial.print(mac[4], HEX);
	Serial.print(":");
	Serial.print(mac[3], HEX);
	Serial.print(":");
	Serial.print(mac[2], HEX);
	Serial.print(":");
	Serial.print(mac[1], HEX);
	Serial.print(":");
	Serial.println(mac[0], HEX);
}

void printCurrentNet() {
	// Print the SSID of the network
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// Print the received signal strength
	long rssi = WiFi.RSSI();
	Serial.print("Signal strength (RSSI):");
	Serial.println(rssi);
}

void setup(void) {
	// Start serial
	Serial.begin(115200);

	// Connect to WiFi
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, pass);
	Serial.print(".");
	
	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("Connected");
	printCurrentNet();
	printWifiData();

	if (MDNS.begin("esp8266")) {
    	Serial.println("MDNS responder started");
  		}

  	webServer.on("/", handleRoot);

  	webServer.on("/inline", [](){
    	webServer.send(200, "text/plain", "this works as well");
  	});

  	webServer.onNotFound(handleNotFound);
	webServer.begin();
	Serial.println("Webserver started");
	
	// Start NeoPixel
	pixel.begin();
	pixel.show();
}
 
void loop(void) {
	webServer.handleClient();
	pressureReading = analogRead(pressureAnalogPin);
	if (pressureReading != lastPressureReading){
		String str = String(pressureReading);
		str.toCharArray(pressureReadingChar,4);
		Serial.print("Pressure Pad Reading = ");
		Serial.println(pressureReading);
		float voltage = pressureReading * (5.0 / 1023.0);
		Serial.print("Voltage = ");
		Serial.println(voltage);
		if (pressureReading < noPressure){
			fontColor = "style=\"color:red\"";
			pixel.setPixelColor(1, 255, 0, 0);
			pixel.show();
		}
		if (pressureReading > noPressure && pressureReading < lightPressure){
			fontColor = "style=\"color:red\"";
			pixel.setPixelColor(1, 255, 128, 0);
			pixel.show();
		}
		if (pressureReading > lightPressure && pressureReading < mediumPressure){
			fontColor = "style=\"color:orange\"";	
			pixel.setPixelColor(1, 255, 255, 0);
			pixel.show();
		}
		if (pressureReading > mediumPressure){
			fontColor = "style=\"color:green\"";
			pixel.setPixelColor(1, 0, 255, 0);
			pixel.show();
		}
	}
	delay(1000);
}
