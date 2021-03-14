/**
 * waterweight
 *
 **/

#include <SPIFFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

int pressureAnalogPin = 32;
int pressureReading = 0;
float weight = 0;
int noWeight = 5;
int lightWeight = 50;
int mediumWeight = 200;

const char* ssid = "SSID";
const char* password = "PASSWORD";

AsyncWebServer server(80);

const int neoPixelDigitalPin = 13;
const int numberOfPixels = 1;

Adafruit_NeoPixel pixel(numberOfPixels,
						neoPixelDigitalPin,
						NEO_GRB + NEO_KHZ800);

void listFilesInDir(File dir, int numTabs = 1);

void initSPIFFS() {
	Serial.println(F("Inizializing FS..."));
	if (SPIFFS.begin()){
		Serial.println(F("SPIFFS mounted correctly."));
	}else{
		Serial.println(F("!An error occurred during SPIFFS mounting"));
	}

	unsigned int totalBytes = SPIFFS.totalBytes();
	unsigned int usedBytes = SPIFFS.usedBytes();

	Serial.println("===== File system info =====");

	Serial.print("Total space:      ");
	Serial.print(totalBytes);
	Serial.println("byte");

	Serial.print("Total space used: ");
	Serial.print(usedBytes);
	Serial.println("byte");

	Serial.println();

	File dir = SPIFFS.open("/");
	listFilesInDir(dir);
	
	Serial.println();
}

void initWiFi() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(500);
	}
	Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
}

void listFilesInDir(File dir, int numTabs) {
	while (true) {
		File entry =  dir.openNextFile();
		if (! entry) {
			// no more files in the folder
			break;
		}
		for (uint8_t i = 0; i < numTabs; i++) {
			Serial.print('\t');
		}
		Serial.print(entry.name());
		if (entry.isDirectory()) {
			Serial.println("/");
			listFilesInDir(entry, numTabs + 1);
		} else {
			// display zise for file, nothing for directory
			Serial.print("\t\t");
			Serial.println(entry.size(), DEC);
		}
		entry.close();
	}
}

float calculateWeight() {
	pressureReading = analogRead(pressureAnalogPin);
	weight = pressureReading * (5.0 / 1023.0);
	Serial.println(weight);
	return weight;
}

String getWeight() {
	StaticJsonDocument<16> doc;
	doc["weight"] = weight;
	String weightJSON;
	serializeJson(doc, weightJSON);
	return weightJSON;
}

String getSSID(){
	StaticJsonDocument<48> doc;
	doc["ssid"] = ssid;
	String ssidJSON;
	serializeJson(doc, ssidJSON);
	return ssidJSON;
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

	initSPIFFS();
	initWiFi();

	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
	server.on("/weight", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", getWeight().c_str());
	});
	server.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/plain", getSSID().c_str());
	});

	server.begin();
}

void loop(void){
	updateNeoPixel(calculateWeight());
	delay(10000);
}
