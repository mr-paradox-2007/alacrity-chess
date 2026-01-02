#ifndef B_TREE_HPP
#define B_TREE_HPP

#include <vector>
#include <string>

struct GameRecord {
    std::string player1;
    std::string player2;
    std::string winner;
    long long timestamp;
    std::string moves;
};

template<typename K, typename V>
class BTree {
private:
    static const int ORDER = 4;
    
    struct Node {
        std::vector<K> keys;
        std::vector<V> values;
        std::vector<Node*> children;
        bool is_leaf;
        
        Node() : is_leaf(true) {}
    };
    
    Node* root;
    
public:
    BTree() { root = new Node(); }
    ~BTree() { DeleteNode(root); }
    
    void Insert(const K& key, const V& value);
    bool Find(const K& key, V& value) const;
    std::vector<V> FindRange(const K& key_start, const K& key_end) const;
    
private:
    void DeleteNode(Node* node);
    void InsertNonFull(Node* node, const K& key, const V& value);
    void SplitChild(Node* parent, int index);
    bool SearchNode(Node* node, const K& key, V& value) const;
    void RangeSearch(Node* node, const K& start, const K& end, std::vector<V>& results) const;
};

template<typename K, typename V>
void BTree<K, V>::Insert(const K& key, const V& value) {
    if (root->keys.size() >= 2 * ORDER - 1) {
        Node* new_root = new Node();
        new_root->is_leaf = false;
        new_root->children.push_back(root);
        SplitChild(new_root, 0);
        root = new_root;
    }
    InsertNonFull(root, key, value);
}

template<typename K, typename V>
void BTree<K, V>::InsertNonFull(Node* node, const K& key, const V& value) {
    int i = node->keys.size() - 1;
    
    if (node->is_leaf) {
        node->keys.push_back(K());
        node->values.push_back(V());
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->values[i + 1] = value;
    } else {
        while (i >= 0 && key < node->keys[i]) i--;
        i++;
        if (node->children[i]->keys.size() >= 2 * ORDER - 1) {
            SplitChild(node, i);
            if (key > node->keys[i]) i++;
        }
        InsertNonFull(node->children[i], key, value);
    }
}

template<typename K, typename V>
void BTree<K, V>::SplitChild(Node* parent, int index) {
    Node* child = parent->children[index];
    Node* new_child = new Node();
    new_child->is_leaf = child->is_leaf;
    
    int mid = ORDER - 1;
    for (int i = 0; i < mid; i++) {
        new_child->keys.push_back(child->keys[mid + i]);
        new_child->values.push_back(child->values[mid + i]);
    }
    
    if (!child->is_leaf) {
        for (int i = 0; i <= mid; i++) {
            new_child->children.push_back(child->children[mid + i]);
        }
        child->children.erase(child->children.begin() + mid, child->children.end());
    }
    
    child->keys.erase(child->keys.begin() + mid, child->keys.end());
    child->values.erase(child->values.begin() + mid, child->values.end());
    
    parent->children.insert(parent->children.begin() + index + 1, new_child);
    parent->keys.insert(parent->keys.begin() + index, child->keys[mid]);
    parent->values.insert(parent->values.begin() + index, child->values[mid]);
}

template<typename K, typename V>
bool BTree<K, V>::Find(const K& key, V& value) const {
    return SearchNode(root, key, value);
}

template<typename K, typename V>
bool BTree<K, V>::SearchNode(Node* node, const K& key, V& value) const {
    int i = 0;
    while (i < node->keys.size() && key > node->keys[i]) i++;
    
    if (i < node->keys.size() && key == node->keys[i]) {
        value = node->values[i];
        return true;
    }
    
    if (node->is_leaf) return false;
    return SearchNode(node->children[i], key, value);
}

template<typename K, typename V>
std::vector<V> BTree<K, V>::FindRange(const K& key_start, const K& key_end) const {
    std::vector<V> results;
    RangeSearch(root, key_start, key_end, results);
    return results;
}

template<typename K, typename V>
void BTree<K, V>::RangeSearch(Node* node, const K& start, const K& end, std::vector<V>& results) const {
    int i = 0;
    while (i < node->keys.size() && start > node->keys[i]) i++;
    
    while (i < node->keys.size() && node->keys[i] <= end) {
        if (!node->is_leaf) {
            RangeSearch(node->children[i], start, end, results);
        }
        if (node->keys[i] >= start && node->keys[i] <= end) {
            results.push_back(node->values[i]);
        }
        i++;
    }
    
    if (!node->is_leaf && i < node->children.size()) {
        RangeSearch(node->children[i], start, end, results);
    }
}

template<typename K, typename V>
void BTree<K, V>::DeleteNode(Node* node) {
    if (node) {
        if (!node->is_leaf) {
            for (auto child : node->children) {
                DeleteNode(child);
            }
        }
        delete node;
    }
}

#endif
