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
char pressureReadingChar[5];
int lastPressureReading = 0;
int noPressure = 5;
int lightPressure = 50;
int mediumPressure = 200;

const char* ssid = "SSID";
const char* password = "PASSWORD";

AsyncWebServer server(80);

const char* PARAM_MESSAGE = "message";

const int neoPixelDigitalPin = 13;
const int numberOfPixels = 1;

Adafruit_NeoPixel pixel(numberOfPixels,
                        neoPixelDigitalPin,
                        NEO_GRB + NEO_KHZ800);

void handleNotFound(AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Not found");
}

void setup(void){
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED){
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        char content[400];
        int sec = millis() / 1000;
        int min = sec / 60;
        int hr = min / 60;
        snprintf(content, 400,
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
        request->send(200, "text/plain", content);
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(handleNotFound);
    server.begin();

    pixel.begin();
}

void loop(void){
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
            pixel.setPixelColor(1, 255, 0, 0);
            pixel.show();
        }
        if (pressureReading > noPressure && pressureReading < lightPressure){
            pixel.setPixelColor(1, 255, 128, 0);
            pixel.show();
        }
        if (pressureReading > lightPressure && pressureReading < mediumPressure){
            pixel.setPixelColor(1, 255, 255, 0);
            pixel.show();
        }
        if (pressureReading > mediumPressure){
            pixel.setPixelColor(1, 0, 255, 0);
            pixel.show();
        }
    }
    delay(1000);
}

