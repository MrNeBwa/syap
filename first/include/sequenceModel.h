#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace seq {

enum class MessageKind {
  Sync,   
  Return, 
  Create, 
};

struct Participant {
  std::string key;   
  std::string label; 
  std::string color;
};

struct Message {
  std::size_t number = 0; 
  std::size_t from = 0;   
  std::size_t to = 0;     
  std::string text;
  MessageKind kind = MessageKind::Sync;

  [[nodiscard]] bool isSelf() const noexcept {
    return from == to;
  }
};

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
  double x = 0;        
  Rect header;           
  double lineTop = 0;    
  double lineBottom = 0; 
  std::vector<Rect> activations;
};

struct MessageLayout {
  std::size_t index = 0; 
  Point start;           
  Point end;             

  double loopX = 0;
  Point label;           
  bool toRight = true;   
};

struct LayoutMetrics {
  double headerWidth = 150;
  double headerHeight = 44;
  double columnGap = 70;   
  double margin = 24;
  double firstMessageY = 30; 
  double rowHeight = 46;    
  double selfRowHeight = 58; 
  double activationWidth = 12;
  double activationOverlap = 5; 
  double selfLoopWidth = 46;
  double tailPadding = 36; 
};

struct Layout {
  std::vector<LifelineLayout> lifelines;
  std::vector<MessageLayout> messages;
  double width = 0;
  double height = 0;
};

class SequenceModel {
public:
  SequenceModel() = default;

  std::size_t ensureParticipant(const std::string& key,
                                const std::string& label,
                                const std::string& color);

  [[nodiscard]] bool hasParticipant(const std::string& key) const;
  [[nodiscard]] std::optional<std::size_t>
  indexOf(const std::string& key) const;

  bool addMessage(const std::string& from, const std::string& to,
                  const std::string& text,
                  MessageKind kind = MessageKind::Sync);

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

  [[nodiscard]] std::optional<std::size_t>
  creatingMessageOf(std::size_t participant) const;

  [[nodiscard]] Layout layout(const LayoutMetrics& metrics = {}) const;

private:
  struct Activation {
    std::size_t participant = 0;
    std::size_t firstMessage = 0; // message index start
    std::size_t lastMessage = 0;  // message index end
    std::size_t depth = 0;  
  };

  [[nodiscard]] std::vector<Activation> computeActivations() const;

  std::vector<Participant> participants_;
  std::vector<Message> messages_;
};

} // namespace seq
