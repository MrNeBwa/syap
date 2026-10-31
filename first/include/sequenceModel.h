#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace seq {

// Kind of a message arrow on the diagram.
enum class MessageKind {
  Sync,   // solid line, filled arrowhead: ordinary call
  Return, // dashed line, open arrowhead: reply to a call
  Create, // solid line that ends at a participant header (object creation)
};

struct Participant {
  std::string key;   // unique id used by callers
  std::string label; // text in the header box
  std::string color; // "#rrggbb", interpreted by the renderer
};

struct Message {
  std::size_t number = 0; // 1-based, assigned by the model
  std::size_t from = 0;   // participant index
  std::size_t to = 0;     // participant index
  std::string text;
  MessageKind kind = MessageKind::Sync;

  [[nodiscard]] bool isSelf() const noexcept {
    return from == to;
  }
};

// Geometry produced by the layout; plain numbers, no Qt types.
struct Rect {
  double x = 0;
  double y = 0;
  double w = 0;
  double h = 0;
};

struct Point {
  double x = 0;
  double y = 0;
};

struct LifelineLayout {
  double x = 0;          // centre line
  Rect header;           // header box
  double lineTop = 0;    // lifeline starts here (below the header)
  double lineBottom = 0; // lifeline ends here
  std::vector<Rect> activations;
};

struct MessageLayout {
  std::size_t index = 0; // index into SequenceModel::messages()
  Point start;           // arrow start
  Point end;             // arrow end (tip)
  // Only for self messages: the loop goes start -> (loopX, start.y)
  // -> (loopX, end.y) -> end.
  double loopX = 0;
  Point label;           // anchor of the label (left edge, baseline-ish)
  bool toRight = true;   // direction of a non-self arrow
};

struct LayoutMetrics {
  double headerWidth = 150;
  double headerHeight = 44;
  double columnGap = 70;   // gap between neighbouring headers
  double margin = 24;
  double firstMessageY = 30; // distance from header bottom to message 1
  double rowHeight = 46;     // vertical step between messages
  double selfRowHeight = 58; // vertical step used by self messages
  double activationWidth = 12;
  double activationOverlap = 5; // shift of nested activation bars
  double selfLoopWidth = 46;
  double tailPadding = 36; // lifeline continues below the last message
};

struct Layout {
  std::vector<LifelineLayout> lifelines;
  std::vector<MessageLayout> messages;
  double width = 0;
  double height = 0;
};

// Pure data + logic. No Qt, so it can be unit-tested anywhere.
class SequenceModel {
public:
  SequenceModel() = default;

  // Adds a participant or updates label/colour of an existing one.
  // Returns its index. Participants keep first-seen order (left to right).
  std::size_t ensureParticipant(const std::string& key,
                                const std::string& label,
                                const std::string& color);

  [[nodiscard]] bool hasParticipant(const std::string& key) const;
  [[nodiscard]] std::optional<std::size_t>
  indexOf(const std::string& key) const;

  // Adds a message between two known participants.
  // Returns false (and changes nothing) if either key is unknown, the text
  // is empty, or a Create message targets a participant that already
  // has messages of its own or is created twice or is the sender itself.
  bool addMessage(const std::string& from, const std::string& to,
                  const std::string& text,
                  MessageKind kind = MessageKind::Sync);

  // Convenience for "call + reply" pairs: a Sync from->to followed by a
  // Return to->from. Self calls get no separate return arrow.
  bool addCall(const std::string& from, const std::string& to,
               const std::string& text, const std::string& reply = {});

  void clearMessages();
  void clear();

  [[nodiscard]] const std::vector<Participant>& participants() const noexcept {
    return participants_;
  }
  [[nodiscard]] const std::vector<Message>& messages() const noexcept {
    return messages_;
  }

  // Index of the message that created participant i, if any.
  [[nodiscard]] std::optional<std::size_t>
  creatingMessageOf(std::size_t participant) const;

  [[nodiscard]] Layout layout(const LayoutMetrics& metrics = {}) const;

private:
  struct Activation {
    std::size_t participant = 0;
    std::size_t firstMessage = 0; // message index where the bar starts
    std::size_t lastMessage = 0;  // message index where the bar ends
    std::size_t depth = 0;        // nesting level for overlapping bars
  };

  [[nodiscard]] std::vector<Activation> computeActivations() const;

  std::vector<Participant> participants_;
  std::vector<Message> messages_;
};

} // namespace seq
