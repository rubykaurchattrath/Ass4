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
SkipList::SkipList(const SkipList &other)
    : levels(other.levels), probability(other.probability) {
  head = new SNode(INT_MIN);
  head->next.resize(levels, nullptr);

  unordered_map<SNode *, SNode *> nodeMap; // Map of original nodes to copied nodes

  // Copy the nodes at each level
  SNode *curr = other.head;
  while (curr) {
    SNode *newNode = new SNode(*curr);
    nodeMap[curr] = newNode;
    curr = curr->next[0];
  }

  // Connect the copied nodes at each level
  curr = other.head;
  while (curr) {
    SNode *newNode = nodeMap[curr];
    for (int i = 0; i < levels; i++) {
      if (curr->next[i]) {
        newNode->next[i] = nodeMap[curr->next[i]];
      }
    }
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

// Check if a node should be inserted at a higher level
bool SkipList::shouldInsertAtHigherLevel() const {
  return (randomChance() < probability);
}

/* // Print the contents of the SkipList
void SkipList::print() const {
  for (int i = levels - 1; i >= 0; i--) {
    SNode *curr = head->next[i];
    cout << "Level " << i << ": ";
    while (curr) {
      cout << curr->val << " ";
      curr = curr->next[i];
    }
    cout << endl;
  }
} */