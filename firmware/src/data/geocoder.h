#pragma once

#include <Arduino.h>

#include "data/geocoder_core.h"

// Turns a Dutch postcode ("1234 AB") into latitude and longitude with the
// PDOK Locatieserver. Needs a Wi-Fi connection. Blocks for up to a few
// seconds, so call it from a request handler or a slow retry, not per frame.
geocoder::Result geocodePostcode(const String& postcode, float& lat, float& lon);
