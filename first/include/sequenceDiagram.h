#pragma once

#include "sequenceModel.h"

#include <QColor>
#include <QPointer>
#include <QString>

#include <memory>

class QGraphicsScene;
class QGraphicsView;
class QWidget;

// Draws a UML-style sequence diagram: participants across the top with
// dashed lifelines, numbered arrows between them, activation bars, dashed
// return arrows, self messages (loops) and object creation.
//
// All of the "what goes where" logic lives in seq::SequenceModel; this class
// only owns the Qt scene/view and turns the computed layout into items.
class sequenceDiagram {
public:
  sequenceDiagram();
  ~sequenceDiagram();

  sequenceDiagram(const sequenceDiagram&) = delete;
  sequenceDiagram& operator=(const sequenceDiagram&) = delete;

  // The widget to put into a layout. Ownership passes to the parent
  // widget once it is added to a layout.
  [[nodiscard]] QWidget* view() const noexcept;
  [[nodiscard]] const seq::SequenceModel& model() const noexcept;

  // Starts a new scenario: forgets all participants and messages.
  // Declare the participants of the scenario afterwards, left to right.
  void beginScenario();

  // Adds a participant, or updates label/colour of an existing one.
  void ensureParticipant(const QString& key, const QString& label,
                         const QColor& color);

  // Adds one arrow. Returns false (and draws nothing) for unknown
  // participants, empty text, or an invalid Create.
  bool addMessage(const QString& from, const QString& to, const QString& text,
                  seq::MessageKind kind = seq::MessageKind::Sync);

  // A call and (if `reply` is not empty and it is not a self call)
  // the dashed return arrow that answers it.
  bool addCall(const QString& from, const QString& to, const QString& text,
               const QString& reply = QString());

private:
  void render();
  void drawLifeline(const seq::LifelineLayout& lifeline);
  void drawActivations(const seq::LifelineLayout& lifeline);
  void drawHeader(const seq::Participant& participant,
                  const seq::LifelineLayout& lifeline);
  void drawMessage(const seq::Message& message,
                   const seq::MessageLayout& geometry);

  std::unique_ptr<QGraphicsScene> scene_;
  QPointer<QGraphicsView> view_;
  seq::SequenceModel model_;
};
