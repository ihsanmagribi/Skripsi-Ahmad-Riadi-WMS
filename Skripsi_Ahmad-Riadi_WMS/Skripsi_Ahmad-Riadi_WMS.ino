#include "WiFi.h"
#include <HTTPClient.h>//Spread Sheet library HTTP
#include "DFRobot_ESP_PH.h"//PH Sensor
#include "EEPROM.h"
#include <OneWire.h>
#include <DallasTemperature.h> //suhu Sensor
#include <FirebaseESP32.h>//Firebase
#include <Wire.h>
#include <Adafruit_SSD1306.h> // Memanggil Library OLED SSD1306
#include <string.h>

#define FIREBASE_HOST "" //untuk database realtime
#define FIREBASE_AUTH "" // untuk token database secrets
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;
// WiFi credentials
const char* ssid = "";         // change SSID
const char* password = "";    // change password
//  Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "";    // change Gscript ID
 
// Constants for pH sensor
DFRobot_ESP_PH ph;
#define ESPADC 4096.0   // The ESP32 Analog Digital Conversion value
#define ESPVOLTAGE 3300 // The ESP32 voltage supply value
#define PH_PIN 35       // The ESP32 GPIO data pin number
const int oneWireBus = 5; // Constants for DS18B20 suhu sensor
OneWire oneWire(oneWireBus); // Setup oneWire instance to communicate with DS18B20
DallasTemperature sensors(&oneWire); // Setup DallasTemperature to access the sensor

#define SCREEN_WIDTH 128  // Lebar Oled dalam Pixel
#define SCREEN_HEIGHT 64  // Tinggi Oled dalam Pixel
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);




float PH, suhu;
float voltage;
float A, B;
String hasil;
String hasilKodular;

char kualitas1[] = "Sangat Buruk";
char kualitas2[] = "Buruk";
char kualitas3[] = "Baik";
char kualitas4[] = "Sangat Baik";

//======== nilai keanggotaan OUTPUT ============
float SBU = 0.2; // SBU = Sangat Buruk
float BU  = 0.4; // BU  = Buruk
float BA  = 0.6; // BA  = Baik
float SBA = 0.8; // SBA = Sangat Baik


float minr[26];
float Rule[26];


void setup()
{
    Serial.begin(115200);
    EEPROM.begin(32); // Needed to permit storage of calibration value in EEPROM
    ph.begin();
    sensors.begin();  // Initialize the DS18B20 sensor
    Serial.println("DS18B20 Sensor Suhu");
    // connect to WiFi
    Serial.println();
    Serial.print("Connecting to wifi: ");
    Serial.println(ssid);
    Serial.flush();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    // Konfigurasi Firebase
    firebaseConfig.host = FIREBASE_HOST;
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    //Display Oled
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // alamat I2C 0x3C untuk 128x28
    Serial.println(F("SSD1306 Gagal"));
    for (;;)
      ;  // mengulang terus, hingga bisa menghubungkan ke I2C Oled
  }
}



//======== Fungsi keanggotaan SUHU =========================================

float fuSSdingin(){
    
    if      (suhu >= 26)             {return 0;}
    
    else if (25 < suhu && suhu < 26) {return (26 - suhu) / (26 - 25);}
    
    else if (suhu <= 25)             {return 1;}
}


float fuSdingin(){

    if      (suhu <= 25 || suhu >= 28) {return 0;}
    
    else if (25 < suhu && suhu < 26)   {return (suhu - 25) / (26 - 25);}
    
    else if (26 <= suhu && suhu <= 27) {return 1;}
        
    else if (27 < suhu && suhu < 28)   {return (28 - suhu) / (28 - 27);}
}


float fuSnormal(){
    
    if (suhu <= 27 || suhu >= 31)    {return 0;}
    
    else if (27 < suhu && suhu < 29) {return (suhu - 27) / (29 - 27);}
    
    else if (suhu == 29)             {return 1;}
        
    else if (29 < suhu && suhu < 31) {return (31 - suhu) / (31 - 29);}
}


float fuSpanas() {
    
    if      (suhu <= 30 || suhu >= 33) {return 0;}
    
    else if (30 < suhu && suhu < 31)   {return (suhu - 30) / (31 - 30);}
    
    else if (31 <= suhu && suhu <= 32) {return 1;}
        
    else if (32 < suhu && suhu < 33)   {return (33 - suhu) / (33 - 32);}
}


float fuSSpanas() {
    
    if (suhu <= 32)                  {return 0;}
    
    else if (32 < suhu && suhu < 33 ){return (suhu - 32) / (33 - 32);}
    
    else if (suhu >= 33)             {return 1;}
}
//========================================================================

//======== Fungsi keanggotaan pH =========================================

float fuPSasam(){
    
    if      (PH >= 5.5)           {return 0;}
    
    else if (5 < PH && PH < 5.5)  {return (5.5 - PH) / (5.5 - 5);}
    
    else if (PH <= 5)             {return 1;}
}


float fuPasam(){

    if      (PH <= 5 || PH >= 6.5) {return 0;}
    
    else if (5 < PH && PH < 5.5)   {return (PH - 5) / (5.5 - 5);}
    
    else if (5.5 <= PH && PH <= 6) {return 1;}
        
    else if (6 < PH && PH < 6.5)   {return (6.5 - PH) / (6.5 - 6);}
}


float fuPnormal(){
    
    if (PH <= 6 || PH >= 8)    {return 0;}
    
    else if (6 < PH && PH < 7) {return (PH - 6) / (7 - 6);}
    
    else if (PH == 7)          {return 1;}
        
    else if (7 < PH && PH < 8) {return (8 - PH) / (8 - 7);}
}


float fuPbasa() {
    
    if      (PH <= 7.5 || PH >= 9) {return 0;}
    
    else if (7.5 < PH && PH < 8)   {return (PH - 7.5) / (8 - 7.5);}
    
    else if (8 <= PH && PH <= 8.5) {return 1;}
        
    else if (8.5 < PH && PH < 9)   {return (9 - PH) / (9 - 8.5);}
}


float fuPSbasa() {
    
    if (PH <= 8.5)                {return 0;}
    
    else if (8.5 < PH && PH < 9)  {return (PH - 8.5) / (9 - 8.5);}
    
    else if (PH >= 9)             {return 1;}
}

//========================================================================


float Min(float a, float b)
{
    if (a < b)
    {
        return a;
    }
    else if (b < a)
    {
        return b;
    }
    else
    {
        return a;
    }
}




void rule()
{
// R1 If pH Normal and Suhu Normal then kualitas Sangat_Baik 
    minr[1] = Min(fuPnormal(), fuSnormal());
    Rule[1] = SBA;
    
// R2 If pH Normal and Suhu Panas then kualitas Baik
    minr[2] = Min(fuPnormal(), fuSpanas());
    Rule[2] = BA;
    
// R3 If pH Normal and Suhu Dingin then kualitas Baik 
    minr[3] = Min(fuPnormal(), fuSdingin());
    Rule[3] = BA;
    
// R4 If pH Normal and Suhu Sangat Panas then kualitas Sangat_Buruk
    minr[4] = Min(fuPnormal(), fuSSpanas());
    Rule[4] = SBU;
    
// R5 If pH Normal and Suhu Sangat Dingin then kualitas Sangat_Buruk 
    minr[5] = Min(fuPnormal(), fuSSdingin());
    Rule[5] = SBU;
    
// R6 If pH Asam and Suhu Normal then kualitas Baik
    minr[6] = Min(fuPasam(), fuSnormal());
    Rule[6] = BA;
    
// R7 If pH Asam and Suhu Panas then kualitas Buruk
    minr[7] = Min(fuPasam(), fuSpanas());
    Rule[7] = BU;
    
// R8 If pH Asam and Suhu Dingin then kualitas Buruk
    minr[8] = Min(fuPasam(), fuSdingin());
    Rule[8] = BU;
    
// R9 If pH Asam and Suhu Sangat Panas then kualitas Sangat_Buruk
    minr[9] = Min(fuPasam(), fuSSpanas());
    Rule[9] = SBU;
    
// R10 If pH Asam and Suhu Sangat Dingin then kualitas Sangat_Buruk
    minr[10] = Min(fuPasam(), fuSSdingin());
    Rule[10] = SBU;
    
// R11 If pH Basa and Suhu Normal then kualitas Baik
    minr[11] = Min(fuPbasa(), fuSnormal());
    Rule[11] = BA;
    
// R12 If pH Basa and Suhu Panas then kualitas Buruk
    minr[12] = Min(fuPbasa(), fuSpanas());
    Rule[12] = BU;
    
// R13 If pH Basa and Suhu Dingin then kualitas Buruk
    minr[13] = Min(fuPbasa(), fuSdingin());
    Rule[13] = BU;
    
// R14 If pH Basa and Suhu Sangat Panas then kualitas Sangat_Buruk
    minr[14] = Min(fuPbasa(), fuSSpanas());
    Rule[14] = SBU;
    
// R15 If pH Basa and Suhu Sangat Dingin then kualitas Sangat_Buruk
    minr[15] = Min(fuPbasa(), fuSSdingin());
    Rule[15] = SBU;
    
// R16 If pH Sangat_Asam and Suhu Normal then kualitas Sangat_Buruk
    minr[16] = Min(fuPSasam(), fuSnormal());
    Rule[16] = SBU;
    
// R17 If pH Sangat_Asam and Suhu Panas then kualitas Sangat_Buruk 
    minr[17] = Min(fuPSasam(), fuSpanas());
    Rule[17] = SBU;
    
// R18 If pH Sangat_Asam and Suhu Dingin then kualitas Sangat_Buruk
    minr[18] = Min(fuPSasam(), fuSdingin());
    Rule[18] = SBU;
    
// R19 If pH Sangat_Asam and Suhu Sangat Panas then kualitas Sangat_Buruk
    minr[19] = Min(fuPSasam(), fuSSpanas());
    Rule[19] = SBU;
    
// R20 If pH Sangat_Asam and Suhu Sangat Dingin then kualitas Sangat_Buruk
    minr[20] = Min(fuPSasam(), fuSSdingin());
    Rule[20] = SBU;
    
// R21 If pH Sangat_Basa and Suhu Normal then kualitas Sangat_Buruk
    minr[21] = Min(fuPSbasa(), fuSnormal());
    Rule[21] = SBU;
    
// R22 If pH Sangat_Basa and Suhu Panas then kualitas Sangat_Buruk
    minr[22] = Min(fuPSbasa(), fuSpanas());
    Rule[22] = SBU;
    
// R23 If pH Sangat_Basa and Suhu Dingin then kualitas Sangat_Buruk
    minr[23] = Min(fuPSbasa(), fuSdingin());
    Rule[23] = SBU;
    
// R24 If pH Sangat_Basa and Suhu Sangat Panas then kualitas Sangat_Buruk
    minr[24] = Min(fuPSbasa(), fuSSpanas());
    Rule[24] = SBU;
    
// R25 If pH Sangat_Basa and Suhu Sangat Dingin then kualitas Sangat_Buruk
    minr[25] = Min(fuPSbasa(), fuSSdingin());
    Rule[25] = SBU;
    

}

float defuzzyfikasi()
{
    rule();
    A = 0;
    B = 0;

    for (int i = 1; i <= 25; i++)
    {
        // printf("Rule ke %d = %f\n", i, Rule[i]);
        // printf("Min ke %d = %f\n", i, minr[i]);
        A += Rule[i] * minr[i];
        B += minr[i];
    }
    // printf("Hasil A : %f\n", A);
    // printf("Hasil B : %f\n", B);
    return A / B;
}


void loop() {

  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U)  // Time interval: 1s
  {
    timepoint = millis();
    // Read the suhu from the DS18B20 sensor
    sensors.requestTemperatures();
    suhu = sensors.getTempCByIndex(0);
    // Voltage = rawPinValue / ESPADC * ESPVOLTAGE
    voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE;  // Read the voltage
    Serial.print("Voltage: ");
    Serial.println(voltage, 4);

    // Display suhu
    Serial.print("suhu: ");
    Serial.print(suhu, 1);
    Serial.println(" °C");

    // Convert voltage to pH with suhu compensation
    PH = ph.readPH(voltage, suhu);
    Serial.print("pH: ");
    Serial.println(PH, 4);
  }


  Serial.print("Nilai pH         : ");
  Serial.println(PH);
  Serial.print("Nilai Suhu       : ");
  Serial.println(suhu);
  Serial.println("===========================================");
  Serial.println("===========   [KEANGGOTAAN SUHU]   ========");

  Serial.print("Sangat dingin    : ");
  Serial.println(fuSSdingin());

  Serial.print("Dingin           : ");
  Serial.println(fuSdingin());

  Serial.print("Normal           : ");
  Serial.println(fuSnormal());

  Serial.print("Panas            : ");
  Serial.println(fuSpanas());

  Serial.print("Sangat panas     : ");
  Serial.println(fuSSpanas());

  Serial.println("============   [KEANGGOTAAN PH]   =========");

  Serial.print("Sangat asam      : ");
  Serial.println(fuPSasam());

  Serial.print("Asam             : ");
  Serial.println(fuPasam());

  Serial.print("Normal           : ");
  Serial.println(fuPnormal());

  Serial.print("Basa             : ");
  Serial.println(fuPbasa());

  Serial.print("Sangat basa      : ");
  Serial.println(fuPSbasa());

  Serial.println("===========================================");

  Serial.print("Deffuzzyfikasi   : ");
  Serial.println(defuzzyfikasi());

  Serial.print("Kualitas Air     : ");
  if (defuzzyfikasi()  < 0.30) {
    hasil = "Sangat Buruk";
    hasilKodular = "Sangat-Buruk";
  }

  else if (defuzzyfikasi() < 0.50 && defuzzyfikasi() >= 0.30) {
    hasil = "Buruk";
    hasilKodular = "Buruk";
  }

  else if (defuzzyfikasi() < 0.70 && defuzzyfikasi() >= 0.50) {
    hasil = "Baik";
    hasilKodular = "Baik";
  }

  else if (defuzzyfikasi() >= 0.70) {
    hasil = "Sangat Baik";
    hasilKodular = "Sangat-Baik";
  }
  Serial.println(hasil);
  Serial.println();Serial.println();Serial.println();
  delay(5000);
  display.display();
  display.clearDisplay();               //Membersihkan tampilan
  display.setTextSize(1);               //Ukuran tulisan
  display.setTextColor(SSD1306_WHITE);  //Warna Tulisan
  display.setCursor(0, 0);              // Koordinat awal tulisan (x,y) dimulai dari atas-kiri
  display.print("-Monitoring Kwt Air-");
  display.setCursor(0, 20);    // Koordinat awal tulisan (x,y) dimulai dari atas-kiri
  display.print("pH     : ");  //Menampilkan Tulisan RobotikIndonesia
  display.println(PH);
  display.setCursor(0, 32);    // Koordinat awal tulisan (x,y) dimulai dari atas-kiri
  display.print("Suhu   : ");  //Menampilkan nilai variabel dataInt
  display.println(suhu);
  display.setCursor(0, 44);  // Koordinat awal tulisan (x,y) dimulai dari atas-kiri
  display.print("Kwt air: ");
  display.println(hasil);
  display.display();                              //Mulai Menampilkan
                                                  //-------------------- Mengirimkan data suhu dan kelembaban ke Firebase ----------------------
  String nilaipH = String(PH, 2);                 // Membatasi nilai pH menjadi 2 desimal
  String nilaisuhu = String(suhu, 2);             // Membatasi nilai suhu menjadi 2 desimal
  float nilaidefuzifikasi = defuzzyfikasi();  //buat variable baru untuk nilai defuzifikasi


  if (Firebase.setString(firebaseData, "/Hasil_Pembacaan/pH", nilaipH)) {
    Serial.println("pH terkirim");
  } else {
    Serial.println("pH tidak terkirim");
    Serial.println("Karena: " + firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/Hasil_Pembacaan/Suhu", nilaisuhu)) {
    Serial.println("Suhu terkirim");
    Serial.println();
  } else {
    Serial.println("suhu tidak terkirim");
    Serial.println("Karena: " + firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/Hasil_Pembacaan/Defuzifikasi", hasilKodular)) {
    Serial.println("Hasil Defuzifikasi terkirim");
    Serial.println();
  } else {
    Serial.println("Hasil Defuzifikasi tidak terkirim");
    Serial.println("Karena: " + firebaseData.errorReason());
  }
  //-----------------------------------
  String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "pH=" + String(PH) + "&suhu=" + String(suhu) + "&hasil=" + String(hasil) + "&nilaidefuzifikasi=" + String(nilaidefuzifikasi);
  HTTPClient http;
  http.begin(urlFinal.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  //getting response from google sheet
  String payload;
  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("Payload: " + payload);
  }
  //---------------------------------------------------------------------
  http.end();
  delay(1000);
}
