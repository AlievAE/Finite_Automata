#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>

class Automaton { // one-letter edges only, 1 = empty symbol
public:
    Automaton(std::vector<char> alphabet) :
        alphabet_(alphabet),
        G_({{}}) {}

    void add(int a, int b, char c);
    void MakeTerminal(int a);
    void ClearTerminal() {
        terminal_.clear();
    }

    Automaton GetComplement() const; // works only with DFA

    Automaton DeleteEmpty() const;

    Automaton GetDFA() const {
        return DeleteEmpty().DFA();
    }

    bool IsComplete() const; // works only with DFA

    Automaton GetComplete() const; // works only with DFA

    Automaton MinDFA() const;

    std::vector<std::vector<std::pair<int, char>>> GetGraph() const {
        return G_;
    }

    int Size() const {
        return G_.size();
    }
    std::vector<int> GetTerminal() const {
        return terminal_;
    }
    std::vector<char> GetAlphabet() const {
        return alphabet_;
    }

    bool operator ==(const Automaton& aim) {
        return (alphabet_ == aim.alphabet_ && 
            G_ == aim.G_ &&
            terminal_ == aim.terminal_);
    }

private:
    std::vector<char> alphabet_;
    std::vector<std::vector<std::pair<int, char>>> G_;
    std::vector<int> terminal_;

    std::vector<int> GetSon(int ind) const;
    Automaton DFA() const;

};

std::ostream& operator << (std::ostream& out, const Automaton& A);