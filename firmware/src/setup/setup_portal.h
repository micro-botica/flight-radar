#pragma once

#ifdef ARDUINO

#include <functional>
#include <string>

#include "portal_core.h"

// Access point "Radar-XXXX" with a captive portal. Call begin(), then
// update() from loop(). When the user has filled in the form, the callback
// gets the validated result.
class SetupPortal {
 public:
  using DoneCallback = std::function<void(const SetupResult&)>;

  void begin(DoneCallback onDone);
  void update();
  bool active() const { return active_; }
  const std::string& apName() const { return apName_; }

 private:
  void handleRoot();
  void handleSave();
  void handleRedirect();
  void refreshScan();

  bool active_ = false;
  std::string apName_;
  DoneCallback onDone_;
};

#endif  // ARDUINO
