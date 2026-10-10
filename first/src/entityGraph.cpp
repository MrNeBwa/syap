#include "entityGraph.h"

#include <QBrush>
#include <QFont>
#include <QFontMetricsF>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPolygonF>
#include <QVariant>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <utility>

namespace {

constexpr qreal kNodeWidth = 170.0;
constexpr qreal kNodeHeight = 60.0;
constexpr qreal kGapX = 60.0;
constexpr qreal kGapY = 70.0;
constexpr std::size_t kColumns = 3;
constexpr qreal kMargin = 30.0;
constexpr qreal kArrowSize = 11.0;
constexpr qreal kSelfLoop = 55.0;
constexpr qreal kRouteMargin = 10.0;
constexpr qreal kEps = 0.5;

bool nearlyEqual(qreal a, qreal b) {
  return std::abs(a - b) < kEps;
}

void uniqueSorted(std::vector<qreal>& values) {
  std::sort(values.begin(), values.end());
  std::vector<qreal> out;
  for (const qreal value : values) {
    if (out.empty() || !nearlyEqual(value, out.back())) {
      out.push_back(value);
    }
  }
  values.swap(out);
}

int indexOf(const std::vector<qreal>& values, qreal target) {
  int best = -1;
  qreal bestDistance = std::numeric_limits<qreal>::max();
  for (std::size_t i = 0; i < values.size(); ++i) {
    const qreal distance = std::abs(values[i] - target);
    if (distance < bestDistance) {
      bestDistance = distance;
      best = static_cast<int>(i);
    }
  }
  return best;
}

bool segmentHitsRect(const QPointF& a, const QPointF& b, const QRectF& rect) {
  const QRectF inner = rect.adjusted(kEps, kEps, -kEps, -kEps);
  if (inner.isEmpty()) {
    return false;
  }

  if (nearlyEqual(a.y(), b.y())) {
    const qreal y = a.y();
    if (y <= inner.top() || y >= inner.bottom()) {
      return false;
    }
    const qreal x1 = std::min(a.x(), b.x());
    const qreal x2 = std::max(a.x(), b.x());
    return x2 > inner.left() && x1 < inner.right();
  }

  if (nearlyEqual(a.x(), b.x())) {
    const qreal x = a.x();
    if (x <= inner.left() || x >= inner.right()) {
      return false;
    }
    const qreal y1 = std::min(a.y(), b.y());
    const qreal y2 = std::max(a.y(), b.y());
    return y2 > inner.top() && y1 < inner.bottom();
  }

  return false;
}

QPolygonF makeArrowHead(const QPointF& tip, const QPointF& unit) {
  const QPointF normal(-unit.y(), unit.x());
  QPolygonF head;
  head << tip
       << tip - unit * kArrowSize + normal * (kArrowSize * 0.45)
       << tip - unit * kArrowSize - normal * (kArrowSize * 0.45);
  return head;
}

QPointF clipToBorder(const QPointF& inside, const QPointF& outside,
                     const QRectF& rect) {
  if (nearlyEqual(outside.y(), inside.y())) {
    return QPointF(outside.x() > inside.x() ? rect.right() : rect.left(),
                   inside.y());
  }
  return QPointF(inside.x(),
                 outside.y() > inside.y() ? rect.bottom() : rect.top());
}

void simplify(std::vector<QPointF>& points) {
  std::vector<QPointF> unique;
  for (const QPointF& point : points) {
    if (unique.empty() || !nearlyEqual(point.x(), unique.back().x()) ||
        !nearlyEqual(point.y(), unique.back().y())) {
      unique.push_back(point);
    }
  }

  std::vector<QPointF> out;
  for (std::size_t i = 0; i < unique.size(); ++i) {
    if (i > 0 && i + 1 < unique.size()) {
      const bool sameX = nearlyEqual(unique[i - 1].x(), unique[i].x()) &&
                         nearlyEqual(unique[i].x(), unique[i + 1].x());
      const bool sameY = nearlyEqual(unique[i - 1].y(), unique[i].y()) &&
                         nearlyEqual(unique[i].y(), unique[i + 1].y());
      if (sameX || sameY) {
        continue;
      }
    }
    out.push_back(unique[i]);
  }
  points.swap(out);
}

QGraphicsSimpleTextItem* makeLabel(const QString& text, const QColor& color) {
  auto* label = new QGraphicsSimpleTextItem(text);
  QFont font = label->font();
  font.setPointSize(9);
  label->setFont(font);
  label->setBrush(color);
  return label;
}

void centerLabel(QGraphicsRectItem* rect, QGraphicsSimpleTextItem* label) {
  const QRectF bounds = label->boundingRect();
  label->setPos((rect->rect().width() - bounds.width()) / 2.0,
                (rect->rect().height() - bounds.height()) / 2.0);
}

class movableRectItem : public QGraphicsRectItem {
public:
  using QGraphicsRectItem::QGraphicsRectItem;
  std::function<void()> onMoved;

protected:
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant& value) override {
    if (change == ItemPositionHasChanged && onMoved) {
      onMoved();
    }
    return QGraphicsRectItem::itemChange(change, value);
  }
};

} // namespace

class edgeItem : public QGraphicsItem {
public:
  edgeItem(entityGraph* graph, QString from, QString to, QString method,
           int parallelIndex = 0, int parallelCount = 1)
      : graph_(graph), from_(std::move(from)), to_(std::move(to)),
        method_(std::move(method)), parallelIndex_(parallelIndex),
        parallelCount_(parallelCount) {
    labelFont_.setPointSize(9);
    recompute();
  }

  [[nodiscard]] QRectF boundingRect() const override {
    return bounds_;
  }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem*,
             QWidget*) override {
    if (path_.isEmpty()) {
      return;
    }
    painter->setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QColor("#333333"));
    pen.setWidthF(2.0);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path_);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#333333"));
    painter->drawPolygon(arrow_);

    painter->setFont(labelFont_);
    const QFontMetricsF metrics(labelFont_);
    const QRectF textBounds = metrics.boundingRect(method_);
    const QRectF box(labelPos_.x() - textBounds.width() / 2.0 - 4.0,
                     labelPos_.y() - textBounds.height() / 2.0 - 2.0,
                     textBounds.width() + 8.0, textBounds.height() + 4.0);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(244, 244, 244, 225));
    painter->drawRoundedRect(box, 3.0, 3.0);

    painter->setPen(QColor("#1a1a1a"));
    painter->drawText(box, Qt::AlignCenter, method_);
  }

  void refresh() {
    prepareGeometryChange();
    recompute();
    update();
  }

private:
  void recompute() {
    path_ = QPainterPath();
    arrow_ = QPolygonF();

    const QRectF fromRect = graph_->rectOf(from_);
    const QRectF toRect = graph_->rectOf(to_);

    if (from_ == to_) {
      buildSelfLoop(fromRect);
    } else {
      const std::vector<QRectF> allRects = graph_->allNodeRects();
      std::vector<QRectF> obstacles;
      for (const QRectF& rect : allRects) {
        if (rect == fromRect || rect == toRect) {
          continue;
        }
        obstacles.push_back(rect.adjusted(-kRouteMargin, -kRouteMargin,
                                          kRouteMargin, kRouteMargin));
      }

      std::vector<QPointF> points = route(fromRect, toRect, allRects, obstacles);
      if (points.size() >= 2) {
        points.front() = clipToBorder(points.front(), points[1], fromRect);
        const std::size_t last = points.size() - 1;
        points.back() = clipToBorder(points[last], points[last - 1], toRect);
      }
      simplify(points);
      buildPath(points);
    }

    applyParallelOffset(fromRect, toRect);

    labelPos_ = path_.pointAtPercent(0.5);
    const QFontMetricsF metrics(labelFont_);
    const QRectF textBounds = metrics.boundingRect(method_);
    const QRectF labelBox(labelPos_.x() - textBounds.width() / 2.0 - 6.0,
                          labelPos_.y() - textBounds.height() / 2.0 - 4.0,
                          textBounds.width() + 12.0,
                          textBounds.height() + 8.0);
    bounds_ = path_.boundingRect()
                  .united(labelBox)
                  .united(arrow_.boundingRect())
                  .adjusted(-6.0, -6.0, 6.0, 6.0);
  }

  void applyParallelOffset(const QRectF& fromRect, const QRectF& toRect) {
    if (parallelCount_ <= 1) {
      return;
    }

    const qreal offset =
        (static_cast<qreal>(parallelIndex_) -
         (static_cast<qreal>(parallelCount_) - 1.0) / 2.0) *
        16.0;

    QPointF shift;
    if (from_ == to_) {
      shift = QPointF(offset, 0.0);
    } else {
      const QPointF fromCenter = fromRect.center();
      const QPointF toCenter = toRect.center();
      if (std::abs(toCenter.x() - fromCenter.x()) >=
          std::abs(toCenter.y() - fromCenter.y())) {
        shift = QPointF(0.0, offset);
      } else {
        shift = QPointF(offset, 0.0);
      }
    }

    path_.translate(shift.x(), shift.y());
    for (int i = 0; i < arrow_.size(); ++i) {
      arrow_[i] += shift;
    }
  }


  void buildPath(const std::vector<QPointF>& points) {
    if (points.size() < 2) {
      return;
    }
    path_.moveTo(points.front());
    for (std::size_t i = 1; i < points.size(); ++i) {
      path_.lineTo(points[i]);
    }
    const QPointF tip = points.back();
    const QPointF delta = tip - points[points.size() - 2];
    const qreal length = std::hypot(delta.x(), delta.y());
    if (length > 1e-6) {
      arrow_ = makeArrowHead(tip, delta / length);
    }
  }

  void buildSelfLoop(const QRectF& rect) {
    const QPointF start(rect.center().x() - 30.0, rect.top());
    const QPointF end(rect.center().x() + 30.0, rect.top());
    path_.moveTo(start);
    path_.cubicTo(QPointF(start.x() - 15.0, rect.top() - kSelfLoop),
                  QPointF(end.x() + 15.0, rect.top() - kSelfLoop), end);
    arrow_ = makeArrowHead(end, QPointF(0.0, 1.0));
  }

  std::vector<QPointF> route(const QRectF& fromRect, const QRectF& toRect,
                             const std::vector<QRectF>& allRects,
                             const std::vector<QRectF>& obstacles) const {
    std::vector<qreal> xs;
    std::vector<qreal> ys;
    for (const QRectF& rect : allRects) {
      xs.push_back(rect.left() - kRouteMargin);
      xs.push_back(rect.left());
      xs.push_back(rect.center().x());
      xs.push_back(rect.right());
      xs.push_back(rect.right() + kRouteMargin);
      ys.push_back(rect.top() - kRouteMargin);
      ys.push_back(rect.top());
      ys.push_back(rect.center().y());
      ys.push_back(rect.bottom());
      ys.push_back(rect.bottom() + kRouteMargin);
    }
    xs.push_back(fromRect.center().x());
    ys.push_back(fromRect.center().y());
    xs.push_back(toRect.center().x());
    ys.push_back(toRect.center().y());
    uniqueSorted(xs);
    uniqueSorted(ys);

    const int nx = static_cast<int>(xs.size());
    const int ny = static_cast<int>(ys.size());
    auto nodeIndex = [ny](int i, int j) { return i * ny + j; };
    auto pointAt = [&xs, &ys](int i, int j) {
      return QPointF(xs[static_cast<std::size_t>(i)],
                     ys[static_cast<std::size_t>(j)]);
    };

    const int si = indexOf(xs, fromRect.center().x());
    const int sj = indexOf(ys, fromRect.center().y());
    const int gi = indexOf(xs, toRect.center().x());
    const int gj = indexOf(ys, toRect.center().y());
    const int start = nodeIndex(si, sj);
    const int goal = nodeIndex(gi, gj);

    const double infinity = std::numeric_limits<double>::infinity();
    std::vector<double> best(nx * ny, infinity);
    std::vector<int> previous(nx * ny, -1);

    auto heuristic = [&](int i, int j) {
      return std::abs(xs[i] - xs[gi]) + std::abs(ys[j] - ys[gj]);
    };

    using QueueEntry = std::pair<double, int>;
    std::priority_queue<QueueEntry, std::vector<QueueEntry>,
                        std::greater<QueueEntry>>
        queue;
    best[start] = 0.0;
    queue.push({heuristic(si, sj), start});

    const int di[4] = {1, -1, 0, 0};
    const int dj[4] = {0, 0, 1, -1};

    while (!queue.empty()) {
      const int current = queue.top().second;
      queue.pop();
      if (current == goal) {
        break;
      }

      const int i = current / ny;
      const int j = current % ny;
      for (int d = 0; d < 4; ++d) {
        const int ni = i + di[d];
        const int nj = j + dj[d];
        if (ni < 0 || nj < 0 || ni >= nx || nj >= ny) {
          continue;
        }

        const QPointF a = pointAt(i, j);
        const QPointF b = pointAt(ni, nj);
        bool blocked = false;
        for (const QRectF& obstacle : obstacles) {
          if (segmentHitsRect(a, b, obstacle)) {
            blocked = true;
            break;
          }
        }
        if (blocked) {
          continue;
        }

        const double cost = std::abs(xs[ni] - xs[i]) + std::abs(ys[nj] - ys[j]);
        const int next = nodeIndex(ni, nj);
        if (best[current] + cost < best[next] - 1e-9) {
          best[next] = best[current] + cost;
          previous[next] = current;
          queue.push({best[next] + heuristic(ni, nj), next});
        }
      }
    }

    if (previous[goal] == -1 && start != goal) {
      return {pointAt(si, sj), pointAt(gi, gj)};
    }

    std::vector<QPointF> points;
    for (int current = goal; current != -1; current = previous[current]) {
      points.push_back(pointAt(current / ny, current % ny));
    }
    std::reverse(points.begin(), points.end());
    return points;
  }

  entityGraph* graph_ = nullptr;
  QString from_;
  QString to_;
  QString method_;
  int parallelIndex_ = 0;
  int parallelCount_ = 1;
  QFont labelFont_;
  QPainterPath path_;
  QPolygonF arrow_;
  QPointF labelPos_;
  QRectF bounds_;
};

entityGraph::entityGraph() {
  scene_ = new QGraphicsScene();
  scene_->setBackgroundBrush(QColor("#f4f4f4"));

  view_ = new QGraphicsView(scene_);
  view_->setRenderHint(QPainter::Antialiasing);
  view_->setMinimumHeight(260);
}

entityGraph::~entityGraph() {
  if (view_) {
    view_->setScene(nullptr);
  }
  delete scene_;
}

QWidget* entityGraph::view() const noexcept {
  return view_;
}

QGraphicsScene* entityGraph::scene() const noexcept {
  return scene_;
}

void entityGraph::ensureEntity(const QString& key, const QString& label,
                               const QColor& color) {
  auto it = nodes_.find(key);
  if (it != nodes_.end()) {
    it->second.label->setText(label);
    centerLabel(it->second.rect, it->second.label);
    it->second.rect->setBrush(color);
    it->second.rect->setPen(QPen(color.darker(130), 2));
    return;
  }

  auto* rect = new movableRectItem(0, 0, kNodeWidth, kNodeHeight);
  rect->setFlag(QGraphicsItem::ItemIsMovable);
  rect->setBrush(color);
  rect->setPen(QPen(color.darker(130), 2));
  rect->setZValue(1);
  rect->onMoved = [this]() { refreshEdges(); };
  scene_->addItem(rect);

  auto* text = makeLabel(label, QColor("#ffffff"));
  text->setParentItem(rect);
  QFont font = text->font();
  font.setPointSize(10);
  font.setBold(true);
  text->setFont(font);
  text->setZValue(2);
  centerLabel(rect, text);

  nodes_.emplace(key, Node{rect, text});
  order_.push_back(key);
  relayout();
}

void entityGraph::beginAction() {
  relations_.clear();
  rebuildEdges();
}

void entityGraph::addRelation(const QString& from, const QString& to,
                              const QString& method) {
  relations_.push_back({from, to, method});
  rebuildEdges();
}

void entityGraph::addPersistentRelation(const QString& from, const QString& to,
                                        const QString& method) {
  for (const Relation& relation : persistentRelations_) {
    if (relation.from == from && relation.to == to &&
        relation.method == method) {
      return;
    }
  }
  persistentRelations_.push_back({from, to, method});
  rebuildEdges();
}

QRectF entityGraph::rectOf(const QString& key) const {
  auto it = nodes_.find(key);
  if (it == nodes_.end()) {
    return {};
  }
  return it->second.rect->sceneBoundingRect();
}

std::vector<QRectF> entityGraph::allNodeRects() const {
  std::vector<QRectF> rects;
  rects.reserve(nodes_.size());
  for (const QString& key : order_) {
    auto it = nodes_.find(key);
    if (it != nodes_.end()) {
      rects.push_back(it->second.rect->sceneBoundingRect());
    }
  }
  return rects;
}

void entityGraph::relayout() {
  qreal maxX = kMargin;
  qreal maxY = kMargin;

  for (std::size_t i = 0; i < order_.size(); ++i) {
    const int col = static_cast<int>(i % kColumns);
    const int row = static_cast<int>(i / kColumns);
    const qreal x = kMargin + static_cast<qreal>(col) * (kNodeWidth + kGapX);
    const qreal y = kMargin + static_cast<qreal>(row) * (kNodeHeight + kGapY);

    auto it = nodes_.find(order_[i]);
    if (it != nodes_.end()) {
      it->second.rect->setPos(x, y);
    }
    maxX = std::max(maxX, x + kNodeWidth);
    maxY = std::max(maxY, y + kNodeHeight);
  }

  scene_->setSceneRect(0, 0, maxX + kMargin, maxY + kMargin);
  refreshEdges();
}

void entityGraph::rebuildEdges() {
  for (edgeItem* item : edgeItems_) {
    scene_->removeItem(item);
    delete item;
  }
  edgeItems_.clear();

  std::vector<Relation> all;
  all.reserve(persistentRelations_.size() + relations_.size());
  all.insert(all.end(), persistentRelations_.begin(), persistentRelations_.end());
  all.insert(all.end(), relations_.begin(), relations_.end());

  auto pairKey = [](const Relation& relation) {
    const QString& first =
        relation.from <= relation.to ? relation.from : relation.to;
    const QString& second =
        relation.from <= relation.to ? relation.to : relation.from;
    return first + QChar(0x1f) + second;
  };

  std::map<QString, int> totals;
  for (const Relation& relation : all) {
    ++totals[pairKey(relation)];
  }

  std::map<QString, int> seen;
  for (const Relation& relation : all) {
    if (nodes_.find(relation.from) == nodes_.end() ||
        nodes_.find(relation.to) == nodes_.end()) {
      continue;
    }
    const QString key = pairKey(relation);
    const int index = seen[key]++;
    auto* item = new edgeItem(this, relation.from, relation.to,
                              relation.method, index, totals[key]);
    scene_->addItem(item);
    edgeItems_.push_back(item);
  }
}

void entityGraph::refreshEdges() {
  for (edgeItem* item : edgeItems_) {
    item->refresh();
  }
  scene_->update();
}
