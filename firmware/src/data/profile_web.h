#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include <functional>

#include "data/profile_store.h"

// Web page on the device to manage the location profiles from a phone.
// Reachable on the local network at http://flightradar.local (or the IP
// address). Starts once Wi-Fi is connected.
//
// There is no login: anyone on the same network can edit the profiles. The
// stored Wi-Fi passwords are never sent back to the browser.
class ProfileWeb {
 public:
  static constexpr const char* kHostname = "flightradar";

  explicit ProfileWeb(ProfileStore& store) : store_(store) {}

  // Called after the profiles were changed through the web page, so the Wi-Fi
  // manager can pick up the new list of networks.
  void onProfilesChanged(std::function<void()> callback) { changed_ = callback; }

  // Call regularly from loop(). Starts the server when Wi-Fi comes up and
  // stops it when Wi-Fi is lost.
  void update(bool wifiConnected);

 private:
  void start();
  void stop();
  void handleList();
  void handleEdit();
  void handleSave();
  void handleActivate();
  void handleDeleteConfirm();
  void handleDelete();
  void sendPage(const String& title, const String& body, int status = 200);
  void redirectHome(const String& message = String());
  bool indexArg(const char* name, size_t& index);

  ProfileStore& store_;
  WebServer server_{80};
  std::function<void()> changed_;
  bool running_ = false;
};
