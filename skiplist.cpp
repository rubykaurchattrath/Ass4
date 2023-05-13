#include "skiplist.h"
#include "random.h"
#include <cassert>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

using namespace std;

// SNode implementation

SNode::SNode(int val) : val(val) {}

SNode::SNode(const SNode &other) : val(other.val), next(other.next) {}

// SkipList implementation

SkipList::SkipList(int levels, int probability) : levels(levels), probability(probability) {
  head = new SNode();
  head->next.resize(levels, nullptr);
}

SkipList::SkipList(const SkipList &other) : levels(other.levels), probability(other.probability) {
  head = new SNode();
  head->next.resize(levels, nullptr);

  unordered_map<const SNode *, SNode *> nodeMap;

  SNode *cur = other.head->next[0];
  SNode *prev = head;
  while (cur != nullptr) {
    SNode *newNode = new SNode(*cur);
    prev->next[0] = newNode;
    nodeMap[cur] = newNode;

    cur = cur->next[0];
    prev = newNode;
  }

  for (int i = 1; i < levels; i++) {
    cur = other.head->next[i];
    prev = head;
    while (cur != nullptr) {
      SNode *newNode = new SNode(*cur);
      prev->next[i] = newNode;
      nodeMap[cur] = newNode;

      cur = cur->next[i];
      prev = newNode;
    }
  }

  for (int i = 0; i < levels; i++) {
    cur = head->next[i];
    while (cur != nullptr) {
      cur->next[i] = nodeMap[cur->next[i]];
      cur = cur->next[i];
    }
  }
}

SkipList::~SkipList() {
  SNode *cur = head;
  while (cur != nullptr) {
    SNode *next = cur->next[0];
    delete cur;
    cur = next;
  }
}

void SkipList::add(int val) {
  vector<SNode *> prevNodes = getBeforeNodes(val);

  SNode *newNode = new SNode(val);
  int newNodeLevels = 1;

  while (shouldInsertAtHigherLevel()) {
    newNodeLevels++;
  }

  newNode->next.resize(newNodeLevels, nullptr);

  for (int i = 0; i < newNodeLevels; i++) {
    newNode->next[i] = prevNodes[i]->next[i];
    prevNodes[i]->next[i] = newNode;
  }
}

void SkipList::add(const vector<int> &values) {
  for (int val : values) {
    add(val);
  }
}

bool SkipList::remove(int val) {
  vector<SNode *> prevNodes = getBeforeNodes(val);

  SNode *targetNode = prevNodes[0]->next[0];

  if (targetNode == nullptr || targetNode->val != val) {
    return false;
  }

  int targetNodeLevels = targetNode->next.size();

  for (int i = 0; i < targetNodeLevels; i++) {
    prevNodes[i]->next[i] = targetNode->next[i];
  }

  delete targetNode;

  return true;
}

bool SkipList::contains(int val) const {
  SNode *cur = head;

  for (int i = levels - 1; i >=







// checks if the value needs to be inserted at a higher level
bool SkipList::shouldInsertAtHigherLevel() const {
  return probability >= Random::random() % 100;
}

