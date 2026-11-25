#pragma once

// Pure logic of the postcode lookup (no hardware, runs on a PC): building the
// PDOK Locatieserver request and reading the coordinates from the answer.

#include <stdlib.h>

#include <string>

namespace geocoder {

constexpr const char* kHost = "api.pdok.nl";

// Request path for a postcode in the form "1234 AB" (spaces are dropped).
// Returns "" when the postcode is not a Dutch postcode.
inline std::string requestPath(const std::string& postcode) {
  std::string s;
  for (char c : postcode) {
    if (c != ' ') s += c;
  }
  if (s.size() != 6 || s[0] < '1' || s[0] > '9') return "";
  for (int i = 1; i < 4; i++) {
    if (s[i] < '0' || s[i] > '9') return "";
  }
  for (int i = 4; i < 6; i++) {
    if (s[i] < 'A' || s[i] > 'Z') return "";
  }
  return "/bzk/locatieserver/search/v3_1/free?q=postcode:" + s +
         "&fq=type:postcode&rows=1&fl=centroide_ll";
}

enum class Result { Ok, NotFound, BadResponse };

// Reads the first "centroide_ll":"POINT(lon lat)" from the JSON answer.
// PDOK writes longitude first. An answer without any match is NotFound.
inline Result parseResponse(const std::string& json, float& lat, float& lon) {
  size_t key = json.find("\"centroide_ll\"");
  if (key == std::string::npos) {
    return json.find("\"numFound\":0") != std::string::npos ? Result::NotFound
                                                            : Result::BadResponse;
  }
  size_t open = json.find("POINT(", key);
  if (open == std::string::npos) return Result::BadResponse;
  const char* start = json.c_str() + open + 6;
  char* end = nullptr;
  double x = strtod(start, &end);
  if (end == start) return Result::BadResponse;
  const char* second = end;
  double y = strtod(second, &end);
  if (end == second) return Result::BadResponse;
  // Sanity check: roughly the Netherlands.
  if (y < 50.5 || y > 53.8 || x < 3.0 || x > 7.5) return Result::BadResponse;
  lat = (float)y;
  lon = (float)x;
  return Result::Ok;
}

inline const char* errorText(Result r) {
  switch (r) {
    case Result::Ok: return "";
    case Result::NotFound: return "Postcode not found. Check the postcode.";
    default: return "Could not look up the postcode. Try again later.";
  }
}

}  // namespace geocoder
