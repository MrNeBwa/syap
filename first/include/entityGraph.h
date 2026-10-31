#pragma once

#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QString>

#include <map>
#include <vector>

class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsRectItem;
class QGraphicsSimpleTextItem;
class QWidget;
class edgeItem;

class entityGraph {
public:
  entityGraph();
  ~entityGraph();

  entityGraph(const entityGraph&) = delete;
  entityGraph& operator=(const entityGraph&) = delete;

  [[nodiscard]] QWidget* view() const noexcept;
  [[nodiscard]] QGraphicsScene* scene() const noexcept;

  void ensureEntity(const QString& key, const QString& label,
                    const QColor& color);

  void beginAction();
  void addRelation(const QString& from, const QString& to,
                   const QString& method);
  void addPersistentRelation(const QString& from, const QString& to,
                             const QString& method);

  [[nodiscard]] QRectF rectOf(const QString& key) const;
  [[nodiscard]] std::vector<QRectF> allNodeRects() const;

private:
  struct Node {
    QGraphicsRectItem* rect = nullptr;
    QGraphicsSimpleTextItem* label = nullptr;
  };

  struct Relation {
    QString from;
    QString to;
    QString method;
  };

  void relayout();
  void rebuildEdges();
  void refreshEdges();

  QGraphicsScene* scene_ = nullptr;
  QGraphicsView* view_ = nullptr;
  std::map<QString, Node> nodes_;
  std::vector<QString> order_;
  std::vector<Relation> relations_;
  std::vector<Relation> persistentRelations_;
  std::vector<edgeItem*> edgeItems_;
};
