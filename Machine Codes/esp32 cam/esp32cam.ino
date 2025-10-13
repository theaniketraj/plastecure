#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <esp_camera.h>
#include <ArduinoJson.h>

// Wi‑Fi credentials
const char* ssid      = "WIFI SSID";
const char* password  = "WIFI PAssword";

// Server URL
const char* serverBase = "https://1e8a278effcd.ngrok-free.app";
const char* detectPath = "/detect";

// GPIO pins
const int motionPin = 13;  // PIR motion sensor output
const int flashPin  = 4;   // Flash LED

// AI‑Thinker camera pin definitions
#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

void setup() {
  Serial.begin(115200);

  pinMode(motionPin, INPUT);
  pinMode(flashPin, OUTPUT);
  digitalWrite(flashPin, LOW);

  // Connect Wi‑Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi‑Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("\nWi‑Fi connected: " + WiFi.localIP().toString());

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size   = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;
    config.fb_count     = 2;
  } else {
    config.frame_size   = FRAMESIZE_CIF;
    config.jpeg_quality = 12;
    config.fb_count     = 1;
  }

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    delay(1000);
    ESP.restart();
  }
}

void loop() {
  static bool  prevMotion     = false;
  static unsigned long lastCapture = 0;

  bool motion = (digitalRead(motionPin) == HIGH);
  unsigned long now = millis();

  // On rising edge AND at least 3 s since last capture
  if (motion && !prevMotion && (now - lastCapture >= 3000)) {
    Serial.println("→ Motion detected, capturing image");

    // flash and capture
    digitalWrite(flashPin, HIGH);
    delay(200);
    camera_fb_t *fb = esp_camera_fb_get();
    digitalWrite(flashPin, LOW);

    if (!fb) {
      Serial.println("Capture failed");
      esp_camera_fb_return(fb);
      return;
    }

    // build multipart body
    String url = String(serverBase) + detectPath;
    const char *boundary = "----ESP32CAM";
    String head = String("--") + boundary + "\r\n"
                + "Content-Disposition: form-data; name=\"image\"; filename=\"capture.jpg\"\r\n"
                + "Content-Type: image/jpeg\r\n\r\n";
    String tail = String("\r\n--") + boundary + "--\r\n";
    size_t totalLen = head.length() + fb->len + tail.length();
    uint8_t *body = (uint8_t*)malloc(totalLen);

    if (body) {
      size_t off = 0;
      memcpy(body + off, head.c_str(), head.length()); off += head.length();
      memcpy(body + off, fb->buf, fb->len);         off += fb->len;
      memcpy(body + off, tail.c_str(), tail.length());

      WiFiClientSecure wclient;
      wclient.setInsecure();
      HTTPClient http;
      http.begin(wclient, url);
      http.addHeader("Content-Type", "multipart/form-data; boundary=" + String(boundary));
      int code = http.POST(body, totalLen);
      free(body);
      esp_camera_fb_return(fb);

      String resp = http.getString();
      Serial.printf("HTTP %d → %s\n", code, resp.c_str());
      http.end();

      if (code > 0) {
        DynamicJsonDocument doc(256);
        if (!deserializeJson(doc, resp)) {
          bool bottle = doc["bottle"];
          Serial.printf("Bottle detected: %s\n", bottle ? "YES" : "NO");
        } else {
          Serial.println("JSON parse error");
        }
      } else {
        Serial.printf("HTTP POST failed: %s\n", http.errorToString(code).c_str());
      }
    } else {
      Serial.println("Memory alloc failed");
      esp_camera_fb_return(fb);
    }

    lastCapture = now;
  }

  prevMotion = motion;
  delay(50);  // small debounce interval
}
