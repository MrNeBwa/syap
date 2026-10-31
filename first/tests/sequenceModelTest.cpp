#include "sequenceModel.h"

#include <cstdlib>
#include <iostream>

namespace {
int failures = 0;
#define CHECK(cond)                                                          \
  do {                                                                       \
    if (!(cond)) {                                                           \
      std::cerr << "FAIL line " << __LINE__ << ": " #cond "\n";              \
      ++failures;                                                            \
    }                                                                        \
  } while (0)
} // namespace

int main() {
  using namespace seq;

  // Scenario from the sketch (figure 57).
  SequenceModel m;
  m.ensureParticipant("admin", "Admin", "#d97a26");
  m.ensureParticipant("forum", "Forum", "#3f9a52");
  m.ensureParticipant("db", "userDB", "#2f6fbd");
  m.ensureParticipant("reg", "Registered", "#d97a26");

  CHECK(m.addCall("admin", "forum", "createMessage()", "message"));
  CHECK(m.addCall("admin", "forum", "getMessages()", "list"));
  CHECK(m.addCall("admin", "forum", "deleteMessage()", "ok"));
  CHECK(m.addMessage("admin", "admin", "changeLogin()"));
  CHECK(m.addMessage("admin", "db", "add()"));
  CHECK(m.addMessage("db", "db", "receive()"));
  CHECK(m.addMessage("db", "reg", "create", MessageKind::Create));
  CHECK(m.addMessage("db", "db", "store()"));
  CHECK(m.messages().size() == 11);
  CHECK(m.messages().back().number == 11);

  // Validation.
  CHECK(!m.addMessage("nobody", "db", "x"));
  CHECK(!m.addMessage("db", "db", ""));
  CHECK(!m.addMessage("admin", "reg", "again", MessageKind::Create));
  CHECK(!m.addMessage("db", "db", "self-create", MessageKind::Create));
  CHECK(m.messages().size() == 11);

  const Layout l = m.layout();
  CHECK(l.lifelines.size() == 4);
  CHECK(l.messages.size() == 11);

  // Messages go strictly top to bottom.
  for (std::size_t i = 1; i < l.messages.size(); ++i) {
    CHECK(l.messages[i].start.y > l.messages[i - 1].start.y);
  }
  // Columns go strictly left to right and do not overlap.
  for (std::size_t i = 1; i < l.lifelines.size(); ++i) {
    CHECK(l.lifelines[i].header.x >=
          l.lifelines[i - 1].header.x + l.lifelines[i - 1].header.w);
  }

  // Self message (#4): loop goes right, ends below the start.
  const MessageLayout& self = l.messages[6];
  CHECK(m.messages()[6].isSelf());
  CHECK(self.loopX > self.start.x);
  CHECK(self.end.y > self.start.y);
  CHECK(self.end.x == self.start.x);

  // Reply arrows point back (leftwards), calls point right.
  CHECK(l.messages[0].toRight);
  CHECK(!l.messages[1].toRight);

  // Created participant: header sits lower than the top row and
  // vertically centred on the create arrow; arrow stops at the header.
  const LifelineLayout& reg = l.lifelines[3];
  const MessageLayout& create = l.messages[9];
  CHECK(reg.header.y > l.lifelines[0].header.y);
  CHECK(reg.header.y + reg.header.h / 2.0 == create.start.y);
  CHECK(create.end.x == reg.header.x);
  CHECK(reg.lineTop == reg.header.y + reg.header.h);
  CHECK(m.creatingMessageOf(3).value() == 9);
  CHECK(!m.creatingMessageOf(0).has_value());

  // Activation bars exist where expected and lie inside the lifeline.
  CHECK(!l.lifelines[1].activations.empty()); // forum
  CHECK(!l.lifelines[2].activations.empty()); // db
  for (const LifelineLayout& line : l.lifelines) {
    for (const Rect& r : line.activations) {
      CHECK(r.h > 0);
      CHECK(r.y >= line.lineTop);
      CHECK(r.y + r.h <= line.lineBottom);
    }
  }
  // db: bar for message 5 spans through its own follow-ups (6..8),
  // plus nested bars for the two self messages.
  CHECK(l.lifelines[2].activations.size() == 3);
  // admin: nested bar for the self message sits to the right of the base.
  bool nested = false;
  for (const Rect& r : l.lifelines[0].activations) {
    if (r.x > l.lifelines[0].x - 6.0) {
      nested = true;
    }
  }
  CHECK(nested);


  // Every arrow starts and ends exactly on the edge of a block
  // (activation bar, or the header for Create), at a height the block covers.
  auto onEdgeOfBar = [](const LifelineLayout& line, const Point& p) {
    for (const Rect& r : line.activations) {
      const bool edge = p.x == r.x || p.x == r.x + r.w;
      if (edge && p.y >= r.y && p.y <= r.y + r.h) {
        return true;
      }
    }
    return false;
  };
  for (const MessageLayout& g : l.messages) {
    const Message& msg = m.messages()[g.index];
    CHECK(onEdgeOfBar(l.lifelines[msg.from], g.start));
    if (msg.isSelf()) {
      CHECK(onEdgeOfBar(l.lifelines[msg.from], g.end));
    } else if (msg.kind != MessageKind::Create) {
      CHECK(onEdgeOfBar(l.lifelines[msg.to], g.end));
    }
  }


  // A whole app session on one diagram (what the window builds up).
  SequenceModel session;
  session.ensureParticipant("user", "User: admin", "#d97a26");
  session.ensureParticipant("forum", "Forum: General", "#3f9a52");
  session.ensureParticipant("db", "userDB", "#2f6fbd");
  CHECK(session.addCall("user", "db", "login()", "ok"));
  CHECK(session.addCall("user", "forum", "createMessage()", "message"));
  CHECK(session.addCall("user", "forum", "getMessages()", "messages"));
  CHECK(session.addCall("user", "forum", "deleteMessage()", "removed"));
  CHECK(session.addMessage("user", "user", "changeLogin()"));
  for (const char* key : {"regUser:100", "regUser:101"}) {
    session.ensureParticipant(key, key, "#d97a26");
    CHECK(session.addMessage("user", "db", "addUser()"));
    CHECK(session.addMessage("db", "db", "receiveRequest()"));
    CHECK(session.addMessage("db", key, "createUser()", MessageKind::Create));
    CHECK(session.addMessage("db", "db", "addToDatabase()"));
  }
  CHECK(session.addCall("user", "db", "addUser()", "exists"));
  const Layout sl = session.layout();
  CHECK(sl.lifelines.size() == 5);
  for (std::size_t i = 1; i < sl.messages.size(); ++i) {
    CHECK(sl.messages[i].start.y > sl.messages[i - 1].start.y);
  }
  for (const MessageLayout& g : sl.messages) {
    const Message& msg = session.messages()[g.index];
    CHECK(onEdgeOfBar(sl.lifelines[msg.from], g.start));
    if (msg.kind != MessageKind::Create) {
      CHECK(onEdgeOfBar(sl.lifelines[msg.to], g.end));
    }
  }
  for (const LifelineLayout& line : sl.lifelines) {
    for (const Rect& r : line.activations) {
      CHECK(r.h > 0 && r.y >= line.lineTop && r.y + r.h <= line.lineBottom);
    }
  }

  // Edge cases: empty model, one participant, clear.
  SequenceModel empty;
  const Layout e = empty.layout();
  CHECK(e.lifelines.empty() && e.messages.empty() && e.height > 0);

  SequenceModel solo;
  solo.ensureParticipant("a", "A", "#000000");
  CHECK(solo.addMessage("a", "a", "recursive()"));
  CHECK(solo.addMessage("a", "a", "recursive()"));
  const Layout s = solo.layout();
  CHECK(s.messages.size() == 2);
  CHECK(s.messages[1].start.y > s.messages[0].end.y);
  CHECK(s.lifelines[0].activations.size() == 4); // base + nested, twice
  CHECK(onEdgeOfBar(s.lifelines[0], s.messages[0].start));
  CHECK(onEdgeOfBar(s.lifelines[0], s.messages[1].end));

  // ensureParticipant is idempotent and updates the label.
  CHECK(m.ensureParticipant("forum", "Forum 2", "#111111") == 1);
  CHECK(m.participants()[1].label == "Forum 2");
  CHECK(m.participants().size() == 4);

  m.clearMessages();
  CHECK(m.messages().empty() && m.participants().size() == 4);
  m.clear();
  CHECK(m.participants().empty());

  if (failures == 0) {
    std::cout << "all sequence model tests passed\n";
  }
  return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
