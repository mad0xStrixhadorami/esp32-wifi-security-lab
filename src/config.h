#pragma once

// ---------------------------------------------------------------------------
// Lab configuration
//
// This project is a captive-portal / "evil twin" demonstration. It must only
// ever be run on hardware and airspace you own or have explicit written
// authorization to test on (your own isolated lab network, a CTF range, or
// a scoped pentest engagement). See README.md -> "Legal & ethical use".
// ---------------------------------------------------------------------------

// SSID the ESP32 broadcasts. Kept generic/fictional on purpose -- do not
// rename this to impersonate a real business, airport, or carrier's SSID.
#define LAB_SSID        "SkyLink Free WiFi (LAB)"
#define LAB_PASSWORD    nullptr   // open network, by design -- that's the point of the demo

// Printed in the Serial monitor and shown as a small footer on the portal
// page so nobody mistakes this for a real network.
#define LAB_BANNER_TEXT "WIFI SECURITY AWARENESS DEMO -- LAB USE ONLY, NOT A REAL NETWORK"

// How many credential submissions to keep in the ring buffer.
#define MAX_STORED_CREDENTIALS 10

// Path to the logo served on the portal page (must be uploaded to SPIFFS
// from the /data folder -- see README.md -> "Flashing the data partition").
#define LOGO_PATH "/logo.png"
