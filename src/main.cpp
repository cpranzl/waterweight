/**
 * waterweight
 * 
 **/
#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <MQTT.h>

int pressureAnalogPin = 0; // Pin where our pressure pad is located.
int pressureReading;
char pressureReadingChar[5];
int lastPressureReading = 0;
int noPressure = 5; // Threshold for no pressure on the pad
int lightPressure = 50; // Threshold for light pressure on the pad
int mediumPressure = 200; // Threshold for medium pressure on the pad

char ssid[8] = "network";
char pass[9] = "password";
int status = WL_IDLE_STATUS;
WiFiClient net;

int webServerPort = 80;
WiFiServer webServer(webServerPort);

int neoPixelDigitalPin = 6;
int numberOfPixels = 1;
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixel(numberOfPixels, neoPixelDigitalPin, NEO_GRB + NEO_KHZ800);

char mqttBroker[11] = "10.0.0.106";
int mqttBrokerPort = 1883;
MQTTClient mqttclient;

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

	// Print the MAC address of the AP
	byte bssid[6];
	WiFi.BSSID(bssid);
	Serial.print("BSSID: ");
	Serial.print(bssid[5], HEX);
	Serial.print(":");
	Serial.print(bssid[4], HEX);
	Serial.print(":");
	Serial.print(bssid[3], HEX);
	Serial.print(":");
	Serial.print(bssid[2], HEX);
	Serial.print(":");
	Serial.print(bssid[1], HEX);
	Serial.print(":");
	Serial.println(bssid[0], HEX);

	// Print the received signal strength
	long rssi = WiFi.RSSI();
	Serial.print("Signal strength (RSSI):");
	Serial.println(rssi);

	// Print the encryption type
	byte encryption = WiFi.encryptionType();
	Serial.print("Encryption Type:");
	Serial.println(encryption, HEX);
	Serial.println();
}

void setup(void) {
	// Start serial
	Serial.begin(9600);

	// Connect to WiFi
	while (status != WL_CONNECTED) {
		Serial.print("Connecting");
		status =  WiFi.begin(ssid, pass);
		delay(10000);
	}
	Serial.println("Connected");
	printCurrentNet();
	printWifiData();
	webServer.begin();
	Serial.println("Webserver started");
	
	// Start NeoPixel
	pixel.begin();
	pixel.show();

	// Connect to MQTTBroker
	mqttclient.begin(mqttBroker, mqttBrokerPort, net); 
}
 
void loop(void) {
	String fontColor = "style=\"color:black\">";
	// Read FSR and update NeoPixel
	pressureReading = analogRead(pressureAnalogPin);
	if (pressureReading != lastPressureReading){
		String str = String(pressureReading);
		str.toCharArray(pressureReadingChar,4);
		Serial.print("Pressure Pad Reading = ");
		Serial.println(pressureReading);
		mqttclient.publish("/appliances/coffeemachine", pressureReadingChar);
		float voltage = pressureReading * (5.0 / 1023.0);
		Serial.print("Voltage = ");
		Serial.println(voltage);
		if (pressureReading < noPressure){
			fontColor = "style=\"color:red\">";
			pixel.setPixelColor(1, 255, 0, 0);
			pixel.show();
		}
		if (pressureReading > noPressure && pressureReading < lightPressure){
			fontColor = "style=\"color:red\">";
			pixel.setPixelColor(1, 255, 128, 0);
			pixel.show();
		}
		if (pressureReading > lightPressure && pressureReading < mediumPressure){
			fontColor = "style=\"color:orange\">";	
			pixel.setPixelColor(1, 255, 255, 0);
			pixel.show();
		}
		if (pressureReading > mediumPressure){
			fontColor = "style=\"color:green\">";
			pixel.setPixelColor(1, 0, 255, 0);
			pixel.show();
		}
	}

	// Serve website to clients
	WiFiClient client = webServer.available();
	if (client){
		Serial.println("Client connected");
		boolean currentLineIsBlank = true;
		while (client.connected()){
			if (client.available()){
				char c = client.read();
				if (c == '\n' && currentLineIsBlank){
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Connection: close");
					client.println("Refresh: 5");
					client.println();
					client.println("<!DOCTYPE HTML>");
					client.println("<html>");
					client.println("<head>");
					client.println("<meta charset=\"utf-8\">");
					client.println("<title>WaterWeight</title>");
					client.println("</head>");
					client.println("<body>");
					client.println("<h1>WaterWeight</h1>");
					client.print("<p ");
					client.print(fontColor);
					client.print(">Pressure: ");
					client.print(pressureReading);
					client.println("</p>");
					client.println("</body>");
					client.println("</html>");
				}
				
				if (c == '\n'){
					currentLineIsBlank = true;
				}
				
				else if (c != '\r'){
					currentLineIsBlank = false;
				}
			}
		}
	}
	delay(1);
	
	client.stop();
	Serial.println("Client disconnected");
}

