#include "sequenceModel.h"

#include <algorithm>
#include <utility>

namespace seq {

namespace {

constexpr double kSelfLoopHeight = 24.0;
constexpr double kActivationPad = 10.0;

} // namespace

std::size_t SequenceModel::ensureParticipant(const std::string& key,
                                             const std::string& label,
                                             const std::string& color) {
  if (const auto existing = indexOf(key)) {
    participants_[*existing].label = label;
    participants_[*existing].color = color;
    return *existing;
  }
  participants_.push_back({key, label, color});
  return participants_.size() - 1;
}

bool SequenceModel::hasParticipant(const std::string& key) const {
  return indexOf(key).has_value();
}

std::optional<std::size_t>
SequenceModel::indexOf(const std::string& key) const {
  for (std::size_t i = 0; i < participants_.size(); ++i) {
    if (participants_[i].key == key) {
      return i;
    }
  }
  return std::nullopt;
}

std::optional<std::size_t>
SequenceModel::creatingMessageOf(std::size_t participant) const {
  for (std::size_t i = 0; i < messages_.size(); ++i) {
    if (messages_[i].kind == MessageKind::Create &&
        messages_[i].to == participant) {
      return i;
    }
  }
  return std::nullopt;
}

bool SequenceModel::addMessage(const std::string& from, const std::string& to,
                               const std::string& text, MessageKind kind) {
  const auto fromIndex = indexOf(from);
  const auto toIndex = indexOf(to);
  if (!fromIndex || !toIndex || text.empty()) {
    return false;
  }

  if (kind == MessageKind::Create) {
    if (*fromIndex == *toIndex) {
      return false;
    }
    for (const Message& message : messages_) {
      if (message.from == *toIndex || message.to == *toIndex) {
        return false;
      }
    }
  }

  Message message;
  message.number = messages_.size() + 1;
  message.from = *fromIndex;
  message.to = *toIndex;
  message.text = text;
  message.kind = kind;
  messages_.push_back(std::move(message));
  return true;
}

bool SequenceModel::addCall(const std::string& from, const std::string& to,
                            const std::string& text,
                            const std::string& reply) {
  if (!addMessage(from, to, text, MessageKind::Sync)) {
    return false;
  }
  if (!reply.empty() && from != to) {
    return addMessage(to, from, reply, MessageKind::Return);
  }
  return true;
}

void SequenceModel::clearMessages() {
  messages_.clear();
}

void SequenceModel::clear() {
  messages_.clear();
  participants_.clear();
}

std::vector<SequenceModel::Activation> SequenceModel::computeActivations() const {
  const std::size_t count = messages_.size();
  std::vector<Activation> result;
  std::vector<bool> returnUsed(count, false);

  auto contains = [](const Activation& outer, const Activation& inner) {
    return outer.participant == inner.participant &&
           outer.firstMessage <= inner.firstMessage &&
           outer.lastMessage >= inner.lastMessage;
  };

  for (std::size_t i = 0; i < count; ++i) {
    const Message& call = messages_[i];
    if (call.kind == MessageKind::Return || call.isSelf()) {
      continue;
    }

    std::size_t reply = count; // none
    for (std::size_t j = i + 1; j < count; ++j) {
      const Message& candidate = messages_[j];
      if (!returnUsed[j] && candidate.kind == MessageKind::Return &&
          candidate.from == call.to && candidate.to == call.from) {
        reply = j;
        break;
      }
    }

    std::size_t calleeLast = i;
    std::size_t callerLast = i;
    if (reply != count) {
      returnUsed[reply] = true;
      calleeLast = reply;
      callerLast = reply;
    } else {
      while (calleeLast + 1 < count &&
             messages_[calleeLast + 1].from == call.to) {
        ++calleeLast;
      }
    }

    result.push_back({call.to, i, calleeLast, 0});

    const bool callerActive = std::any_of(
        result.begin(), result.end(), [&](const Activation& activation) {
          return activation.participant == call.from &&
                 activation.firstMessage <= i && activation.lastMessage >= i;
        });
    if (!callerActive) {
      result.push_back({call.from, i, callerLast, 0});
    }
  }

  std::sort(result.begin(), result.end(),
            [](const Activation& a, const Activation& b) {
              if (a.participant != b.participant) {
                return a.participant < b.participant;
              }
              if (a.firstMessage != b.firstMessage) {
                return a.firstMessage < b.firstMessage;
              }
              return a.lastMessage > b.lastMessage;
            });
  result.erase(std::unique(result.begin(), result.end(),
                           [](const Activation& a, const Activation& b) {
                             return a.participant == b.participant &&
                                    a.firstMessage == b.firstMessage &&
                                    a.lastMessage == b.lastMessage;
                           }),
               result.end());

  for (std::size_t i = 0; i < result.size(); ++i) {
    std::size_t depth = 0;
    for (std::size_t k = 0; k < i; ++k) {
      if (contains(result[k], result[i])) {
        ++depth;
      }
    }
    result[i].depth = depth;
  }

  for (std::size_t i = 0; i < count; ++i) {
    if (!messages_[i].isSelf()) {
      continue;
    }
    const std::size_t who = messages_[i].from;
    bool covered = false;
    std::size_t deepest = 0;
    for (const Activation& activation : result) {
      if (activation.participant == who && activation.firstMessage <= i &&
          activation.lastMessage >= i) {
        covered = true;
        deepest = std::max(deepest, activation.depth);
      }
    }
    if (!covered) {
      result.push_back({who, i, i, 0});
      deepest = 0;
    }
    result.push_back({who, i, i, deepest + 1});
  }

  return result;
}

Layout SequenceModel::layout(const LayoutMetrics& m) const {
  Layout out;
  const std::size_t columns = participants_.size();
  const std::size_t count = messages_.size();

  std::vector<double> ys(count, 0.0);
  const double firstY = m.margin + m.headerHeight + m.firstMessageY;
  for (std::size_t i = 0; i < count; ++i) {
    ys[i] = i == 0 ? firstY
                   : ys[i - 1] + (messages_[i - 1].isSelf() ? m.selfRowHeight
                                                             : m.rowHeight);
  }

  double bottomY = m.margin + m.headerHeight;
  if (count > 0) {
    bottomY = ys.back() + (messages_.back().isSelf() ? kSelfLoopHeight : 0.0);
  }
  const double lineBottom = bottomY + m.tailPadding;

  // Lifelines and headers.
  out.lifelines.resize(columns);
  for (std::size_t c = 0; c < columns; ++c) {
    LifelineLayout& line = out.lifelines[c];
    line.x = m.margin + m.headerWidth / 2.0 +
             static_cast<double>(c) * (m.headerWidth + m.columnGap);

    double headerTop = m.margin;
    if (const auto created = creatingMessageOf(c)) {
      headerTop = ys[*created] - m.headerHeight / 2.0;
    }
    line.header = {line.x - m.headerWidth / 2.0, headerTop, m.headerWidth,
                   m.headerHeight};
    line.lineTop = headerTop + m.headerHeight;
    line.lineBottom = lineBottom;
  }

  const std::vector<Activation> activations = computeActivations();
  std::vector<Rect> rects;
  rects.reserve(activations.size());
  for (const Activation& activation : activations) {
    LifelineLayout& line = out.lifelines[activation.participant];
    const double top =
        std::max(ys[activation.firstMessage] - kActivationPad, line.lineTop);
    const double extra =
        messages_[activation.lastMessage].isSelf() ? kSelfLoopHeight : 0.0;
    const double bottom =
        std::max(ys[activation.lastMessage] + extra + kActivationPad,
                 top + 2.0 * kActivationPad);
    const double x = line.x - m.activationWidth / 2.0 +
                     static_cast<double>(activation.depth) *
                         m.activationOverlap;
    const Rect rect{x, top, m.activationWidth, bottom - top};
    rects.push_back(rect);
    line.activations.push_back(rect);
  }

  auto barOf = [&](std::size_t participant, std::size_t i) {
    std::optional<std::size_t> best;
    for (std::size_t k = 0; k < activations.size(); ++k) {
      const Activation& a = activations[k];
      if (a.participant == participant && a.firstMessage <= i &&
          a.lastMessage >= i &&
          (!best || a.depth >= activations[*best].depth)) {
        best = k;
      }
    }
    if (best) {
      return rects[*best];
    }
    const double x = out.lifelines[participant].x;
    return Rect{x - m.activationWidth / 2.0, ys[i], m.activationWidth, 0.0};
  };

  // Messages.
  out.messages.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    const Message& message = messages_[i];
    const LifelineLayout& from = out.lifelines[message.from];
    const LifelineLayout& to = out.lifelines[message.to];

    MessageLayout item;
    item.index = i;

    if (message.isSelf()) {
      const Rect bar = barOf(message.from, i);
      const double right = bar.x + bar.w;
      item.start = {right, ys[i]};
      item.end = {right, ys[i] + kSelfLoopHeight};
      item.loopX = right + m.selfLoopWidth;
      item.label = {item.loopX + 8.0, ys[i] + kSelfLoopHeight / 2.0};
      item.toRight = true;
    } else {
      item.toRight = to.x > from.x;
      const Rect source = barOf(message.from, i);
      const double startX = item.toRight ? source.x + source.w : source.x;
      double endX = 0.0;
      if (message.kind == MessageKind::Create) {
        endX = item.toRight ? to.header.x : to.header.x + to.header.w;
      } else {
        const Rect target = barOf(message.to, i);
        endX = item.toRight ? target.x : target.x + target.w;
      }
      item.start = {startX, ys[i]};
      item.end = {endX, ys[i]};
      item.label = {(startX + endX) / 2.0, ys[i] - 6.0};
    }
    out.messages.push_back(item);
  }

  out.width = columns == 0
                  ? 2.0 * m.margin
                  : 2.0 * m.margin +
                        static_cast<double>(columns) * m.headerWidth +
                        static_cast<double>(columns - 1) * m.columnGap;
  out.height = lineBottom + m.margin;
  return out;
}

} // namespace seq
