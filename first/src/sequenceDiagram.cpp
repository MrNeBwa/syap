#include "sequenceDiagram.h"

#include <QBrush>
#include <QFont>
#include <QFontMetricsF>
#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <QLineF>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>
#include <QResizeEvent>
#include <QSize>

#include <algorithm>
#include <utility>

namespace {

const QColor kBackground("#f4f4f4");
const QColor kLineColor("#333333");
const QColor kLifelineColor("#8a8a8a");
const QColor kActivationFill("#ffffff");
const QColor kLabelColor("#1a1a1a");

constexpr qreal kArrowSize = 10.0;
constexpr qreal kSceneMargin = 16.0;

// Stacking order: lifelines < activation bars < arrows < headers.
constexpr qreal kZLifeline = 0.0;
constexpr qreal kZActivation = 1.0;
constexpr qreal kZMessage = 2.0;
constexpr qreal kZHeader = 3.0;

QPointF toQt(const seq::Point& point) {
  return {point.x, point.y};
}

QRectF toQt(const seq::Rect& rect) {
  return {rect.x, rect.y, rect.w, rect.h};
}

QPolygonF filledArrowHead(const QPointF& tip, const QPointF& unit) {
  const QPointF normal(-unit.y(), unit.x());
  QPolygonF head;
  head << tip << tip - unit * kArrowSize + normal * (kArrowSize * 0.4)
       << tip - unit * kArrowSize - normal * (kArrowSize * 0.4);
  return head;
}

QPainterPath openArrowHead(const QPointF& tip, const QPointF& unit) {
  const QPointF normal(-unit.y(), unit.x());
  QPainterPath head;
  head.moveTo(tip - unit * kArrowSize + normal * (kArrowSize * 0.4));
  head.lineTo(tip);
  head.lineTo(tip - unit * kArrowSize - normal * (kArrowSize * 0.4));
  return head;
}

// A view that shrinks the scene to the widget's width (never enlarges it)
// and keeps the newest messages in sight: the diagram grows downwards.
class fitView : public QGraphicsView {
public:
  using QGraphicsView::QGraphicsView;

  void refit() {
    resetTransform();
    const QRectF area = sceneRect();
    const QSize port = viewport()->size();
    if (area.isEmpty() || port.isEmpty()) {
      return;
    }
    const qreal factor = std::min(1.0, port.width() / area.width());
    scale(factor, factor);
    centerOn(QPointF(area.center().x(), area.bottom()));
  }

protected:
  void resizeEvent(QResizeEvent* event) override {
    QGraphicsView::resizeEvent(event);
    refit();
  }
};

} // namespace

sequenceDiagram::sequenceDiagram()
    : scene_(std::make_unique<QGraphicsScene>()) {
  scene_->setBackgroundBrush(kBackground);

  view_ = new fitView(scene_.get());
  view_->setRenderHint(QPainter::Antialiasing);
  view_->setMinimumHeight(340);
  render();
}

sequenceDiagram::~sequenceDiagram() {
  // The view may outlive us (its parent widget owns it) or already be gone;
  // either way it must not keep pointing at a deleted scene.
  if (view_) {
    view_->setScene(nullptr);
  }
}

QWidget* sequenceDiagram::view() const noexcept {
  return view_.data();
}

const seq::SequenceModel& sequenceDiagram::model() const noexcept {
  return model_;
}

void sequenceDiagram::beginScenario() {
  model_.clear();
  render();
}

void sequenceDiagram::ensureParticipant(const QString& key,
                                        const QString& label,
                                        const QColor& color) {
  model_.ensureParticipant(key.toStdString(), label.toStdString(),
                           color.name().toStdString());
  render();
}

bool sequenceDiagram::addMessage(const QString& from, const QString& to,
                                 const QString& text, seq::MessageKind kind) {
  const bool added = model_.addMessage(from.toStdString(), to.toStdString(),
                                       text.toStdString(), kind);
  if (added) {
    render();
  }
  return added;
}

bool sequenceDiagram::addCall(const QString& from, const QString& to,
                              const QString& text, const QString& reply) {
  const bool added = model_.addCall(from.toStdString(), to.toStdString(),
                                    text.toStdString(), reply.toStdString());
  if (added) {
    render();
  }
  return added;
}

void sequenceDiagram::render() {
  scene_->clear();

  const seq::Layout layout = model_.layout();
  const auto& participants = model_.participants();
  const auto& messages = model_.messages();

  for (std::size_t i = 0; i < participants.size(); ++i) {
    drawLifeline(layout.lifelines[i]);
    drawActivations(layout.lifelines[i]);
  }
  for (const seq::MessageLayout& geometry : layout.messages) {
    drawMessage(messages[geometry.index], geometry);
  }
  for (std::size_t i = 0; i < participants.size(); ++i) {
    drawHeader(participants[i], layout.lifelines[i]);
  }

  // Cover the whole layout and anything that sticks out of it
  // (long labels next to a self loop on the right-most lifeline).
  const QRectF content(0.0, 0.0, layout.width, layout.height);
  scene_->setSceneRect(content.united(scene_->itemsBoundingRect())
                           .adjusted(-kSceneMargin, -kSceneMargin,
                                     kSceneMargin, kSceneMargin));
  if (view_) {
    static_cast<fitView*>(view_.data())->refit();
  }
}

void sequenceDiagram::drawLifeline(const seq::LifelineLayout& lifeline) {
  QPen pen(kLifelineColor);
  pen.setStyle(Qt::DashLine);
  pen.setWidthF(1.2);
  auto* line = scene_->addLine(
      QLineF(lifeline.x, lifeline.lineTop, lifeline.x, lifeline.lineBottom),
      pen);
  line->setZValue(kZLifeline);
}

void sequenceDiagram::drawActivations(const seq::LifelineLayout& lifeline) {
  const QPen pen(kLineColor, 1.4);
  for (const seq::Rect& bar : lifeline.activations) {
    auto* item = scene_->addRect(toQt(bar), pen, QBrush(kActivationFill));
    item->setZValue(kZActivation);
  }
}

void sequenceDiagram::drawHeader(const seq::Participant& participant,
                                 const seq::LifelineLayout& lifeline) {
  const QColor color(QString::fromStdString(participant.color));
  const QRectF box = toQt(lifeline.header);

  auto* rect = scene_->addRect(box, QPen(color.darker(130), 2), QBrush(color));
  rect->setZValue(kZHeader);

  QFont font;
  font.setPointSize(10);
  font.setBold(true);

  const QFontMetricsF metrics(font);
  const QString text = metrics.elidedText(
      QString::fromStdString(participant.label), Qt::ElideRight,
      box.width() - 12.0);

  auto* label = scene_->addSimpleText(text, font);
  label->setBrush(QColor("#ffffff"));
  label->setZValue(kZHeader + 1.0);
  const QRectF bounds = label->boundingRect();
  label->setPos(box.center().x() - bounds.width() / 2.0,
                box.center().y() - bounds.height() / 2.0);
}

void sequenceDiagram::drawMessage(const seq::Message& message,
                                  const seq::MessageLayout& geometry) {
  const QPointF start = toQt(geometry.start);
  const QPointF end = toQt(geometry.end);

  QPainterPath path;
  QPointF unit;
  path.moveTo(start);
  if (message.isSelf()) {
    path.lineTo(geometry.loopX, start.y());
    path.lineTo(geometry.loopX, end.y());
    path.lineTo(end);
    unit = QPointF(-1.0, 0.0); // the loop comes back from the right
  } else {
    path.lineTo(end);
    unit = QPointF(geometry.toRight ? 1.0 : -1.0, 0.0);
  }

  QPen pen(kLineColor, 1.6);
  pen.setJoinStyle(Qt::MiterJoin);
  if (message.kind == seq::MessageKind::Return) {
    pen.setStyle(Qt::DashLine);
  }
  auto* line = scene_->addPath(path, pen, QBrush(Qt::NoBrush));
  line->setZValue(kZMessage);

  if (message.kind == seq::MessageKind::Return) {
    QPen headPen(kLineColor, 1.6);
    auto* head = scene_->addPath(openArrowHead(end, unit), headPen,
                                QBrush(Qt::NoBrush));
    head->setZValue(kZMessage);
  } else {
    auto* head = scene_->addPolygon(filledArrowHead(end, unit), QPen(Qt::NoPen),
                                    QBrush(kLineColor));
    head->setZValue(kZMessage);
  }

  QFont font;
  font.setPointSize(9);
  const QString text = QString("%1: %2")
                           .arg(static_cast<qulonglong>(message.number))
                           .arg(QString::fromStdString(message.text));
  auto* label = scene_->addSimpleText(text, font);
  label->setBrush(kLabelColor);
  label->setZValue(kZMessage);

  const QRectF bounds = label->boundingRect();
  const QPointF anchor = toQt(geometry.label);
  if (message.isSelf()) {
    // Left-aligned, vertically centred next to the loop.
    label->setPos(anchor.x(), anchor.y() - bounds.height() / 2.0);
  } else {
    // Centred above the arrow.
    label->setPos(anchor.x() - bounds.width() / 2.0,
                  anchor.y() - bounds.height());
  }
}
