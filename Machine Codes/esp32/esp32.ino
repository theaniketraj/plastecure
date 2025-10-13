#include <Wire.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Wi-Fi credentials
#define WIFI_SSID "Wifi SSID"
#define WIFI_PASSWORD "Wifi password"

// Firebase project credentials
#define API_KEY "Your API key "
#define DATABASE_URL "Database Link"

// Firebase email/password
#define USER_EMAIL "Authenticated Email in Firebase"
#define USER_PASSWORD "Authenticated Password in Firebase"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// I2C slave address
#define I2C_SLAVE_ADDRESS 0x08
String receivedData = "";
bool newDataAvailable = false;

// Firebase token status callback
void tokenStatusCallback(TokenInfo info) {
  Serial.printf("[Firebase Token] Status: %s\n", info.status == token_status_ready ? "Ready" : "Refreshing");
}

// I2C receive handler
void receiveEvent(int howMany) {
  receivedData = "";
  while (Wire.available()) {
    char c = Wire.read();
    receivedData += c;
  }

  Serial.println("📩 [I2C] Raw data received: " + receivedData); // DEBUG
  newDataAvailable = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("🚀 ESP32 Starting");

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("🔌 Connecting to Wi-Fi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    Serial.print(".");
    delay(500);
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n❌ Failed to connect to Wi-Fi.");
    return;
  }

  Serial.println("\n✅ Wi-Fi connected");
  Serial.println("📶 IP: " + WiFi.localIP().toString());

  // Firebase config
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Begin Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  while (!Firebase.ready()) {
    Serial.println("⏳ Waiting for Firebase to be ready...");
    delay(1000);
  }
  Serial.println("✅ Firebase ready");

  // Start I2C slave
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Serial.println("📡 I2C ready to receive data");
}

void loop() {
  if (newDataAvailable) {
    newDataAvailable = false;

    Serial.println("📩 Received over I2C: " + receivedData);

    // Expected format: userID,bottleCount,rewardPoints
    int firstComma = receivedData.indexOf(',');
    int secondComma = receivedData.indexOf(',', firstComma + 1);

    if (firstComma > 0 && secondComma > firstComma) {
      String userID = receivedData.substring(0, firstComma);
      int newBottles = receivedData.substring(firstComma + 1, secondComma).toInt();
      int newRewards = receivedData.substring(secondComma + 1).toInt();

      Serial.println("Parsed Data:");
      Serial.println("  👤 User ID: " + userID);
      Serial.println("  🍼 Bottles: " + String(newBottles));
      Serial.println("  🎁 Rewards: " + String(newRewards));

      String path = "/users/" + userID;
      int totalBottles = newBottles;
      int totalRewards = newRewards;

      // Try reading existing data
      if (Firebase.RTDB.getJSON(&fbdo, path)) {
        FirebaseJson& json = fbdo.jsonObject();
        FirebaseJsonData result;

        if (json.get(result, "bottle_count") && result.type == "int") {
          totalBottles += result.intValue;
        }

        if (json.get(result, "total_rewards") && result.type == "int") {
          totalRewards += result.intValue;
        }

        Serial.println("🔁 Existing user found. Totals updated.");
      } else {
        Serial.println("🆕 New user or read failed. Creating new entry.");
      }

      // Prepare updated JSON
      FirebaseJson updatedJson;
      updatedJson.set("bottle_count", totalBottles);
      updatedJson.set("total_rewards", totalRewards);

      // Upload to Firebase
      if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &updatedJson)) {
        Serial.println("✅ Data uploaded to Firebase successfully.");
      } else {
        Serial.println("❌ Firebase upload failed:");
        Serial.println("Reason: " + fbdo.errorReason());
      }

    } else {
      Serial.println("❌ Invalid data format received over I2C.");
    }
  }
}
