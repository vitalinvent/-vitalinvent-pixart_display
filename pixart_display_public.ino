// This is how many color levels the display shows - the more the slower the update
//#define PxMATRIX_COLOR_DEPTH 4
// Defines the speed of the SPI bus (reducing this may help if you experience noisy images)
//#define PxMATRIX_SPI_FREQUENCY 20000000
// Creates a second buffer for backround drawing (doubles the required RAM)
//#define PxMATRIX_double_buffer true
#include <PxMatrix.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
// Pins for LED MATRIX
#ifdef ESP8266
  #include <Ticker.h>
  Ticker display_ticker;
  #define P_LAT 16
  #define P_A 5
  #define P_B 4
  #define P_C 15
  #define P_D 12
  #define P_E 0
  #define P_OE 2
#endif
#define matrix_width 64
#define matrix_height 32
// This defines the 'on' time of the display is us. The larger this number,
// the brighter the display. If too large the ESP will crash
uint8_t display_draw_time=10; //30-70 is usually fine
const char* ssid = "YOU_WIFI";
const char* password = "PASSWORD";

PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);
//PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C);
//PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);
// Some standard colors
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);
uint16_t myCOLORS[8]={myRED,myGREEN,myBLUE,myWHITE,myYELLOW,myCYAN,myMAGENTA,myBLACK};
uint8_t icon_index=0;
int max_pix = 6;
int timerPix = 1000;
uint16_t r_col[100]={ 0 };
uint16_t r_x[100]={ 0 };
uint16_t r_y[100]={ 0 };
uint16_t counter=0;
boolean filled=false;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  display.begin(16);
  display.clearDisplay();
  display.setCursor(2,0);
  display_update_enable(true);
  display.setBrightness(25);
}

void loop() {
  ArduinoOTA.handle();
  display.drawPixel(r_x[counter],r_y[counter],r_col[counter]);
  //if (filled){
    //if (counter+1<=max_pix) {
      display.drawPixel(r_x[counter+1],r_y[counter+1],0);
    //}
  //}
  if (counter>=max_pix){
    counter=0;
    filled=true;
    max_pix = random(5,50);
    display.clearDisplay();
    timerPix=10;
  }
  r_x[counter]=random(matrix_width);
  r_y[counter]=random(matrix_height);
  r_col[counter]=random(65535);
  
  delay(timerPix);
  counter++;
  timerPix = random(100,3000);
}

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  display.display(display_draw_time);
}
#endif

void display_update_enable(bool is_enable)
{

#ifdef ESP8266
  if (is_enable)
    display_ticker.attach(0.004, display_updater);
  else
    display_ticker.detach();
#endif
}

union single_double{
  uint8_t two[2];
  uint16_t one;
} this_single_double;
