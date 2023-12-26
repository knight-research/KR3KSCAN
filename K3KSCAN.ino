/*
    Name:       K3KSCAN.ino
    Created:	26.12.2023 21:05:00
    Author:     knight-research
*/
bool REGION = true; //I AM ONLY HERE TO SHOW AND HIDE CODE
bool debug = false; //PRINT INFORMATIONS TO CONSOLE
String version = "V2.0.0";
String last_change = "2023-12-26-2144";

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include "html_pages.h"

#define PIN1 D1
#define PIN2 D4
#define NUMPIXELS 60                // Number of LED's in your strip
#define NUM_LEDS 60                // Number of LED's in your strip

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUMPIXELS, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUMPIXELS, PIN2, NEO_GRB + NEO_KHZ800);

MDNSResponder mdns;

ESP8266WebServer server(80);

const int delayval = 500;
String rgb_now = "#0000ff";    //global rgb state values for use in various html pages
long red_int = 0;
long green_int = 0;
long blue_int = 0;
int brightness = 15;
int mode_flag = 2;

void handleIndex();
void handleNotFound();
void handleSetBrightness();
void handleBrightness();
void handleSelectMode();
void handle_mode1();
void handle_mode2();
void handle_mode3();
void handle_mode4();
void handle_mode5();
void handle_mode6();
void handle_mode7();
void handle_mode8();
void turn_off_all();

void setup(void) {
    Serial.begin(115200);
    Serial.println("");
    EEPROM.begin(5);                           // set up EEPROM storage space for config values
    Serial.println("");
    Serial.print("Setting soft-AP ... ");
    Serial.println("open 192.168.4.1 on your browser");
    boolean result = WiFi.softAP("K3000_V2020-08_004", "Comtron1234");
    if (result == true)
    {
        Serial.println("Ready");
    }
    else
    {
        Serial.println("Failed!");
    }

    if (mdns.begin("esp8266", WiFi.localIP())) {
        Serial.println("MDNS responder started");
    }

    Serial.println("HTTP server started");

    server.on("/", handleIndex);
    server.onNotFound(handleNotFound);

    server.on("/switch_on", handleSwitchOn);
    server.on("/set_brightness", handleSetBrightness);
    server.on("/set_bright_val", handleBrightness);
    server.on("/select_mode", handleSelectMode);
    server.on("/set_mode1", handle_mode1);
    server.on("/set_mode2", handle_mode2);
    server.on("/set_mode3", handle_mode3);
    server.on("/set_mode4", handle_mode4);
    server.on("/set_mode5", handle_mode5);
    server.on("/set_mode6", handle_mode6);
    server.on("/set_mode7", handle_mode7);
    server.on("/set_mode8", handle_mode8);

    //EEPROM Memory//
    //Mem Location ||--0--||--1--||--2--||--3--||--4--||--5--||--6--||
    //                red   green  blue   bright  mode

    red_int = EEPROM.read(0);       //restore colour to last used value. Ensures RGB lamp is same colour as when last switched off
    green_int = EEPROM.read(1);
    blue_int = EEPROM.read(2);
    brightness = EEPROM.read(3);
    mode_flag = EEPROM.read(4);

    server.begin();
    strip1.begin();
    strip2.begin();
    colorBothSideNegative(strip1.Color(255, 0, 0), 10);
}


void loop(void) {
    mdns.update();
    server.handleClient();
    return;
}

void handleIndex() {
    Serial.println("Request for index page received");
    server.send(200, "text/html", page_contents);
}

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void handleSwitchOn() {
    mode_flag = EEPROM.read(4);                       // start-up in last saved mode
    delay(100);
    switch (mode_flag) {
    case 1: handle_mode1();
        break;
    case 2: handle_mode2();
        break;
    case 3: handle_mode3();
        break;
    case 4: handle_mode4();
        break;
    case 5: handle_mode5();
        break;
    case 6: handle_mode6();
        break;
    case 7: handle_mode7();
        break;
    case 8: handle_mode8();
        break;
    default:
        handle_mode3();                          //Default to fixed colour should the EEProm become corrupted
        break;
    }
    server.send(200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
}

void handleSetBrightness() {
    server.send(200, "text/html", bright_set);
}

void handleSelectMode() {
    server.send(200, "text/html", mode_page);
    //  Serial.println ( "Mode select page" );
}

void handleBrightness() {
    String message = server.arg(0);                //get the 1st argument from the url which is the brightness level set by the slider
    String bright_val = message.substring(0, 3);   //extract the brightness value from the end of the argument in the URL
    brightness = bright_val.toInt();
    EEPROM.write(3, brightness);                   //write the brightness value to EEPROM to be restored on start-up
    EEPROM.commit();

    String java_redirect = "<SCRIPT language='JavaScript'>window.location='/set_brightness?";
    java_redirect += brightness;                                              //send brightness value in URL to update the slider control
    java_redirect += "';</SCRIPT>";
    server.send(200, "text/html", java_redirect);                                  // all done! - take user back to the brightness selection page
}

void turn_off_all() {
    mode_flag = 99;                                     //go to non-existent mode and turn off all pixels
    for (int i = 0; i < NUMPIXELS; i++) {                                                                                                      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip1.setPixelColor(i, strip1.Color(0, 0, 0));                                                                                             // Turn off led strip
        strip1.show();
    }                                                                                                                            // This sends the updated pixel color to the hardware.
}

void colorBothSideNegative(uint32_t c, uint8_t wait) {
    for (uint16_t i = 0; i < (strip1.numPixels() / 2); i++) {
        strip1.setPixelColor((strip1.numPixels() / 2 - 1) - i, c);
        strip2.setPixelColor((strip2.numPixels() / 2 - 1) - i, c);
        strip1.setPixelColor((((strip1.numPixels() / 2) + i)), c);
        strip2.setPixelColor((((strip2.numPixels() / 2) + i)), c);
        strip1.show();
        strip2.show();
    }
    delay(200);
    pulsedown(1);
}

void pulsedown(uint8_t wait) {
    for (int r = 255; r >= 0; r--) { // Ramp down from 255 to 0
        strip1.fill(strip1.Color(r, 0, 0));
        strip2.fill(strip1.Color(r, 0, 0));
        strip1.show();
        strip2.show();
    }
    handleSwitchOn();
}

void handle_mode1() {
    mode_flag = 1;
    EEPROM.write(4, mode_flag);                       //write mode to EEProm so can be restored on start-up
    EEPROM.commit();
    server.send(200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
    turn_off_all();
    mode_flag = 1;
    loop();
}

void handle_mode2() {
    mode_flag = 2;
    EEPROM.write(4, mode_flag);                        //write mode to EEProm so can be restored on start-up
    EEPROM.commit();
    server.send(200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
    uint16_t i, j, k;
    int wait = 2;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266

    while (mode_flag == 2) {                           // do this indefenitely or until mode changes
        int fade = 8;
        int r = 255;
        int g = 70;
        int b = 0;
        int faktor = 20 / (fade * 2);
        int r2, g2, b2;
        int width = 60;
        //WHILE SCHLEIFFE---------------------------------------------------------------------------------------------
        for (int i = 0; i < strip1.numPixels() + width + fade; i++) { // gesamten strip1 durchlaufen VORWÄRTS
            //EFFEKT VON INNEN NACH AUSSEN
            for (int j = 0; j < width; j++) { // hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i - j, r, g, b);
                strip2.setPixelColor(i - j, r, g, b);
                for (int f = fade; f > 0; f--) { // hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i - width - fade + f), r2, g2, b2);
                    strip2.setPixelColor((i - width - fade + f), r2, g2, b2);
                }
                strip1.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
                strip2.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }

        for (int i = strip1.numPixels(); i > 0 - width - fade - 2; i--) { // gesamten strip1 durchlaufen RÜCKWÄRTS (rw)
            for (int j = width; j > 0; j--) { //rw: hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i + j, r, g, b);
                strip2.setPixelColor(i + j, r, g, b);

                for (int f = fade; f > 0; f--) { //rw: hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                    strip2.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                }
                strip1.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
                strip2.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }
        //WHILE SCHLEIFFE-END-----------------------------------------------------------------------------------------
    } return;
}

void handle_mode3() {
    mode_flag = 3;
    EEPROM.write(4, mode_flag);                       //write mode to EEProm so can be restored on start-up
    EEPROM.commit();
    server.send(200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
    uint16_t i, j, k;
    int wait = 2;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266

    while (mode_flag == 3) {                           // do this indefenitely or until mode changes
        int fade = 10;
        int r = 255;
        int g = 0;
        int b = 0;
        int faktor = 20 / (fade * 2);
        int r2, g2, b2;
        int width = 60;
        //WHILE SCHLEIFFE---------------------------------------------------------------------------------------------
        for (int i = 0; i < strip1.numPixels() + width + fade; i++) { // gesamten strip1 durchlaufen VORWÄRTS
            //EFFEKT VON INNEN NACH AUSSEN
            for (int j = 0; j < width; j++) { // hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i - j, r, g, b);
                strip2.setPixelColor(i - j, r, g, b);
                for (int f = fade; f > 0; f--) { // hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i - width - fade + f), r2, g2, b2);
                    strip2.setPixelColor((i - width - fade + f), r2, g2, b2);
                }
                strip1.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
                strip2.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }

        for (int i = strip1.numPixels(); i > 0 - width - fade - 2; i--) { // gesamten strip1 durchlaufen RÜCKWÄRTS (rw)
            for (int j = width; j > 0; j--) { //rw: hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i + j, r, g, b);
                strip2.setPixelColor(i + j, r, g, b);

                for (int f = fade; f > 0; f--) { //rw: hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                    strip2.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                }
                strip1.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
                strip2.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }
        //WHILE SCHLEIFFE-END-----------------------------------------------------------------------------------------
    } return;
}

void handle_mode4() {
    mode_flag = 4;
    EEPROM.write(4, mode_flag);                       //write mode to EEProm so can be restored on start-up
    EEPROM.commit();
    server.send(200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
    uint16_t i, j, k;
    int wait = 2;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266

    while (mode_flag == 4) {                           // do this indefenitely or until mode changes
        int fade = 10;
        int r = 0;
        int g = 0;
        int b = 255;
        int faktor = 20 / (fade * 2);
        int r2, g2, b2;
        int width = 60;
        //WHILE SCHLEIFFE---------------------------------------------------------------------------------------------
        for (int i = 0; i < strip1.numPixels() + width + fade; i++) { // gesamten strip1 durchlaufen VORWÄRTS
            //EFFEKT VON INNEN NACH AUSSEN
            for (int j = 0; j < width; j++) { // hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i - j, r, g, b);
                strip2.setPixelColor(i - j, r, g, b);
                for (int f = fade; f > 0; f--) { // hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i - width - fade + f), r2, g2, b2);
                    strip2.setPixelColor((i - width - fade + f), r2, g2, b2);
                }
                strip1.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
                strip2.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }

        for (int i = strip1.numPixels(); i > 0 - width - fade - 2; i--) { // gesamten strip1 durchlaufen RÜCKWÄRTS (rw)
            for (int j = width; j > 0; j--) { //rw: hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i + j, r, g, b);
                strip2.setPixelColor(i + j, r, g, b);

                for (int f = fade; f > 0; f--) { //rw: hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                    strip2.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                }
                strip1.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
                strip2.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }
        //WHILE SCHLEIFFE-END-----------------------------------------------------------------------------------------
    } return;
}

void handle_mode5() {
    mode_flag = 5;
    EEPROM.write(4, mode_flag);                       //write mode to EEProm so can be restored on start-up
    EEPROM.commit();
    server.send(200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
    uint16_t i, j, k;
    int wait = 2;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266

    while (mode_flag == 5) {                           // do this indefenitely or until mode changes
        int fade = 10;
        int r = 0;
        int g = 255;
        int b = 0;
        int faktor = 20 / (fade * 2);
        int r2, g2, b2;
        int width = 60;
        //WHILE SCHLEIFFE---------------------------------------------------------------------------------------------
        for (int i = 0; i < strip1.numPixels() + width + fade; i++) { // gesamten strip1 durchlaufen VORWÄRTS
            //EFFEKT VON INNEN NACH AUSSEN
            for (int j = 0; j < width; j++) { // hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i - j, r, g, b);
                strip2.setPixelColor(i - j, r, g, b);
                for (int f = fade; f > 0; f--) { // hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i - width - fade + f), r2, g2, b2);
                    strip2.setPixelColor((i - width - fade + f), r2, g2, b2);
                }
                strip1.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
                strip2.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }

        for (int i = strip1.numPixels(); i > 0 - width - fade - 2; i--) { // gesamten strip1 durchlaufen RÜCKWÄRTS (rw)
            for (int j = width; j > 0; j--) { //rw: hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i + j, r, g, b);
                strip2.setPixelColor(i + j, r, g, b);

                for (int f = fade; f > 0; f--) { //rw: hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                    strip2.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                }
                strip1.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
                strip2.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }
        //WHILE SCHLEIFFE-END-----------------------------------------------------------------------------------------
    } return;
}

void handle_mode6() {
    mode_flag = 6;
    EEPROM.write(4, mode_flag);                       //write mode to EEProm so can be restored on start-up
    EEPROM.commit();
    server.send(200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
    uint16_t i, j, k;
    int wait = 2;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266

    while (mode_flag == 6) {                           // do this indefenitely or until mode changes
        int fade = 10;
        int r = 0;
        int g = 255;
        int b = 255;
        int faktor = 20 / (fade * 2);
        int r2, g2, b2;
        int width = 60;
        //WHILE SCHLEIFFE---------------------------------------------------------------------------------------------
        for (int i = 0; i < strip1.numPixels() + width + fade; i++) { // gesamten strip1 durchlaufen VORWÄRTS
            //EFFEKT VON INNEN NACH AUSSEN
            for (int j = 0; j < width; j++) { // hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i - j, r, g, b);
                strip2.setPixelColor(i - j, r, g, b);
                for (int f = fade; f > 0; f--) { // hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i - width - fade + f), r2, g2, b2);
                    strip2.setPixelColor((i - width - fade + f), r2, g2, b2);
                }
                strip1.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
                strip2.setPixelColor(i - width - fade, 0, 0, 0); // löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }

        for (int i = strip1.numPixels(); i > 0 - width - fade - 2; i--) { // gesamten strip1 durchlaufen RÜCKWÄRTS (rw)
            for (int j = width; j > 0; j--) { //rw: hier werden die Pixel (ohne den fade) generiert
                strip1.setPixelColor(i + j, r, g, b);
                strip2.setPixelColor(i + j, r, g, b);

                for (int f = fade; f > 0; f--) { //rw: hier wird das fade generiert und an das Band angehängt
                    r2 = (r / 20) * (f * faktor);
                    g2 = (g / 20) * (f * faktor);
                    b2 = (b / 20) * (f * faktor);
                    strip1.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                    strip2.setPixelColor((i + width + fade - f + 1), r2, g2, b2);
                }
                strip1.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
                strip2.setPixelColor(i + width + fade + 1, 0, 0, 0); //rw löscht die Pixel nach dem Fade
            }
            strip1.show();
            strip2.show();
            //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
            for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
                delay(wait);
                loop();
            }
        }
        //WHILE SCHLEIFFE-END-----------------------------------------------------------------------------------------
    } return;
}

void handle_mode7() {
    mode_flag = 7;
    EEPROM.write(4, mode_flag);                       //write mode to EEProm so can be restored on start-up
    EEPROM.commit();
    server.send(200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
    uint16_t i, j, k;
    int wait = 2;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266

    while (mode_flag == 7) {                           // do this indefenitely or until mode changes
        static byte heat[NUM_LEDS];
        int cooldown;
        int Cooling = 55;
        int Sparking = 120;
        // Step 1.  Cool down every cell a little
        for (int i = 0; i < NUM_LEDS; i++) {
            cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

            if (cooldown > heat[i]) {
                heat[i] = 0;
            }
            else {
                heat[i] = heat[i] - cooldown;
            }
        }

        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (int k = NUM_LEDS - 1; k >= 2; k--) {
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
        }

        // Step 3.  Randomly ignite new 'sparks' near the bottom
        if (random(255) < Sparking) {
            int y = random(7);
            //heat[y] = heat[y] + random(160, 255);
            heat[y] = random(160, 255);
        }

        // Step 4.  Convert heat to LED colors
        for (int j = 0; j < NUM_LEDS; j++) {
            strip1.setPixelColor(j, heat[j], 0, 0);
            strip2.setPixelColor(j, heat[j], 0, 0);
        }

        strip1.show();
        strip2.show();
        delay(wait);
        //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
        for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
            delay(wait);
            loop();
        }
        //WHILE SCHLEIFFE-END-----------------------------------------------------------------------------------------
    } return;
}

void handle_mode8() {
    mode_flag = 8;
    EEPROM.write(4, mode_flag);                       //write mode to EEProm so can be restored on start-up
    EEPROM.commit();
    server.send(200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
    uint16_t i, j, k;
    int wait = 2;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266

    while (mode_flag == 8) {                           // do this indefenitely or until mode changes
        static byte heat[NUM_LEDS];
        int cooldown;
        int Cooling = 55;
        int Sparking = 120;
        // Step 1.  Cool down every cell a little
        for (int i = 0; i < NUM_LEDS; i++) {
            cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

            if (cooldown > heat[i]) {
                heat[i] = 0;
            }
            else {
                heat[i] = heat[i] - cooldown;
            }
        }

        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (int k = NUM_LEDS - 1; k >= 2; k--) {
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
        }

        // Step 3.  Randomly ignite new 'sparks' near the bottom
        if (random(255) < Sparking) {
            int y = random(7);
            //heat[y] = heat[y] + random(160, 255);
            heat[y] = random(160, 255);
        }

        // Step 4.  Convert heat to LED colors
        for (int j = 0; j < NUM_LEDS; j++) {
            strip1.setPixelColor(j, heat[j], 0, 0);
            strip2.setPixelColor(j, heat[j], 0, 0);
        }

        strip1.show();
        strip2.show();
        delay(wait);
        //HIER DIE GESCHWINDIGKEIT EINSTELLEN----------------------------------------------------------------------
        for (k = 0; k < brightness; k++) {                     // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
            delay(wait);
            loop();
        }
        //WHILE SCHLEIFFE-END-----------------------------------------------------------------------------------------
    } return;
}


// used by rainbowCycle and theaterChaseRainbow
byte* Wheel(byte WheelPos) {
    static byte c[3];

    if (WheelPos < 85) {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    }
    else if (WheelPos < 170) {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    }
    else {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }

    return c;
}
