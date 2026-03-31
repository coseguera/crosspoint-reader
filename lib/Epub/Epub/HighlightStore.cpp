#include "HighlightStore.h"

#include <HalStorage.h>
#include <Logging.h>
#include <Serialization.h>

#include <algorithm>

HighlightStore::HighlightStore(const std::string& cachePath) : filePath(cachePath + "/highlights.bin") {}

void HighlightStore::load() {
  highlights.clear();
  FsFile f;
  if (!Storage.openFileForRead("HLS", filePath, f)) {
    return;  // No highlights file yet — normal on first open
  }

  uint8_t version = 0;
  serialization::readPod(f, version);
  if (version != FILE_VERSION) {
    f.close();
    LOG_DBG("HLS", "Highlight file version mismatch (%u != %u), starting fresh", version, FILE_VERSION);
    return;
  }

  uint16_t count = 0;
  serialization::readPod(f, count);
  if (count > MAX_HIGHLIGHTS) {
    count = MAX_HIGHLIGHTS;
  }

  highlights.reserve(count);
  for (uint16_t i = 0; i < count; i++) {
    Highlight h;
    serialization::readPod(f, h.spineIndex);
    serialization::readPod(f, h.startWordOffset);
    serialization::readPod(f, h.endWordOffset);
    highlights.push_back(h);
  }
  f.close();
  LOG_DBG("HLS", "Loaded %u highlights", static_cast<unsigned>(highlights.size()));
}

void HighlightStore::save() const {
  FsFile f;
  if (!Storage.openFileForWrite("HLS", filePath, f)) {
    LOG_ERR("HLS", "Failed to save highlights");
    return;
  }

  serialization::writePod(f, FILE_VERSION);
  const uint16_t count = static_cast<uint16_t>(std::min(highlights.size(), static_cast<size_t>(MAX_HIGHLIGHTS)));
  serialization::writePod(f, count);
  for (uint16_t i = 0; i < count; i++) {
    const auto& h = highlights[i];
    serialization::writePod(f, h.spineIndex);
    serialization::writePod(f, h.startWordOffset);
    serialization::writePod(f, h.endWordOffset);
  }
  f.close();
  LOG_DBG("HLS", "Saved %u highlights", static_cast<unsigned>(count));
}

void HighlightStore::removePageHighlights(const uint16_t spineIndex, const uint32_t pageWordStart,
                                          const uint32_t pageWordEnd) {
  highlights.erase(
      std::remove_if(highlights.begin(), highlights.end(),
                     [spineIndex, pageWordStart, pageWordEnd](const Highlight& h) {
                       return h.spineIndex == spineIndex && h.startWordOffset < pageWordEnd &&
                              h.endWordOffset > pageWordStart;
                     }),
      highlights.end());
}

void HighlightStore::addHighlight(const Highlight& h) {
  if (highlights.size() >= MAX_HIGHLIGHTS) {
    LOG_ERR("HLS", "Highlight limit reached, cannot add more");
    return;
  }
  highlights.push_back(h);
}

const Highlight* HighlightStore::findNextHighlight(const uint16_t currentSpineIndex,
                                                    const uint32_t pageWordEnd) const {
  const Highlight* best = nullptr;
  for (const auto& h : highlights) {
    const bool isAfter = (h.spineIndex > currentSpineIndex) ||
                         (h.spineIndex == currentSpineIndex && h.startWordOffset >= pageWordEnd);
    if (!isAfter) {
      continue;
    }
    if (best == nullptr || h.spineIndex < best->spineIndex ||
        (h.spineIndex == best->spineIndex && h.startWordOffset < best->startWordOffset)) {
      best = &h;
    }
  }
  return best;
}

const Highlight* HighlightStore::findPrevHighlight(const uint16_t currentSpineIndex,
                                                    const uint32_t pageWordStart) const {
  const Highlight* best = nullptr;
  for (const auto& h : highlights) {
    const bool isBefore = (h.spineIndex < currentSpineIndex) ||
                          (h.spineIndex == currentSpineIndex && h.endWordOffset <= pageWordStart);
    if (!isBefore) {
      continue;
    }
    if (best == nullptr || h.spineIndex > best->spineIndex ||
        (h.spineIndex == best->spineIndex && h.startWordOffset > best->startWordOffset)) {
      best = &h;
    }
  }
  return best;
}

std::vector<const Highlight*> HighlightStore::getSpineHighlights(const uint16_t spineIndex) const {
  std::vector<const Highlight*> result;
  for (const auto& h : highlights) {
    if (h.spineIndex == spineIndex) {
      result.push_back(&h);
    }
  }
  return result;
}
