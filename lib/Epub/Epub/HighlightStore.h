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
  // Removes all highlights whose word range overlaps [pageWordStart, pageWordEnd).
  void removePageHighlights(uint16_t spineIndex, uint32_t pageWordStart, uint32_t pageWordEnd);
  const std::vector<Highlight>& getHighlights() const { return highlights; }
  // Returns all highlights for a spine item (caller checks word-offset overlap per line).
  std::vector<const Highlight*> getSpineHighlights(uint16_t spineIndex) const;
  // Returns the highlight with the earliest (spineIndex, startWordOffset) that starts at or
  // after pageWordEnd in the same spine, or in any later spine.
  const Highlight* findNextHighlight(uint16_t currentSpineIndex, uint32_t pageWordEnd) const;
  // Returns the highlight with the latest (spineIndex, startWordOffset) that ends at or
  // before pageWordStart in the same spine, or in any earlier spine.
  const Highlight* findPrevHighlight(uint16_t currentSpineIndex, uint32_t pageWordStart) const;

 private:
  std::string filePath;
  std::vector<Highlight> highlights;
  static constexpr uint8_t FILE_VERSION = 2;  // bumped: word-offset format
  static constexpr uint16_t MAX_HIGHLIGHTS = 256;
};
