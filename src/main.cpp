/**
 * waterweight
 *
 **/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>

int pressureAnalogPin = 0;
int pressureReading = 0;
int noWeight = 5;
int lightWeight = 50;
int mediumweight = 200;

const char* ssid = "SSID";
const char* password = "PASSWORD";

AsyncWebServer server(80);

const int neoPixelDigitalPin = 13;
const int numberOfPixels = 1;

void initNeoPixel() {
	Adafruit_NeoPixel pixel(numberOfPixels,
							neoPixelDigitalPin,
							NEO_GRB + NEO_KHZ800);
}

void initSPIFFS() {
	if (!SPIFFS.begin()) {
		Serial.println("Cannot mount SPIFFS volume...");
	}
}

void initWiFi() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASS);
	Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(500);
	}
	Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
}

void initWebServer() {
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/index.html");
	});
	server.on("/weight", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", getWeight(calculateWeight(analogRead(pressureAnalogPin))).c_str());
	});
	server.begin();
}

float calculateWeight(int pressureReading) {
	float weight = pressureReading * (5.0 / 1023.0)
	return weight;
}

String getWeight(float weight) {
	return String(weight)
}

void updateNeoPixel(float weightReading){
	if (weightReading < noWeight){
		pixel.setPixelColor(1, 255, 0, 0);
		pixel.show();
	}
	if (weightReading > noWeight && weightReading < lightWeight){
		pixel.setPixelColor(1, 255, 128, 0);
		pixel.show();
	}
	if (weightReading > lightWeight && weightReading < mediumWeight){
		pixel.setPixelColor(1, 255, 255, 0);
		pixel.show();
	}
	if (weightReading > mediumWeight){
		pixel.setPixelColor(1, 0, 255, 0);
		pixel.show();
	}
}

void setup(void){
	Serial.begin(115200); delay(500);
	initNeoPixel();
	initSPIFFS();
	initWiFi();
	initWebServer();
}

void loop(void){
	updateNeoPixel(calculateWeight(analogRead(pressureAnalogPin)));
	delay(10000);
}
