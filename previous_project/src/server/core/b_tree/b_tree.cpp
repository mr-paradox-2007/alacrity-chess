#include <b_tree/b_tree.hpp>
#include <fstream>
#include <algorithm>
#include <functional>

namespace ds
{

b_tree::b_tree() : root_(nullptr), count_(0)
{
    root_ = new b_node(true);
}

b_tree::~b_tree()
{
    clear_node(root_);
}

bool b_tree::insert(const game_record& record)
{
    std::lock_guard<std::mutex> lock(lock_);

    if(search_helper(root_, record) != nullptr)
    {
        return false;
    }

    if(root_->keys_.size() == 2 * b_order_ - 1)
    {
        b_node* old_root = root_;
        root_ = new b_node(false);
        root_->children_.push_back(old_root);
        split_child(root_, 0);
    }

    insert_non_full(root_, record);
    ++count_;
    return true;
}

void b_tree::split_child(b_node* parent, int index)
{
    b_node* left_child = parent->children_[index];
    b_node* right_child = new b_node(left_child->is_leaf_);

    int mid_point = b_order_ - 1;

    // Extract middle key before modifying left child
    game_record middle_key = left_child->keys_[mid_point];

    right_child->keys_.assign(left_child->keys_.begin() + mid_point + 1, left_child->keys_.end());
    left_child->keys_.erase(left_child->keys_.begin() + mid_point, left_child->keys_.end());

    if(!left_child->is_leaf_)
    {
        right_child->children_.assign(left_child->children_.begin() + mid_point + 1,
                                      left_child->children_.end());
        left_child->children_.erase(left_child->children_.begin() + mid_point + 1,
                                    left_child->children_.end());
    }

    parent->keys_.insert(parent->keys_.begin() + index, middle_key);
    parent->children_.insert(parent->children_.begin() + index + 1, right_child);
}

void b_tree::insert_non_full(b_node* node, const game_record& record)
{
    int i = node->keys_.size() - 1;

    if(node->is_leaf_)
    {
        node->keys_.push_back(record);
        std::sort(node->keys_.begin(), node->keys_.end());
    }
    else
    {
        // Find the child to insert into
        while(i >= 0 && record < node->keys_[i])
        {
            --i;
        }
        ++i;

        // Check if child is full
        if((int)node->children_[i]->keys_.size() == 2 * b_order_ - 1)
        {
            split_child(node, i);
            // After split, compare record with new key at index i
            if(record > node->keys_[i])
            {
                ++i;
            }
        }

        insert_non_full(node->children_[i], record);
    }
}


void b_tree::remove_helper(b_node* node, const game_record& record)
{
    int index = find_key(node, record);

    if (index < (int)node->keys_.size() && node->keys_[index] == record)
    {
        if (node->is_leaf_)
        {
            node->keys_.erase(node->keys_.begin() + index);
        }
        else
        {
            b_node* left_child = node->children_[index];
            if ((int)left_child->keys_.size() >= b_order_)
            {
                game_record predecessor = left_child->keys_.back();
                node->keys_[index] = predecessor;
                remove_helper(left_child, predecessor);
            }
            else
            {
                b_node* right_child = node->children_[index + 1];
                if ((int)right_child->keys_.size() >= b_order_)
                {
                    game_record successor = right_child->keys_.front();
                    node->keys_[index] = successor;
                    remove_helper(right_child, successor);
                }
                else
                {
                    merge(node, index);
                    remove_helper(left_child, record);
                }
            }
        }
    }
    else
    {
        if (node->is_leaf_)
        {
            return;
        }

        bool flag = (index == (int)node->keys_.size());
        b_node* child = node->children_[index];

        if ((int)child->keys_.size() < b_order_)
        {
            if (index != 0 && (int)node->children_[index - 1]->keys_.size() >= b_order_)
            {
                borrow_from_prev(node, index);
            }
            else if (index != (int)node->keys_.size() && (int)node->children_[index + 1]->keys_.size() >= b_order_)
            {
                borrow_from_next(node, index);
            }
            else
            {
                if (index != (int)node->keys_.size())
                {
                    merge(node, index);
                }
                else
                {
                    merge(node, index - 1);
                }
            }
        }
        
        if (flag && index > (int)node->keys_.size())
        {
             remove_helper(node->children_[index-1], record);
        }
        else
        {
             remove_helper(node->children_[index], record);
        }
    }
}

bool b_tree::remove(const game_record& record)
{
    std::lock_guard<std::mutex> lock(lock_);

    if(search_helper(root_, record) == nullptr)
        return false;

    remove_helper(root_, record);
    count_--;

    if (root_->keys_.empty() && !root_->is_leaf_)
    {
        b_node* old_root = root_;
        root_ = root_->children_[0];
        old_root->children_.clear();
        delete old_root;
    }
    
    return true;
}

int b_tree::find_key(b_node* node, const game_record& record) const
{
    int i = 0;
    while(i < (int)node->keys_.size() && record > node->keys_[i])
    {
        ++i;
    }
    return i;
}

void b_tree::merge(b_node* parent, int index)
{
    b_node* left_child = parent->children_[index];
    b_node* right_child = parent->children_[index + 1];

    left_child->keys_.push_back(parent->keys_[index]);
    left_child->keys_.insert(left_child->keys_.end(), right_child->keys_.begin(),
                             right_child->keys_.end());

    if(!left_child->is_leaf_)
    {
        left_child->children_.insert(left_child->children_.end(), right_child->children_.begin(),
                                     right_child->children_.end());
    }

    parent->keys_.erase(parent->keys_.begin() + index);
    parent->children_.erase(parent->children_.begin() + index + 1);

    delete right_child;
}

void b_tree::borrow_from_prev(b_node* parent, int child_index)
{
    b_node* child = parent->children_[child_index];
    b_node* sibling = parent->children_[child_index - 1];

    child->keys_.insert(child->keys_.begin(), parent->keys_[child_index - 1]);
    parent->keys_[child_index - 1] = sibling->keys_.back();
    sibling->keys_.pop_back();

    if(!child->is_leaf_)
    {
        child->children_.insert(child->children_.begin(), sibling->children_.back());
        sibling->children_.pop_back();
    }
}

void b_tree::borrow_from_next(b_node* parent, int child_index)
{
    b_node* child = parent->children_[child_index];
    b_node* sibling = parent->children_[child_index + 1];

    child->keys_.push_back(parent->keys_[child_index]);
    parent->keys_[child_index] = sibling->keys_.front();
    sibling->keys_.erase(sibling->keys_.begin());

    if(!child->is_leaf_)
    {
        child->children_.push_back(sibling->children_.front());
        sibling->children_.erase(sibling->children_.begin());
    }
}

b_tree::b_node* b_tree::search_helper(b_tree::b_node* node, const game_record& record) const
{
    int i = 0;
    while(i < (int)node->keys_.size() && record > node->keys_[i])
    {
        ++i;
    }

    if(i < (int)node->keys_.size() && record == node->keys_[i])
    {
        return node;
    }

    if(node->is_leaf_)
    {
        return nullptr;
    }

    return search_helper(node->children_[i], record);
}

std::vector<game_record> b_tree::get_top_by_elo(int n, int player_id) const
{
    std::lock_guard<std::mutex> lock(lock_);

    std::vector<game_record> all_records = get_all_records();

    if(player_id != -1)
    {
        auto it = std::remove_if(all_records.begin(), all_records.end(),
                                 [player_id](const game_record& r) { return r.player_id != player_id; });
        all_records.erase(it, all_records.end());
    }

    std::sort(all_records.begin(), all_records.end(),
              [](const game_record& a, const game_record& b) { return a.elo_change > b.elo_change; });

    if(n > (int)all_records.size())
    {
        n = all_records.size();
    }

    std::vector<game_record> result(all_records.begin(), all_records.begin() + n);
    return result;
}

std::vector<game_record> b_tree::get_range(time_t start_time, time_t end_time, int player_id) const
{
    std::lock_guard<std::mutex> lock(lock_);

    std::vector<game_record> result;
    get_range_helper(root_, start_time, end_time, player_id, result);
    return result;
}

void b_tree::get_range_helper(b_node* node, time_t start_time, time_t end_time, int player_id,
                               std::vector<game_record>& result) const
{
    int i = 0;

    for(i = 0; i < (int)node->keys_.size(); ++i)
    {
        if(!node->is_leaf_)
        {
            get_range_helper(node->children_[i], start_time, end_time, player_id, result);
        }

        if(node->keys_[i].timestamp >= start_time && node->keys_[i].timestamp <= end_time)
        {
            if(player_id == -1 || node->keys_[i].player_id == player_id)
            {
                result.push_back(node->keys_[i]);
            }
        }
    }

    if(!node->is_leaf_)
    {
        get_range_helper(node->children_[i], start_time, end_time, player_id, result);
    }
}

std::vector<game_record> b_tree::get_all_records() const
{
    std::vector<game_record> result;

    std::function<void(b_node*)> traverse = [&](b_node* node) {
        for(int i = 0; i < (int)node->keys_.size(); ++i)
        {
            if(!node->is_leaf_)
            {
                traverse(node->children_[i]);
            }
            result.push_back(node->keys_[i]);
        }

        if(!node->is_leaf_ && (int)node->children_.size() > (int)node->keys_.size())
        {
            traverse(node->children_.back());
        }
    };

    traverse(root_);
    return result;
}

void b_tree::serialize_to_disk(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(lock_);

    std::ofstream file(filename, std::ios::binary);
    if(!file.is_open())
    {
        return;
    }

    file.write(reinterpret_cast<const char*>(&count_), sizeof(count_));
    serialize_node(root_, file);

    file.close();
}

void b_tree::serialize_node(b_node* node, std::ofstream& file) const
{
    bool is_leaf = node->is_leaf_;
    file.write(reinterpret_cast<const char*>(&is_leaf), sizeof(is_leaf));

    size_t key_count = node->keys_.size();
    file.write(reinterpret_cast<const char*>(&key_count), sizeof(key_count));

    for(const auto& key : node->keys_)
    {
        file.write(reinterpret_cast<const char*>(&key.player_id), sizeof(key.player_id));
        file.write(reinterpret_cast<const char*>(&key.opponent_id), sizeof(key.opponent_id));

        size_t result_len = key.result.length();
        file.write(reinterpret_cast<const char*>(&result_len), sizeof(result_len));
        file.write(key.result.c_str(), result_len);

        size_t pgn_len = key.pgn_moves.length();
        file.write(reinterpret_cast<const char*>(&pgn_len), sizeof(pgn_len));
        file.write(key.pgn_moves.c_str(), pgn_len);

        file.write(reinterpret_cast<const char*>(&key.timestamp), sizeof(key.timestamp));
        file.write(reinterpret_cast<const char*>(&key.duration_seconds), sizeof(key.duration_seconds));
        file.write(reinterpret_cast<const char*>(&key.elo_change), sizeof(key.elo_change));
    }

    if(!is_leaf)
    {
        size_t child_count = node->children_.size();
        file.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));

        for(auto child : node->children_)
        {
            serialize_node(child, file);
        }
    }
}

bool b_tree::deserialize_from_disk(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(lock_);

    std::ifstream file(filename, std::ios::binary);
    if(!file.is_open())
    {
        return false;
    }

    file.read(reinterpret_cast<char*>(&count_), sizeof(count_));

    clear_node(root_);
    root_ = deserialize_node(file);

    file.close();
    return true;
}

b_tree::b_node* b_tree::deserialize_node(std::ifstream& file)
{
    bool is_leaf;
    file.read(reinterpret_cast<char*>(&is_leaf), sizeof(is_leaf));

    b_node* node = new b_node(is_leaf);

    size_t key_count;
    file.read(reinterpret_cast<char*>(&key_count), sizeof(key_count));

    for(size_t i = 0; i < key_count; ++i)
    {
        game_record record;

        file.read(reinterpret_cast<char*>(&record.player_id), sizeof(record.player_id));
        file.read(reinterpret_cast<char*>(&record.opponent_id), sizeof(record.opponent_id));

        size_t result_len;
        file.read(reinterpret_cast<char*>(&result_len), sizeof(result_len));
        record.result.resize(result_len);
        file.read(&record.result[0], result_len);

        size_t pgn_len;
        file.read(reinterpret_cast<char*>(&pgn_len), sizeof(pgn_len));
        record.pgn_moves.resize(pgn_len);
        file.read(&record.pgn_moves[0], pgn_len);

        file.read(reinterpret_cast<char*>(&record.timestamp), sizeof(record.timestamp));
        file.read(reinterpret_cast<char*>(&record.duration_seconds), sizeof(record.duration_seconds));
        file.read(reinterpret_cast<char*>(&record.elo_change), sizeof(record.elo_change));

        node->keys_.push_back(record);
    }

    if(!is_leaf)
    {
        size_t child_count;
        file.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));

        for(size_t i = 0; i < child_count; ++i)
        {
            node->children_.push_back(deserialize_node(file));
        }
    }

    return node;
}

void b_tree::clear_node(b_node* node)
{
    if(node == nullptr)
    {
        return;
    }

    for(auto child : node->children_)
    {
        clear_node(child);
    }

    delete node;
}

void b_tree::clear()
{
    std::lock_guard<std::mutex> lock(lock_);

    clear_node(root_);
    root_ = new b_node(true);
    count_ = 0;
}

size_t b_tree::size() const
{
    std::lock_guard<std::mutex> lock(lock_);
    return count_;
}

} // namespace ds
