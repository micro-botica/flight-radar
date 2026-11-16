#include "profile_store.h"

#include <Preferences.h>

namespace {
constexpr const char* kNamespace = "profiles";

// Keys are "<field><index>", for example "n0" or "la3" (NVS keys are max 15
// characters).
String key(const char* field, size_t i) { return String(field) + String((unsigned)i); }
}  // namespace

bool ProfileStore::valid(const Profile& p) const {
  return p.name.length() > 0 && profileRangeValid(p.rangeKm);
}

void ProfileStore::begin() {
  Preferences prefs;
  prefs.begin(kNamespace, true);
  size_t count = prefs.getUChar("count", 0);
  if (count > PROFILE_MAX_COUNT) count = PROFILE_MAX_COUNT;
  profiles_.clear();
  for (size_t i = 0; i < count; i++) {
    Profile p;
    p.name = prefs.getString(key("n", i).c_str(), "");
    p.ssid = prefs.getString(key("s", i).c_str(), "");
    p.password = prefs.getString(key("w", i).c_str(), "");
    p.postcode = prefs.getString(key("z", i).c_str(), "");
    p.lat = prefs.getFloat(key("la", i).c_str(), 0.0f);
    p.lon = prefs.getFloat(key("lo", i).c_str(), 0.0f);
    p.rangeKm = prefs.getUChar(key("r", i).c_str(), PROFILE_DEFAULT_RANGE_KM);
    if (!valid(p)) p.rangeKm = PROFILE_DEFAULT_RANGE_KM;
    profiles_.push_back(p);
  }
  int active = prefs.getChar("active", 0);
  prefs.end();
  active_ = profiles_.empty() ? -1 : (active < (int)profiles_.size() ? active : 0);
  Serial.printf("Profiles: %u loaded, active %d\n", (unsigned)profiles_.size(),
                active_);
}

void ProfileStore::save() {
  Preferences prefs;
  prefs.begin(kNamespace, false);
  prefs.clear();
  prefs.putUChar("count", (uint8_t)profiles_.size());
  prefs.putChar("active", (int8_t)active_);
  for (size_t i = 0; i < profiles_.size(); i++) {
    const Profile& p = profiles_[i];
    prefs.putString(key("n", i).c_str(), p.name);
    prefs.putString(key("s", i).c_str(), p.ssid);
    prefs.putString(key("w", i).c_str(), p.password);
    prefs.putString(key("z", i).c_str(), p.postcode);
    prefs.putFloat(key("la", i).c_str(), p.lat);
    prefs.putFloat(key("lo", i).c_str(), p.lon);
    prefs.putUChar(key("r", i).c_str(), p.rangeKm);
  }
  prefs.end();
}

const Profile* ProfileStore::active() const {
  if (active_ < 0 || active_ >= (int)profiles_.size()) return nullptr;
  return &profiles_[active_];
}

void ProfileStore::setActiveInternal(int index) {
  bool changed = index != active_;
  active_ = index;
  save();
  if (changed && callback_ && active()) callback_(active_, *active());
}

int ProfileStore::add(const Profile& profile) {
  if (!valid(profile) || profiles_.size() >= PROFILE_MAX_COUNT) return -1;
  profiles_.push_back(profile);
  int index = (int)profiles_.size() - 1;
  if (active_ < 0) {
    setActiveInternal(index);
  } else {
    save();
  }
  return index;
}

bool ProfileStore::update(size_t index, const Profile& profile) {
  if (index >= profiles_.size() || !valid(profile)) return false;
  profiles_[index] = profile;
  save();
  // The active profile may have moved or changed range: tell the listeners.
  if ((int)index == active_ && callback_) callback_(active_, profiles_[index]);
  return true;
}

bool ProfileStore::remove(size_t index) {
  if (index >= profiles_.size()) return false;
  profiles_.erase(profiles_.begin() + index);
  if (profiles_.empty()) {
    active_ = -1;
    save();
  } else if ((int)index == active_) {
    // The active profile is gone: fall back to the first one.
    active_ = 0;
    save();
    if (callback_) callback_(active_, profiles_[active_]);
  } else {
    if ((int)index < active_) active_--;
    save();
  }
  return true;
}

bool ProfileStore::setActive(size_t index) {
  if (index >= profiles_.size()) return false;
  setActiveInternal((int)index);
  return true;
}

void ProfileStore::onWifiChanged(const String& connectedSsid) {
  // A manual choice stays until the network changes.
  if (connectedSsid == lastSsid_) return;
  lastSsid_ = connectedSsid;
  int index = profileIndexForSsid(profiles_, connectedSsid);
  if (index >= 0) setActiveInternal(index);
}

std::vector<WifiNetwork> ProfileStore::networks() const {
  std::vector<WifiNetwork> list;
  for (const auto& p : profiles_) {
    if (p.ssid.length() > 0) list.push_back({p.ssid, p.password});
  }
  return list;
}
