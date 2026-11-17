#pragma once

// HTML for the setup portal: one page, four steps (T1 to T4 in the design).
// Plain HTML and a little JavaScript, no external files, so it works while
// the phone has no internet.

#include <string>
#include <vector>

#include "portal_core.h"

namespace portal {

inline std::string htmlEscape(const std::string& in) {
  std::string out;
  for (char c : in) {
    switch (c) {
      case '&': out += "&amp;"; break;
      case '<': out += "&lt;"; break;
      case '>': out += "&gt;"; break;
      case '"': out += "&quot;"; break;
      case '\'': out += "&#39;"; break;
      default: out += c;
    }
  }
  return out;
}

inline std::string setupPage(const std::vector<PortalNetwork>& networks,
                             const std::string& error) {
  std::string h =
      "<!DOCTYPE html><html><head><meta charset=\"utf-8\">"
      "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
      "<title>Flight Radar setup</title><style>"
      "body{font-family:sans-serif;background:#0b0f0c;color:#e8ffee;margin:0;"
      "padding:16px;max-width:480px;margin:auto}"
      "h1{color:#00ff66;font-size:22px}"
      ".step{display:none}.step.on{display:block}"
      "label.net{display:flex;justify-content:space-between;padding:12px;"
      "margin:6px 0;background:#18221b;border-radius:8px}"
      "input[type=text],input[type=password]{width:100%;box-sizing:border-box;"
      "padding:12px;font-size:16px;margin:6px 0;border-radius:8px;border:0}"
      "button{width:100%;padding:14px;font-size:16px;margin-top:12px;border:0;"
      "border-radius:8px;background:#00ff66;color:#000}"
      "button.s{background:#333;color:#fff}.err{color:#ff6666}"
      "</style></head><body><h1>Flight Radar setup</h1>";
  if (!error.empty()) h += "<p class=\"err\">" + htmlEscape(error) + "</p>";
  h += "<form method=\"post\" action=\"/save\" id=\"f\">";

  // Step 1: choose a network.
  h += "<div class=\"step on\" id=\"s1\"><h2>1. Choose Wi-Fi</h2>";
  if (networks.empty()) h += "<p>No networks found. Reload to scan again.</p>";
  for (const auto& n : networks) {
    std::string bars(signalBars(n.rssi), '|');
    h += "<label class=\"net\"><span><input type=\"radio\" name=\"ssid\" "
         "value=\"" + htmlEscape(n.ssid) + "\"> " + htmlEscape(n.ssid) +
         "</span><span>" + bars + " " + std::to_string(n.rssi) +
         " dBm</span></label>";
  }
  h += "<button type=\"button\" onclick=\"go(2)\">Next</button></div>";

  // Step 2: password.
  h += "<div class=\"step\" id=\"s2\"><h2>2. Password</h2>"
       "<input type=\"password\" name=\"password\" id=\"pw\" "
       "autocomplete=\"off\" placeholder=\"Wi-Fi password\">"
       "<label><input type=\"checkbox\" onclick=\"pw.type=this.checked?"
       "'text':'password'\"> Show password</label>"
       "<button type=\"button\" onclick=\"go(3)\">Next</button>"
       "<button type=\"button\" class=\"s\" onclick=\"go(1)\">Back</button>"
       "</div>";

  // Step 3: postcode and profile name.
  h += "<div class=\"step\" id=\"s3\"><h2>3. Location</h2>"
       "<input type=\"text\" name=\"postcode\" placeholder=\"Postcode, 1234 AB\""
       " maxlength=\"7\" autocomplete=\"postal-code\">"
       "<input type=\"text\" name=\"name\" placeholder=\"Profile name\" "
       "value=\"Home\" maxlength=\"24\">"
       "<button type=\"submit\">Save</button>"
       "<button type=\"button\" class=\"s\" onclick=\"go(2)\">Back</button>"
       "</div></form>"
       "<script>var pw=document.getElementById('pw');"
       "function go(n){for(var i=1;i<=3;i++)document.getElementById('s'+i)"
       ".className='step'+(i==n?' on':'');}"
       "</script></body></html>";
  return h;
}

// Step 4: done screen.
inline std::string donePage(const std::string& ssid) {
  return "<!DOCTYPE html><html><head><meta charset=\"utf-8\">"
         "<meta name=\"viewport\" content=\"width=device-width,"
         "initial-scale=1\"><title>Done</title></head>"
         "<body style=\"font-family:sans-serif;background:#0b0f0c;"
         "color:#e8ffee;padding:16px\"><h1 style=\"color:#00ff66\">"
         "4. Done</h1><p>The radar is now connecting to <b>" +
         htmlEscape(ssid) +
         "</b>. You can close this page.</p></body></html>";
}

}  // namespace portal
