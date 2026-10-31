#pragma once
#include "sequenceModel.h"
#include <QColor>
#include <QPointer>
#include <QString>
#include <memory>

class QGraphicsScene;
class QGraphicsView;
class QWidget;

class sequenceDiagram {
public:
  sequenceDiagram();
  ~sequenceDiagram();

  sequenceDiagram(const sequenceDiagram&) = delete;
  sequenceDiagram& operator=(const sequenceDiagram&) = delete;

  [[nodiscard]] QWidget* view() const noexcept;
  [[nodiscard]] const seq::SequenceModel& model() const noexcept;

  void beginScenario();

  void ensureParticipant(const QString& key, const QString& label,
                         const QColor& color);

  bool addMessage(const QString& from, const QString& to, const QString& text,
                  seq::MessageKind kind = seq::MessageKind::Sync);
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
