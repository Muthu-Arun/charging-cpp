/*
 * ESP32 HTTP POST Request Example
 * * This code does the following:
 * 1. Connects to WiFi.
 * 2. Creates a JSON payload: {"outlet_id": 123}
 * 3. Sends this payload via HTTP POST to a REST API.
 * 4. Receives a JSON response: {"status": "available"} or {"status": "occupied"}
 * 5. Parses the response and prints the status.
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- START: UPDATE THESE VALUES ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Your REST API endpoint URL
// Example: "http://your-server.com/api/check_status"
const char* api_url = "http://YOUR_SERVER.COM/api/endpoint"; 

// The outlet ID you want to send
int my_outlet_id = 123; 
// --- END: UPDATE THESE VALUES ---

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // We only run this once for demonstration.
  // In a real app, you might trigger this on a button press
  // or run it every 30 seconds.
  
  Serial.println("\nMaking API Request...");
  checkOutletStatus(my_outlet_id);

  // Wait for 30 seconds before next request
  delay(30000); 
}

void checkOutletStatus(int outletId) {
  
  // 1. Create the JSON payload for the request
  // Use StaticJsonDocument for predictable memory usage on ESP
  StaticJsonDocument<64> requestDoc; // 64 bytes is enough for {"outlet_id":123}
  requestDoc["outlet_id"] = outletId;

  // Serialize JSON to a string
  String jsonRequestBuffer;
  serializeJson(requestDoc, jsonRequestBuffer);

  // 2. Configure the HTTP client
  HTTPClient http;
  http.begin(api_url);
  
  // Set the content type header to application/json
  http.addHeader("Content-Type", "application/json");

  // 3. Send the HTTP POST request
  Serial.print("Sending POST request with payload: ");
  Serial.println(jsonRequestBuffer);
  
  int httpResponseCode = http.POST(jsonRequestBuffer);

  // 4. Check the response
  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    
    if (httpResponseCode == HTTP_CODE_OK) { // Check for 200 OK
      String jsonResponsePayload = http.getString();
      Serial.print("Received payload: ");
      Serial.println(jsonResponsePayload);

      // 5. Parse the JSON response
      StaticJsonDocument<64> responseDoc; // 64 bytes is enough for {"status":"available"}
      DeserializationError error = deserializeJson(responseDoc, jsonResponsePayload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        http.end(); // Free resources
        return;
      }

      // 6. Extract the "status" value
      const char* status = responseDoc["status"]; // "available" or "occupied"
      
      Serial.print("Outlet status is: ");
      Serial.println(status);

      // --- Here you can add your logic ---
      // For example, turn on an LED if it's "available"
      // if (strcmp(status, "available") == 0) {
      //   digitalWrite(LED_BUILTIN, HIGH);
      // } else {
      //   digitalWrite(LED_BUILTIN, LOW);
      // }
      // ------------------------------------

    } else {
      Serial.printf("Server returned error code: %d\n", httpResponseCode);
    }
    
  } else {
    Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  // Free resources
  http.end();
}