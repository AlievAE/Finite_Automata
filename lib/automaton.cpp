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
    int graph_size = Graph_.size();
    for (int i = 0; i < vertex - graph_size + 1; ++i) {
        Graph_.push_back({});
    }
    for (auto term : terminal_) {
        if (term == vertex) {
            return;
        }
    }
    terminal_.push_back(vertex);
}

Automaton Automaton::DeleteEmpty() const {
    Automaton res(alphabet_);
    for (int i = 0; i < Graph_.size(); ++i) {
        std::vector<int> children = GetSon(i);
        for (int son : children) {
            for (auto [vertex, symbol] : Graph_[son]) {
                if (symbol != '1') {
                    res.add(i, vertex, symbol);
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
    std::vector<int> res; // BFS preprocessing
    res.push_back(ind);
    std::vector<bool> used;
    used.assign(Graph_.size(), false);
    used[ind] = true;
    std::queue<int> process;
    process.push(ind);

    while (!process.empty()) { // BFS start
        int cur_vertex = process.front();
        process.pop();
        for (auto [vertex, symbol] : Graph_[cur_vertex]) {
            if (symbol != '1') continue;
            if (!used[vertex]) {
                used[vertex] = true;
                res.push_back(vertex);
                process.push(vertex);
            }
        }
    }
    return res;
}

Automaton Automaton::DFA() const {
    Automaton res(alphabet_);

    std::map<std::set<int>, int> index_of_subset;
    std::vector<std::set<int>> subset;
    std::queue<int> process;


    const int root = 0;
    process.push(root);
    subset.push_back({root});
    index_of_subset[{root}] = 0;

    while (!process.empty()) {
        auto cur_vertex = process.front();
        process.pop();
        for (char symbol: alphabet_) {
            std::set<int> vertex;
            for (int v : subset[cur_vertex]) {
                for (auto edge : Graph_[v]) {
                    if (edge.second == symbol) {
                        vertex.insert(edge.first);
                    }
                }
            }  
            if (vertex.empty()) {
                continue;
            }
            if (index_of_subset.find(vertex) != index_of_subset.end()) {
                res.add(cur_vertex, index_of_subset[vertex], symbol);
            } else {
                subset.push_back(vertex);
                index_of_subset[vertex] = subset.size() - 1;
                process.push(subset.size() - 1);
                res.add(cur_vertex, index_of_subset[vertex], symbol);
            }
        }
    }

    for (int i = 0; i < subset.size(); ++i) {
        for (int state : subset[i]) {
            if (isTerminal(state)) {
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
        std::map<std::vector<int>, int> equivalence_class;
        for (int i = 0; i < cDFA.Size(); ++i) {
            next[0] = category[i];
            for (auto [vertex, symbol] : cDFA.Graph_[i]) {
                next[alphabet_index[symbol] + 1] = category[vertex];
            }
            if (equivalence_class.find(next) != equivalence_class.end()) {
                next_category[i] = equivalence_class[next];
            } else {
                next_category[i] = equivalence_class.size();
                equivalence_class[next] = next_category[i];
            }
        } 
        int old_size = (*std::max_element(category.begin(), category.end())) + 1;
        if (equivalence_class.size() == old_size) {
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

Automaton Automaton::Intersect(const Automaton& right) const {
    Automaton Left = DeleteEmpty();
    Automaton Right = right.DeleteEmpty();
    Automaton res(alphabet_);
    std::map<std::pair<int, int>, int> used;
    const int root = 0;
    used[{root, root}] = 0;
    std::vector<std::pair<int, int>> vertex;
    vertex.push_back({root, root});
    std::queue<int> process;
    process.push(0);
    while (!process.empty()) {
        int state = process.front();
        process.pop();
        int left_state = vertex[state].first;
        int right_state = vertex[state].second;
        for (auto [left_vertex, left_symbol] : Left.Graph_[left_state]) {
            for (auto [right_vertex, right_symbol] : Right.Graph_[right_state]) {
                if (left_symbol == right_symbol) {
                    std::pair<int, int> new_vertex = {left_vertex, right_vertex};
                    if (used.find(new_vertex) == used.end()) {
                        used[new_vertex] = vertex.size();
                        vertex.push_back(new_vertex);
                        process.push(used[new_vertex]);
                    }
                    res.add(state, used[new_vertex], left_symbol);
                }
            }
        }
    }
    for (int i = 0; i < vertex.size(); ++i) {
        int left_vertex = vertex[i].first;
        int right_vertex = vertex[i].second;
        if (Left.isTerminal(left_vertex) && Right.isTerminal(right_vertex)) {
            res.MakeTerminal(i);
        }
    }
    return res;
}

bool Automaton::isEquivalent(const Automaton& right) const {
    Automaton Left = GetDFA();
    Automaton Right = right.GetDFA();
    Automaton onlyLeft = Left.Intersect(Right.GetComplement());
    Automaton onlyRight = Right.Intersect(Left.GetComplement());
    return (onlyLeft.terminal_.empty() && onlyRight.terminal_.empty());
}

bool Automaton::AnyWord(std::vector<int>& used, std::string& answer, std::string& path, int start) const {
    used[start] = true;
    if (isTerminal(start)) {
        answer = path;
        return true;
    }
    for (auto [vertex, symbol] : Graph_[start]) {
        if (used[vertex]) {
            continue;
        }
        path += symbol;
        if (AnyWord(used, answer, path, vertex)) {
            return true;
        }
        path.pop_back();
    }
    return false;
}

std::string Automaton::Discriminator(const Automaton& right) const {
    Automaton Left = GetDFA();
    Automaton Right = right.GetDFA();
    Automaton onlyLeft = Left.Intersect(Right.GetComplement());
    Automaton onlyRight = Right.Intersect(Left.GetComplement());
    std::string res = "";
    std::string path = "";
    std::vector<int> used;
    int root = 0;
    if (!onlyLeft.terminal_.empty()) {
        used.assign(onlyLeft.Size(), false);
        onlyLeft.AnyWord(used, res, path, 0);
    } else if (!onlyRight.terminal_.empty()) {
        used.assign(onlyRight.Size(), false);
        onlyRight.AnyWord(used, res, path, 0);
    } else {
        std::cerr << "no discriminator found" << std::endl;
        return "$$$DISCRIMINATOR FAIL!!!!!";
    }
    return res;
}