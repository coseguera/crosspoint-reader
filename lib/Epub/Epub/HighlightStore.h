#pragma once
#include <string>
#include <vector>

#include "Highlight.h"

class HighlightStore {
 public:
  explicit HighlightStore(const std::string& cachePath);
  void load();
  void save() const;
  void addHighlight(const Highlight& h);
  const std::vector<Highlight>& getHighlights() const { return highlights; }
  // Returns all highlights for a spine item (caller checks word-offset overlap per line).
  std::vector<const Highlight*> getSpineHighlights(uint16_t spineIndex) const;

 private:
  std::string filePath;
  std::vector<Highlight> highlights;
  static constexpr uint8_t FILE_VERSION = 2;  // bumped: word-offset format
  static constexpr uint16_t MAX_HIGHLIGHTS = 256;
};
