#pragma once

#include <string>

// Screen shown while the setup portal runs: a QR code to join the access
// point and the network name as text.
void showSetupScreen(const std::string& apName);
