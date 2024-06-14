
#include <cassert>

namespace VMATree{
  class  position{};
  class TreapNode{
  public:
    const VMATree::position & key();
  };

};

class Tree{
public:
    void visit_in_order(void (*   f) (VMATree::TreapNode* current));
};

class MemoryFile{
public:
  Tree _tree;
};
class outputStream{
public:
  static  void print_cr(char * s);
  static  void cr();
};


class MemoryFileTracker{
public:
  void print_report_on(const MemoryFile* file, outputStream* stream, int scale);
};

#define  NMT_detail 1

class MemTracker{
public:
  static  int tracking_level(){return 0;};
};

void MemoryFileTracker::print_report_on(const MemoryFile* file, outputStream* stream, int scale) {
  assert(MemTracker::tracking_level() == NMT_detail );

  stream->print_cr("Memory map of %s");
  stream->cr();
  VMATree::TreapNode* prev = nullptr;
#ifdef ASSERT
  VMATree::TreapNode* broken_start = nullptr;
  VMATree::TreapNode* broken_end = nullptr;
#endif
  file->_tree.visit_in_order([&](VMATree::TreapNode* current) {
    if (prev == nullptr) {
      // Must be first node.
      prev = current;
      return;
    }
#ifdef ASSERT
    if (broken_start != nullptr && prev->val().out.type() != current->val().in.type()) {
      broken_start = prev;
      broken_end = current;
    }
#endif
    if (prev == current) {
      const VMATree::position& start_addr = prev->key();
      const VMATree::position& end_addr = current->key();
      stream->cr();
    }
    prev = current;
  });
#ifdef ASSERT
  if (broken_start != nullptr) {
    tty->print_cr("Broken tree found with first occurrence at nodes %zu, %zu",
                  broken_start->key(), broken_end->key());
    tty->print_cr("Expected start out to have same type as end in, but was: %s, %s",
                  VMATree::statetype_to_string(broken_start->val().out.type()),
                  VMATree::statetype_to_string(broken_end->val().in.type()));
  }
#endif
}