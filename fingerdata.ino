
#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Bu RIEKE";      // Nama SSID WiFi Anda
const char* password = "gulamanis";  // Kata sandi WiFi Anda
const char* serverAddress = "absensi.mtsshifa.com";
const int serverPort = 80;
const char* apiEndpoint = "/api/status/data";
const char* serverjari = "http://absensi.mtsshifa.com/api/status/response";
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 adalah alamat I2C default untuk modul LCD I2C

// Buffer to store HTTP response
const int bufferSize = 4096;
char responseBuffer[bufferSize];

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(D3, D4);
#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t enrollFingerprint();
uint8_t getFingerprintID();

void setup() {
  Serial.begin(9600);
  pinMode(D7,OUTPUT);
  while (!Serial); // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
//    while (1) {
//      delay(1);
//    }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.println("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
  }
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Koneksi WiFi berhasil");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());

   // Inisialisasi koneksi I2C
  Wire.begin();
  
  // Inisialisasi LCD
  lcd.begin(16, 2);  // Mengatur ukuran LCD (16 kolom dan 2 baris)
   lcd.backlight();
  // Menampilkan pesan awal pada LCD
}

void loop() {
  digitalWrite(D7,LOW);
  lcd.setCursor(0, 0);
  lcd.print("Selamat Datang");
  lcd.setCursor(0, 1);
  lcd.print("SMP 1 Margomulyo");
  Serial.println("Choose an option:");
  Serial.println("1. Enroll fingerprint");
  Serial.println("2. Verify fingerprint");

//  while (!Serial.available()) {
//    // Menunggu hingga data tersedia di Serial Monitor
//  }

//get data api
    char nilai='0';
      int id ;
            String namaSiswa ;
            String alamat;
            String noTelpOrtu;
            String kelas;
            String sidikJari ;
            String createdAt ;
            String updatedAt ;
            String status ;
if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = "http://absensi.mtsshifa.com/api/status/data"; // Ganti URL dengan URL API yang diinginkan
    if (http.begin(client, url)) {
      int httpCode = http.GET();

      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
//          Serial.println(payload); // Tampilkan respons dari API

          // Parsing JSON
          DynamicJsonDocument doc(30000);
          deserializeJson(doc, payload);

          // Mengambil data siswa
          JsonArray siswaArray = doc["siswa"];

          // Iterasi setiap objek siswa
          for (JsonVariant siswa : siswaArray) {
            id = siswa["id"];
           namaSiswa = siswa["nama_siswa"].as<String>();
            alamat = siswa["alamat"].as<String>();
            noTelpOrtu = siswa["no_telp_ortu"].as<String>();
            kelas = siswa["kelas"].as<String>();
            sidikJari = siswa["sidik_jari"].as<String>();
            createdAt = siswa["created_at"].as<String>();
            updatedAt = siswa["updated_at"].as<String>();
            status = siswa["status"].as<String>();

            // Lakukan tindakan dengan data siswa
            // Misalnya, cetak data siswa ke Serial Monitor
            Serial.println("ID: " + String(id));
            Serial.println("Nama Siswa: " + namaSiswa);
            Serial.println("Alamat: " + alamat);
            Serial.println("No. Telp Ortu: " + noTelpOrtu);
            Serial.println("Kelas: " + kelas);
            Serial.println("Sidik Jari: " + sidikJari);
            Serial.println("Created At: " + createdAt);
            Serial.println("Updated At: " + updatedAt);
            Serial.println("Status: " + status);
            Serial.println();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(namaSiswa);   
            lcd.setCursor(0, 1);
            lcd.print(kelas);
            nilai='1';
          }
        }
      } else {
        Serial.printf("HTTP request failed with error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("Unable to connect");
    }
  }

  char option = nilai;
  Serial.println();

  switch (option) {
    case '1':
      enrollFingerprint(id);
      break;
    case '2':
      getFingerprintID();
      break;
    default:
      getFingerprintID();
      break;
  }

  delay(1000); // Memberikan jeda setelah setiap operasi
}

uint8_t enrollFingerprint(int id) {
  uint8_t p = -1;

  Serial.println("Place your finger on the sensor...");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println("No finger detected");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  Serial.println("Remove your finger");
  lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("lepaskan jari");   
            lcd.setCursor(0, 1);
            lcd.print("tekan lagi");
  delay(2000);
  lcd.clear();

  p = -1;
  Serial.println("Place the same finger again...");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println("No finger detected");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove your finger");
  delay(2000);

  Serial.println("Place the same finger again...");

  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println("No finger detected");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Creating model...");
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Model created");
  } else {
    Serial.println("Failed to create model");
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    senddatajari(id);
    Serial.println("Fingerprint enrolled successfully!");
    return p;
  } else {
    Serial.println("Failed to store the fingerprint");
    return p;
  }
}

uint8_t getFingerprintID() {
  
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprint matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
      lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("data jari");  
            lcd.setCursor(0, 1);
            lcd.print("tidak di temukan");
            delay(1000);
            lcd.clear();
    Serial.println("Fingerprint not found");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("Fingerprint ID: ");
  Serial.println(finger.fingerID);
  Serial.print("Confidence: ");
  Serial.println(finger.confidence);
  if (WiFi.status() == WL_CONNECTED) {  
    WiFiClient client;
    HTTPClient http;
  Serial.println("Sudah Sampai Sini");
    String url = "http://absensi.mtsshifa.com/api/status/data/"+String(finger.fingerID); // Ganti URL dengan URL API yang diinginkan
    Serial.println(url);
    if (http.begin(client, url)) {
      int httpCode = http.GET();

      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
//          Serial.println(payload); // Tampilkan respons dari API

          // Parsing JSON
          DynamicJsonDocument doc(30000);
          deserializeJson(doc, payload);

          // Mengambil data siswa
          String Status=doc["status"];
          Serial.println(Status);
          JsonArray siswaArray = doc["data"];

          // Iterasi setiap objek siswa
          for (JsonVariant siswa : siswaArray) {
//            id = siswa["id"];
          String namaSiswa = siswa["nama_siswa"].as<String>();
//            alamat = siswa["alamat"].as<String>();
//            noTelpOrtu = siswa["no_telp_ortu"].as<String>();
//            kelas = siswa["kelas"].as<String>();
//            sidikJari = siswa["sidik_jari"].as<String>();
//            createdAt = siswa["created_at"].as<String>();
//            updatedAt = siswa["updated_at"].as<String>();
//            status = siswa["status"].as<String>();
            digitalWrite(D7,HIGH);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(Status);  
            lcd.setCursor(0, 1);
            lcd.print(namaSiswa);
            delay(1000);
            lcd.clear();
          }
        }
      } else {
        Serial.printf("HTTP request failed with error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("Unable to connect");
    }
  }


  return finger.fingerID;
}
void senddatajari(int value) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // Membuat permintaan POST ke API
    http.begin(client, serverjari);

    // Mengatur header permintaan
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Membuat payload data
    String postData = "sidik_jari=" + String(value);

    // Mengirim permintaan POST dengan payload data
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Respon dari server: " + response);
            digitalWrite(D7,HIGH);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Data Tersimpan");   
            lcd.setCursor(0, 1);
            lcd.print("Terimakasih");
            lcd.clear();
    } else {
      Serial.println("Gagal mengirim permintaan POST");
    }

    http.end();
  } else {
    Serial.println("Tidak terhubung ke jaringan WiFi");
  }
}
