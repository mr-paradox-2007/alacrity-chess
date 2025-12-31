#ifndef B_TREE_HPP
#define B_TREE_HPP

#include <vector>
#include <string>
#include <ctime>
#include <mutex>
#include <algorithm>

namespace ds
{

struct game_record
{
    int player_id;
    int opponent_id;
    std::string result;
    std::string pgn_moves;
    time_t timestamp;
    int duration_seconds;
    int elo_change;

    game_record() : player_id(0), opponent_id(0), timestamp(0), duration_seconds(0), elo_change(0)
    {
    }

    bool operator<(const game_record& other) const
    {
        if(player_id != other.player_id)
            return player_id < other.player_id;
        return timestamp < other.timestamp;
    }

    bool operator==(const game_record& other) const
    {
        return player_id == other.player_id && opponent_id == other.opponent_id && timestamp == other.timestamp;
    }

    bool operator>(const game_record& other) const
    {
        if(player_id != other.player_id)
            return player_id > other.player_id;
        return timestamp > other.timestamp;
    }
};

class b_tree
{
public:
    struct b_node
    {
        std::vector<game_record> keys_;
        std::vector<b_node*> children_;
        bool is_leaf_;

        b_node(bool is_leaf = true) : is_leaf_(is_leaf)
        {
        }
    };

private:
    static const int b_order_ = 3;

    b_node* root_;
    size_t count_;
    mutable std::mutex lock_;

    void split_child(b_node* parent, int index);
    void insert_non_full(b_node* node, const game_record& record);
    void merge(b_node* parent, int index);
    void borrow_from_prev(b_node* parent, int child_index);
    void borrow_from_next(b_node* parent, int child_index);
    void remove_helper(b_node* node, const game_record& record);
    b_node* search_helper(b_node* node, const game_record& record) const;
    void get_range_helper(b_node* node, time_t start_time, time_t end_time, int player_id,std::vector<game_record>& result) const;
    void serialize_node(b_node* node, std::ofstream& file) const;
    b_node* deserialize_node(std::ifstream& file);
    void clear_node(b_node* node);
    int find_key(b_node* node, const game_record& record) const;

public:
    b_tree();
    ~b_tree();

    bool insert(const game_record& record);
    std::vector<game_record> get_top_by_elo(int n, int player_id = -1) const;
    std::vector<game_record> get_range(time_t start_time, time_t end_time, int player_id = -1) const;
    std::vector<game_record> get_all_records() const;
    bool remove(const game_record& record);
    void serialize_to_disk(const std::string& filename);
    bool deserialize_from_disk(const std::string& filename);
    void clear();
    size_t size() const;

    b_tree(const b_tree&) = delete;
    b_tree& operator=(const b_tree&) = delete;
};

} // namespace ds

#endif // B_TREE_HPP
