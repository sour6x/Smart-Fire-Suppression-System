#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "YOUR_WIFI_SSID";      // <-- change if needed
const char* password = "YOUR_WIFI_PASSWORD";     // <-- change if needed

HardwareSerial UnoSerial(2);
WebServer server(80);
String currentStatus = "NO FIRE DETECTED";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Fire Suppression</title>
  <meta http-equiv='refresh' content='2'>
</head>
<body>
  <p>Fire Suppression System</p>
  <p>Status: <span id='status'>NO FIRE DETECTED</span></p>
  <script>
    fetch('/status')
      .then(response => response.text())
      .then(data => document.getElementById('status').innerHTML = data);
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  UnoSerial.begin(9600, SERIAL_8N1, 16, 17);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi‑Fi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi‑Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, []() {
      server.send(200, "text/html", index_html);
    });
    server.on("/status", HTTP_GET, []() {
      server.send(200, "text/plain", currentStatus);
    });
    server.begin();
    Serial.println("Web server started. Open the IP above in a browser.");
  } else {
    Serial.println("\nWi‑Fi FAILED. Check SSID and password.");
  }
}

void loop() {
  server.handleClient();

  while (UnoSerial.available()) {
    String msg = UnoSerial.readStringUntil('\n');
    msg.trim();
    if (msg.length() == 0) continue;

    if (msg == "SYS:READY") {
      currentStatus = "NO FIRE DETECTED";
    } else if (msg == "FIRE_DETECTED") {
      currentStatus = "FIRE DETECTED - Deploying countermeasures...";
    } else if (msg == "FIRE_CLEARED") {
      currentStatus = "FIRE SUPPRESSED";
    }
  }
}
