#ifndef B_TREE_HPP
#define B_TREE_HPP

#include <vector>
#include <algorithm>
#include <mutex>

template<typename K, typename V>
class b_tree {
private:
    struct node {
        std::vector<K> keys;
        std::vector<V> values;
        std::vector<node*> children;
        bool is_leaf;
        
        node() : is_leaf(true) {}
    };
    
    node* root;
    int order;
    mutable std::mutex mutex_lock;
    
    void insert_non_full(node* node_ptr, const K& key, const V& value);
    void split_child(node* parent, int index);
    void merge(node* parent, int index);
    void borrow_from_left(node* parent, int index);
    void borrow_from_right(node* parent, int index);
    void fill(node* node_ptr, int index);
    
    node* search_helper(node* node_ptr, const K& key) const;
    bool remove_helper(node* node_ptr, const K& key);
    void delete_tree(node* node_ptr);
    void range_helper(node* node_ptr, const K& start, const K& end,
                     std::vector<std::pair<K, V>>& results) const;
    
public:
    b_tree(int order_val = 5);
    ~b_tree();
    
    void insert(const K& key, const V& value);
    bool find(const K& key, V& value) const;
    bool remove(const K& key);
    
    void range_query(const K& start, const K& end, 
                     std::vector<std::pair<K, V>>& results) const;
    
    void clear();
};

template<typename K, typename V>
b_tree<K, V>::b_tree(int order_val) : order(order_val) {
    root = new node();
}

template<typename K, typename V>
b_tree<K, V>::~b_tree() {
    delete_tree(root);
}

template<typename K, typename V>
void b_tree<K, V>::delete_tree(node* node_ptr) {
    if (!node_ptr) return;
    if (!node_ptr->is_leaf) {
        for (auto child : node_ptr->children) delete_tree(child);
    }
    delete node_ptr;
}

template<typename K, typename V>
void b_tree<K, V>::insert(const K& key, const V& value) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    if (root->keys.size() >= 2 * order - 1) {
        node* new_root = new node();
        new_root->is_leaf = false;
        new_root->children.push_back(root);
        split_child(new_root, 0);
        root = new_root;
    }
    insert_non_full(root, key, value);
}

template<typename K, typename V>
void b_tree<K, V>::insert_non_full(node* node_ptr, const K& key, const V& value) {
    int i = node_ptr->keys.size() - 1;
    if (node_ptr->is_leaf) {
        node_ptr->keys.push_back(K());
        node_ptr->values.push_back(V());
        while (i >= 0 && key < node_ptr->keys[i]) {
            node_ptr->keys[i + 1] = node_ptr->keys[i];
            node_ptr->values[i + 1] = node_ptr->values[i];
            i--;
        }
        node_ptr->keys[i + 1] = key;
        node_ptr->values[i + 1] = value;
    } else {
        while (i >= 0 && key < node_ptr->keys[i]) i--;
        i++;
        if (node_ptr->children[i]->keys.size() >= 2 * order - 1) {
            split_child(node_ptr, i);
            if (key > node_ptr->keys[i]) i++;
        }
        insert_non_full(node_ptr->children[i], key, value);
    }
}

template<typename K, typename V>
void b_tree<K, V>::split_child(node* parent, int index) {
    node* full_child = parent->children[index];
    node* new_child = new node();
    new_child->is_leaf = full_child->is_leaf;
    int mid = order - 1;
    
    new_child->keys.assign(full_child->keys.begin() + mid + 1, full_child->keys.end());
    new_child->values.assign(full_child->values.begin() + mid + 1, full_child->values.end());
    if (!full_child->is_leaf) {
        new_child->children.assign(full_child->children.begin() + mid + 1, full_child->children.end());
    }
    
    full_child->keys.erase(full_child->keys.begin() + mid + 1, full_child->keys.end());
    full_child->values.erase(full_child->values.begin() + mid + 1, full_child->values.end());
    if (!full_child->is_leaf) {
        full_child->children.erase(full_child->children.begin() + mid + 1, full_child->children.end());
    }
    
    parent->children.insert(parent->children.begin() + index + 1, new_child);
    parent->keys.insert(parent->keys.begin() + index, full_child->keys[mid]);
    parent->values.insert(parent->values.begin() + index, full_child->values[mid]);
}

template<typename K, typename V>
bool b_tree<K, V>::find(const K& key, V& value) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    node* found_node = search_helper(root, key);
    if (!found_node) return false;
    auto it = std::find(found_node->keys.begin(), found_node->keys.end(), key);
    if (it != found_node->keys.end()) {
        int index = it - found_node->keys.begin();
        value = found_node->values[index];
        return true;
    }
    return false;
}

template<typename K, typename V>
typename b_tree<K, V>::node* b_tree<K, V>::search_helper(node* node_ptr, const K& key) const {
    int i = 0;
    while (i < node_ptr->keys.size() && key > node_ptr->keys[i]) i++;
    if (i < node_ptr->keys.size() && key == node_ptr->keys[i]) return node_ptr;
    if (node_ptr->is_leaf) return nullptr;
    return search_helper(node_ptr->children[i], key);
}

template<typename K, typename V>
void b_tree<K, V>::range_query(const K& start, const K& end, std::vector<std::pair<K, V>>& results) const {
    std::lock_guard<std::mutex> lock(mutex_lock);
    range_helper(root, start, end, results);
}

template<typename K, typename V>
void b_tree<K, V>::range_helper(node* node_ptr, const K& start, const K& end, std::vector<std::pair<K, V>>& results) const {
    int i = 0;
    while (i < node_ptr->keys.size()) {
        if (start <= node_ptr->keys[i] && node_ptr->keys[i] <= end) {
            results.push_back({node_ptr->keys[i], node_ptr->values[i]});
        } else if (node_ptr->keys[i] > end) break;
        i++;
    }
    if (!node_ptr->is_leaf) {
        i = 0;
        while (i <= node_ptr->children.size() - 1) {
            if (i < node_ptr->keys.size() && end >= node_ptr->keys[i]) {
                range_helper(node_ptr->children[i], start, end, results);
                i++;
            } else if (i == node_ptr->keys.size() || (i < node_ptr->keys.size() && start < node_ptr->keys[i])) {
                range_helper(node_ptr->children[i], start, end, results);
                break;
            } else break;
        }
    }
}

template<typename K, typename V>
bool b_tree<K, V>::remove(const K& key) {
    std::lock_guard<std::mutex> lock(mutex_lock);
    return remove_helper(root, key);
}

template<typename K, typename V>
bool b_tree<K, V>::remove_helper(node* node_ptr, const K& key) {
    int i = 0;
    while (i < node_ptr->keys.size() && key > node_ptr->keys[i]) i++;
    if (i < node_ptr->keys.size() && key == node_ptr->keys[i]) {
        if (node_ptr->is_leaf) {
            node_ptr->keys.erase(node_ptr->keys.begin() + i);
            node_ptr->values.erase(node_ptr->values.begin() + i);
            return true;
        }
        K pred = node_ptr->keys[i - 1];
        node_ptr->keys[i] = pred;
        return remove_helper(node_ptr->children[i - 1], pred);
    }
    if (node_ptr->is_leaf) return false;
    bool is_in_subtree = (i == node_ptr->keys.size());
    if (node_ptr->children[i]->keys.size() < order) {
        if (i != 0 && node_ptr->children[i - 1]->keys.size() >= order) {
            borrow_from_left(node_ptr, i);
        } else if (i != node_ptr->children.size() - 1 && node_ptr->children[i + 1]->keys.size() >= order) {
            borrow_from_right(node_ptr, i);
        } else {
            if (i != node_ptr->children.size() - 1) {
                merge(node_ptr, i);
            } else {
                merge(node_ptr, i - 1);
                i--;
            }
        }
    }
    if (is_in_subtree && i > node_ptr->keys.size()) i--;
    return remove_helper(node_ptr->children[i], key);
}

template<typename K, typename V>
void b_tree<K, V>::borrow_from_left(node* parent, int child_index) {
    node* child = parent->children[child_index];
    node* sibling = parent->children[child_index - 1];
    child->keys.insert(child->keys.begin(), parent->keys[child_index - 1]);
    child->values.insert(child->values.begin(), parent->values[child_index - 1]);
    parent->keys[child_index - 1] = sibling->keys.back();
    parent->values[child_index - 1] = sibling->values.back();
    sibling->keys.pop_back();
    sibling->values.pop_back();
    if (!child->is_leaf) {
        child->children.insert(child->children.begin(), sibling->children.back());
        sibling->children.pop_back();
    }
}

template<typename K, typename V>
void b_tree<K, V>::borrow_from_right(node* parent, int child_index) {
    node* child = parent->children[child_index];
    node* sibling = parent->children[child_index + 1];
    child->keys.push_back(parent->keys[child_index]);
    child->values.push_back(parent->values[child_index]);
    parent->keys[child_index] = sibling->keys.front();
    parent->values[child_index] = sibling->values.front();
    sibling->keys.erase(sibling->keys.begin());
    sibling->values.erase(sibling->values.begin());
    if (!child->is_leaf) {
        child->children.push_back(sibling->children.front());
        sibling->children.erase(sibling->children.begin());
    }
}

template<typename K, typename V>
void b_tree<K, V>::merge(node* parent, int index) {
    node* child = parent->children[index];
    node* sibling = parent->children[index + 1];
    child->keys.push_back(parent->keys[index]);
    child->values.push_back(parent->values[index]);
    child->keys.insert(child->keys.end(), sibling->keys.begin(), sibling->keys.end());
    child->values.insert(child->values.end(), sibling->values.begin(), sibling->values.end());
    if (!child->is_leaf) {
        child->children.insert(child->children.end(), sibling->children.begin(), sibling->children.end());
    }
    parent->keys.erase(parent->keys.begin() + index);
    parent->values.erase(parent->values.begin() + index);
    parent->children.erase(parent->children.begin() + index + 1);
    delete sibling;
}

template<typename K, typename V>
void b_tree<K, V>::clear() {
    std::lock_guard<std::mutex> lock(mutex_lock);
    delete_tree(root);
    root = new node();
}

#endif
