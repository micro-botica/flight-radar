#include "profile_web.h"

#include <ESPmDNS.h>

namespace {

String esc(const String& in) {
  String out;
  out.reserve(in.length() + 8);
  for (size_t i = 0; i < in.length(); i++) {
    char c = in[i];
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

// Dutch postcode: 4 digits and 2 letters, with an optional space.
// Normalised to "1234 AB".
bool normalisePostcode(const String& in, String& out) {
  String s;
  for (size_t i = 0; i < in.length(); i++) {
    if (in[i] != ' ') s += (char)toupper(in[i]);
  }
  if (s.length() != 6) return false;
  for (int i = 0; i < 4; i++) {
    if (!isDigit(s[i])) return false;
  }
  if (s[0] == '0') return false;
  if (!isAlpha(s[4]) || !isAlpha(s[5])) return false;
  out = s.substring(0, 4) + " " + s.substring(4);
  return true;
}

const char kStyle[] =
    "<style>"
    "body{font-family:system-ui,sans-serif;background:#0b0f14;color:#e8edf2;"
    "margin:0;padding:16px;max-width:480px;margin:auto}"
    "h1{font-size:22px}"
    ".card{background:#161c24;border-radius:12px;padding:14px;margin:10px 0}"
    ".card.active{border:2px solid #4ade80}"
    ".sub{color:#9aa7b4;font-size:14px}"
    ".badge{background:#4ade80;color:#0b0f14;border-radius:8px;padding:2px 8px;"
    "font-size:12px;font-weight:600;margin-left:6px}"
    "label{display:block;margin:12px 0 4px;font-size:14px}"
    "input,select{width:100%;box-sizing:border-box;padding:12px;font-size:16px;"
    "border-radius:8px;border:1px solid #2b3642;background:#0b0f14;color:#e8edf2}"
    "button,.btn{display:inline-block;padding:12px 16px;margin:8px 8px 0 0;"
    "font-size:16px;border-radius:8px;border:0;background:#2b3642;color:#e8edf2;"
    "text-decoration:none;cursor:pointer}"
    ".primary{background:#4ade80;color:#0b0f14;font-weight:600}"
    ".danger{background:#f87171;color:#0b0f14;font-weight:600}"
    ".msg{background:#1e3a2a;border-radius:8px;padding:10px;margin:10px 0}"
    ".err{background:#472222}"
    "</style>";

}  // namespace

void ProfileWeb::start() {
  if (MDNS.begin(kHostname)) MDNS.addService("http", "tcp", 80);
  server_.on("/", HTTP_GET, [this]() { handleList(); });
  server_.on("/edit", HTTP_GET, [this]() { handleEdit(); });
  server_.on("/save", HTTP_POST, [this]() { handleSave(); });
  server_.on("/activate", HTTP_POST, [this]() { handleActivate(); });
  server_.on("/delete", HTTP_GET, [this]() { handleDeleteConfirm(); });
  server_.on("/delete", HTTP_POST, [this]() { handleDelete(); });
  server_.onNotFound([this]() { redirectHome(); });
  server_.begin();
  running_ = true;
  Serial.printf("Profile web: http://%s.local (%s)\n", kHostname,
                WiFi.localIP().toString().c_str());
}

void ProfileWeb::stop() {
  server_.stop();
  MDNS.end();
  running_ = false;
}

void ProfileWeb::update(bool wifiConnected) {
  if (wifiConnected && !running_) start();
  if (!wifiConnected && running_) stop();
  if (running_) server_.handleClient();
}

void ProfileWeb::sendPage(const String& title, const String& body, int status) {
  String html = "<!doctype html><html lang=\"en\"><head><meta charset=\"utf-8\">"
                "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
                "<title>";
  html += esc(title);
  html += "</title>";
  html += kStyle;
  html += "</head><body><h1>";
  html += esc(title);
  html += "</h1>";
  html += body;
  html += "</body></html>";
  server_.send(status, "text/html; charset=utf-8", html);
}

void ProfileWeb::redirectHome(const String& message) {
  String url = "/";
  if (message.length() > 0) url += "?m=" + message;
  server_.sendHeader("Location", url);
  server_.send(303);
}

bool ProfileWeb::indexArg(const char* name, size_t& index) {
  if (!server_.hasArg(name)) return false;
  String v = server_.arg(name);
  if (v.length() == 0) return false;
  for (size_t i = 0; i < v.length(); i++) {
    if (!isDigit(v[i])) return false;
  }
  index = (size_t)v.toInt();
  return index < store_.profiles().size();
}

void ProfileWeb::handleList() {
  String body;
  String m = server_.arg("m");
  if (m == "saved") body += "<div class=\"msg\">Profile saved.</div>";
  if (m == "deleted") body += "<div class=\"msg\">Profile deleted.</div>";
  if (m == "active") body += "<div class=\"msg\">Profile is now active.</div>";
  if (m == "error") body += "<div class=\"msg err\">That did not work.</div>";

  const auto& profiles = store_.profiles();
  if (profiles.empty()) body += "<p class=\"sub\">No profiles yet.</p>";
  for (size_t i = 0; i < profiles.size(); i++) {
    const Profile& p = profiles[i];
    bool isActive = (int)i == store_.activeIndex();
    body += isActive ? "<div class=\"card active\">" : "<div class=\"card\">";
    body += "<strong>" + esc(p.name) + "</strong>";
    if (isActive) body += "<span class=\"badge\">Active</span>";
    body += "<div class=\"sub\">";
    body += p.postcode.length() ? esc(p.postcode) : String("No postcode");
    body += " &middot; " + String(p.rangeKm) + " km</div>";
    if (p.ssid.length()) body += "<div class=\"sub\">Wi-Fi: " + esc(p.ssid) + "</div>";
    body += "<a class=\"btn\" href=\"/edit?i=" + String(i) + "\">Edit</a>";
    if (!isActive) {
      body += "<form method=\"post\" action=\"/activate\" style=\"display:inline\">"
              "<input type=\"hidden\" name=\"i\" value=\"" + String(i) + "\">"
              "<button>Make active</button></form>";
      body += "<a class=\"btn danger\" href=\"/delete?i=" + String(i) + "\">Delete</a>";
    }
    body += "</div>";
  }
  if (profiles.size() < PROFILE_MAX_COUNT) {
    body += "<a class=\"btn primary\" href=\"/edit\">Add profile</a>";
  } else {
    body += "<p class=\"sub\">Maximum of " + String((unsigned)PROFILE_MAX_COUNT) +
            " profiles reached.</p>";
  }
  sendPage("Location profiles", body);
}

void ProfileWeb::handleEdit() {
  size_t index = 0;
  bool editing = server_.hasArg("i");
  if (editing && !indexArg("i", index)) {
    redirectHome("error");
    return;
  }
  Profile p;
  if (editing) p = store_.profiles()[index];
  String code = server_.arg("e");
  String err;
  if (code == "name") err = "Enter a name of up to 24 characters.";
  if (code == "postcode") err = "Enter a postcode like 1234 AB.";
  if (code == "range") err = "Choose a range of 20, 40 or 80 km.";
  if (code == "ssid") err = "Wi-Fi name is too long.";

  String body;
  if (err.length()) body += "<div class=\"msg err\">" + esc(err) + "</div>";
  body += "<form method=\"post\" action=\"/save\">";
  if (editing) body += "<input type=\"hidden\" name=\"i\" value=\"" + String(index) + "\">";
  body += "<label>Name</label><input name=\"name\" maxlength=\"24\" required value=\"" +
          esc(p.name) + "\">";
  body += "<label>Postcode</label><input name=\"postcode\" maxlength=\"7\" "
          "placeholder=\"1234 AB\" required value=\"" + esc(p.postcode) + "\">";
  body += "<label>Range</label><select name=\"range\">";
  for (uint8_t r : PROFILE_RANGES_KM) {
    body += "<option value=\"" + String(r) + "\"";
    if (r == p.rangeKm) body += " selected";
    body += ">" + String(r) + " km</option>";
  }
  body += "</select>";
  body += "<label>Wi-Fi network (optional)</label><input name=\"ssid\" maxlength=\"32\" value=\"" +
          esc(p.ssid) + "\">";
  body += "<label>Wi-Fi password</label><input name=\"password\" type=\"password\" "
          "maxlength=\"63\" autocomplete=\"new-password\" placeholder=\"" +
          String(editing && p.password.length() ? "Leave empty to keep the current one" : "") +
          "\">";
  body += "<button class=\"primary\">Save</button><a class=\"btn\" href=\"/\">Cancel</a>";
  body += "</form>";
  sendPage(editing ? "Edit profile" : "New profile", body);
}

void ProfileWeb::handleSave() {
  size_t index = 0;
  bool editing = server_.hasArg("i");
  if (editing && !indexArg("i", index)) {
    redirectHome("error");
    return;
  }
  Profile p;
  if (editing) p = store_.profiles()[index];

  String name = server_.arg("name");
  name.trim();
  String postcode;
  String ssid = server_.arg("ssid");
  ssid.trim();
  long range = server_.arg("range").toInt();

  String error;
  if (name.length() == 0 || name.length() > 24) {
    error = "name";
  } else if (!normalisePostcode(server_.arg("postcode"), postcode)) {
    error = "postcode";
  } else if (!profileRangeValid((uint8_t)range)) {
    error = "range";
  } else if (ssid.length() > 32) {
    error = "ssid";
  }
  if (error.length()) {
    // Show the form again with a message. The user retypes the fields.
    server_.sendHeader("Location", String("/edit?") +
                                       (editing ? "i=" + String(index) + "&" : String()) +
                                       "e=" + error);
    server_.send(303);
    return;
  }

  // The postcode to coordinates lookup is a separate feature. Until then a
  // changed postcode keeps the old coordinates.
  p.name = name;
  p.postcode = postcode;
  p.rangeKm = (uint8_t)range;
  p.ssid = ssid;
  String password = server_.arg("password");
  if (password.length() > 0 || ssid.length() == 0) p.password = password;

  bool ok = editing ? store_.update(index, p) : store_.add(p) >= 0;
  if (ok && changed_) changed_();
  redirectHome(ok ? "saved" : "error");
}

void ProfileWeb::handleActivate() {
  size_t index = 0;
  if (!indexArg("i", index) || !store_.setActive(index)) {
    redirectHome("error");
    return;
  }
  redirectHome("active");
}

void ProfileWeb::handleDeleteConfirm() {
  size_t index = 0;
  // The active profile cannot be deleted.
  if (!indexArg("i", index) || (int)index == store_.activeIndex()) {
    redirectHome("error");
    return;
  }
  const Profile& p = store_.profiles()[index];
  String body = "<p>Delete <strong>" + esc(p.name) + "</strong>? This cannot be undone.</p>"
                "<form method=\"post\" action=\"/delete\">"
                "<input type=\"hidden\" name=\"i\" value=\"" + String(index) + "\">"
                "<button class=\"danger\">Delete</button>"
                "<a class=\"btn\" href=\"/\">Cancel</a></form>";
  sendPage("Delete profile", body);
}

void ProfileWeb::handleDelete() {
  size_t index = 0;
  if (!indexArg("i", index) || (int)index == store_.activeIndex() ||
      !store_.remove(index)) {
    redirectHome("error");
    return;
  }
  if (changed_) changed_();
  redirectHome("deleted");
}
