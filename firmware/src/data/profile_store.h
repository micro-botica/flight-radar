#pragma once

#include <Arduino.h>

#include <functional>
#include <vector>

#include "data/wifi_manager.h"
#include "model/profile.h"

// Keeps the location profiles in NVS and tracks the active one.
//
// The active profile follows the Wi-Fi network we are connected to. A manual
// choice with setActive() sticks until the connected network changes.
class ProfileStore {
 public:
  // Called after the active profile changed, with its new index. The radar,
  // clock (profile name) and weather subscribe to this to jump to the new
  // location.
  using ChangeCallback = std::function<void(int index, const Profile& profile)>;

  // Load profiles and the active index from NVS.
  void begin();

  const std::vector<Profile>& profiles() const { return profiles_; }

  // Active profile, or nullptr when there are no profiles.
  const Profile* active() const;
  int activeIndex() const { return active_; }

  // Add a profile. Returns its index, or -1 when full or invalid.
  int add(const Profile& profile);
  // Replace a profile. Returns false for a bad index or invalid profile.
  bool update(size_t index, const Profile& profile);
  // Remove a profile. Returns false for a bad index.
  bool remove(size_t index);

  // Manual switch. Returns false for a bad index.
  bool setActive(size_t index);

  // Call when Wi-Fi connected (or lost the network, with an empty ssid).
  // Switches to the profile of that network, if there is one.
  void onWifiChanged(const String& connectedSsid);

  // Networks for WifiManager::setNetworks().
  std::vector<WifiNetwork> networks() const;

  void onChange(ChangeCallback callback) { callback_ = callback; }

 private:
  bool valid(const Profile& profile) const;
  void save();
  void setActiveInternal(int index);

  std::vector<Profile> profiles_;
  int active_ = -1;
  String lastSsid_;
  ChangeCallback callback_;
};
