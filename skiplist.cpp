#include "skiplist.h"
#include "random.h"
#include <cassert>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

using namespace std;

// SNode constructor
SNode::SNode(int val) : val(val) {}

// SNode copy constructor
SNode::SNode(const SNode &other) : val(other.val), next(other.next) {}

// SkipList constructor
SkipList::SkipList(int levels, int probability)
    : levels(levels), probability(probability) {
  head = new SNode(INT_MIN);
  head->next.resize(levels, nullptr);
}


// SkipList copy constructor
SkipList::SkipList(const SkipList &other) : levels(other.levels), probability(other.probability) {
  head = new SNode(INT_MIN);
  head->next.resize(levels, nullptr);

  // Find the maximum level of the nodes in the original SkipList
  int maxLevel = 0;
  SNode *curr = other.head;
  while (curr) {
    maxLevel = max(maxLevel, (int)curr->next.size());
    curr = curr->next[0];
  }

  // Calculate the number of levels needed for the copied SkipList
  levels = min(maxLevel, levels);
  head->next.resize(levels, nullptr);

  // Add the nodes from the original SkipList to the copied SkipList
  curr = other.head->next[0];
  while (curr) {
    add(curr->val);
    curr = curr->next[0];
  }
}


// SkipList destructor
SkipList::~SkipList() {
  SNode *curr = head;
  while (curr) {
    SNode *next = curr->next[0];
    delete curr;
    curr = next;
  }
}

// Add a single value to the SkipList
void SkipList::add(int val) {
  vector<SNode *> beforeNodes = getBeforeNodes(val);
  SNode *newNode = new SNode(val);
  newNode->next.resize(levels, nullptr);

  for (int i = 0; i < levels; i++) {
    newNode->next[i] = beforeNodes[i]->next[i];
    beforeNodes[i]->next[i] = newNode;

    if (!shouldInsertAtHigherLevel()) {
      break;
    }
  }
} 


// Add multiple values to the SkipList
void SkipList::add(const vector<int> &values) {
  for (int val : values) {
    add(val);
  }
}

// Remove a value from the SkipList
bool SkipList::remove(int val) {
  vector<SNode *> beforeNodes = getBeforeNodes(val);

  SNode *target = beforeNodes[0]->next[0];

  if (!target || target->val != val) {
    return false; // Value not found
  }

  for (int i = 0; i < levels; i++) {
    if (beforeNodes[i]->next[i] != target) {
      break;
    }
    beforeNodes[i]->next[i] = target->next[i];
  }

  delete target;
  return true;
}

// Check if a value is present in the SkipList
bool SkipList::contains(int val) const {
  SNode *curr = head;
  for (int i = levels - 1; i >= 0; i--) {
    while (curr->next[i] && curr->next[i]->val < val) {
      curr = curr->next[i]; 
    }
  }
  return (curr->next[0] && curr->next[0]->val == val);
}

// Get the nodes before the insertion point for a given value
vector<SNode *> SkipList::getBeforeNodes(int val) const {
  vector<SNode *> beforeNodes(levels, nullptr);

  SNode *curr = head;
  for (int i = levels - 1; i >= 0; i--) {
    while (curr->next[i] && curr->next[i]->val < val) {
      curr = curr->next[i];
    }
    beforeNodes[i] = curr;
  }

  return beforeNodes;
}

// checks if the value needs to be inserted at a higher level
bool SkipList::shouldInsertAtHigherLevel() const {
return probability >= Random::random() % 100;
}

//ostream
ostream &operator<<(ostream &out, const SkipList &skip) {
  for (int level = skip.levels -1 ; level >= 0; level--) {
    out << "[level: " << level + 1 << "] ";

    // Traverse the list at the current level
    SNode *curr = skip.head->next[level];
    while (curr != nullptr) {
      out << curr->val << "-->";
      curr = curr->next[level];
  } 

  // Print nullptr to indicate the end of the list
  out << "nullptr\n";
  }

  return out;
}
