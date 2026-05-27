#pragma once
#include <cstdint>

// A highlighted text range identified by word offsets within its spine item.
// Word offsets are counted from the start of the chapter and are stable across
// font/orientation changes because they describe the text content, not its layout.
struct Highlight {
  uint16_t spineIndex;
  uint32_t startWordOffset;  // words from chapter start to first word of selection (inclusive)
  uint32_t endWordOffset;    // words from chapter start past last word of selection (exclusive)
};
