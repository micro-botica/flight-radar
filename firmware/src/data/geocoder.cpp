#include "geocoder.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

geocoder::Result geocodePostcode(const String& postcode, float& lat, float& lon) {
  std::string path = geocoder::requestPath(postcode.c_str());
  if (path.empty()) return geocoder::Result::NotFound;

  // Only public postcode data is fetched and nothing secret is sent, so the
  // certificate is not pinned (no CA bundle to maintain on the device).
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setTimeout(8000);
  String url = String("https://") + geocoder::kHost + path.c_str();
  if (!http.begin(client, url)) return geocoder::Result::BadResponse;
  int code = http.GET();
  geocoder::Result result = geocoder::Result::BadResponse;
  if (code == HTTP_CODE_OK) {
    String body = http.getString();
    result = geocoder::parseResponse(body.c_str(), lat, lon);
  } else {
    Serial.printf("Geocoder: HTTP %d\n", code);
  }
  http.end();
  return result;
}
