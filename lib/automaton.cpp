#include "automaton.h"

std::ostream& operator << (std::ostream& out, const Automaton& automaton) {
    auto Graph = automaton.GetGraph();
    out << "total vertexes: " << Graph.size() << "\n" << "start: " << 0 << "\n";
    for (int i = 0; i < Graph.size(); ++i) {
        for (auto edge : Graph[i]) {
            out << i << " ---> " << edge.first << " via " << edge.second << "\n";
        }
    }
    out << "\nterminal vertexes:\n";
    auto term = automaton.GetTerminal();
    for (int el : term) {
        out << el << " ";
    }
    out << std::endl;
    return out;
}

void Automaton::add(int start, int finish, char symbol) {
    int sz = Graph_.size();
    for (int i = 0; i < std::max(start, finish) - sz + 1; ++i) {
        Graph_.push_back({});
    }
    for (std::pair<int, char>& p : Graph_[start]) {
        if (p == std::make_pair(finish, symbol)) {
            return;
        }
    }
    Graph_[start].push_back({finish, symbol});
}

void Automaton::MakeTerminal(int vertex) {
    int sz = Graph_.size();
    for (int i = 0; i < vertex - sz + 1; ++i) {
        Graph_.push_back({});
    }
    for (auto el : terminal_) {
        if (el == vertex) return;
    }
    terminal_.push_back(vertex);
}

Automaton Automaton::DeleteEmpty() const {
    Automaton res(alphabet_);
    for (int i = 0; i < Graph_.size(); ++i) {
        std::vector<int> children = GetSon(i);
        for (int son : children) {
            for (auto edge : Graph_[son]) {
                if (edge.second != '1') {
                    res.add(i, edge.first, edge.second);
                }
            }
            for (int term : terminal_) {
                if (term == son) {
                    res.MakeTerminal(i);
                }
            }
        }
    }
    return res;
}

std::vector<int> Automaton::GetSon(int ind) const {
    std::vector<int> res;
    std::vector<bool> used;
    used.assign(Graph_.size(), false);
    std::queue<int> bfs_q;
    bfs_q.push(ind);
    res.push_back(ind);
    used[ind] = true;
    while (!bfs_q.empty()) {
        int cur_vertex = bfs_q.front();
        bfs_q.pop();
        for (auto edge : Graph_[cur_vertex]) {
            if (edge.second != '1') continue;
            if (!used[edge.first]) {
                used[edge.first] = true;
                res.push_back(edge.first);
                bfs_q.push(edge.first);
            }
        }
    }
    return res;
}

Automaton Automaton::DFA() const {
    Automaton res(alphabet_);

    std::map<std::set<int>, int> index_of_subset;
    std::vector<std::set<int>> subset;
    std::queue<int> bfs_q;

    bfs_q.push(0);
    subset.push_back({0});
    index_of_subset[{0}] = 0;

    while (!bfs_q.empty()) {
        auto cur_vertex = bfs_q.front();
        bfs_q.pop();
        for (char symbol: alphabet_) {
            std::set<int> vertex;
            for (int v : subset[cur_vertex]) {
                for (auto edge : Graph_[v]) {
                    if (edge.second == symbol) {
                        vertex.insert(edge.first);
                    }
                }
            }  
            if (vertex.empty()) continue;
            if (index_of_subset.find(vertex) != index_of_subset.end()) {
                res.add(cur_vertex, index_of_subset[vertex], symbol);
            } else {
                subset.push_back(vertex);
                index_of_subset[vertex] = subset.size() - 1;
                bfs_q.push(subset.size() - 1);
                res.add(cur_vertex, index_of_subset[vertex], symbol);
            }
        }
    }

    for (int i = 0; i < subset.size(); ++i) {
        for (int el : subset[i]) {
            if (std::find(terminal_.begin(), terminal_.end(), el) != terminal_.end()) {
                res.MakeTerminal(i);
                break;
            }
        }
    }
    return res;
}

bool Automaton::IsComplete() const {
    for (int i = 0; i < Graph_.size(); ++i) {
        for (char symbol : alphabet_) {
            bool flag = false;
            for (auto edge : Graph_[i]) {
                if (edge.second == symbol) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                return false;
            }
        }
    }
    return true;
}

Automaton Automaton::GetComplete() const {
    if (IsComplete()) {
        return *this;
    }
    Automaton res = *this;
    for (int i = 0; i < Graph_.size() + 1; ++i) {
        for (char symbol : alphabet_) {
            bool flag = false;
            for (auto edge : res.Graph_[i]) {
                if (edge.second == symbol) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                res.add(i, Graph_.size(), symbol);
            }
        }
    }
    return res;
}


Automaton Automaton::GetComplement() const {
    auto res = GetComplete();
    res.ClearTerminal();
    for (int i = 0; i < res.Graph_.size(); ++i) {
        bool flag = false;
        for (int term : terminal_) {
            if (term == i) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            res.MakeTerminal(i);
        }
    }
    return res;
}

Automaton Automaton::MinDFA() const {
    auto cDFA = GetDFA().GetComplete();
    std::map<char, int> alphabet_index;
    for (int i = 0; i < cDFA.alphabet_.size(); ++i) {
        alphabet_index[cDFA.alphabet_[i]] = i;
    }
    int matrix[cDFA.Size()][cDFA.alphabet_.size()];
    for (int i = 0; i < cDFA.Size(); ++i) {
        for (auto [vertex, symbol] : cDFA.Graph_[i]) {
            matrix[i][alphabet_index[symbol]] = vertex; 
        }
    }
    std::vector<int> category;
    std::vector<int> next_category;
    category.assign(cDFA.Size(), 0);
    next_category.assign(cDFA.Size(), 0);
    for (int term : cDFA.terminal_) {
        category[term] = 1;
    }
    while (true) {
        std::vector<int> next;
        next.assign(cDFA.alphabet_.size() + 1, 0);
        std::map<std::vector<int>, int> new_cat;
        for (int i = 0; i < cDFA.Size(); ++i) {
            next[0] = category[i];
            for (auto [vertex, symbol] : cDFA.Graph_[i]) {
                next[alphabet_index[symbol] + 1] = category[vertex];
            }
            if (new_cat.find(next) != new_cat.end()) {
                next_category[i] = new_cat[next];
            } else {
                next_category[i] = new_cat.size();
                new_cat[next] = next_category[i];
            }
        } 
        int old_size = (*std::max_element(category.begin(), category.end())) + 1;
        if (new_cat.size() == old_size) {
            Automaton res(cDFA.alphabet_);
            for (int i = 0; i < cDFA.Size(); ++i) {
                for (auto [vertex, symbol] : cDFA.Graph_[i]) {
                    res.add(next_category[i], next_category[vertex], symbol);
                }
            }
            for (int term : cDFA.terminal_) {
                res.MakeTerminal(next_category[term]);
            }
            return res;
        }
        category.swap(next_category);
    }
}