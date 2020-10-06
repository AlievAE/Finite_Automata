#include "lib/automaton.h"

Automaton Task1() {
    Automaton A({'a', 'b'});
    A.add(0, 0, 'a');
    A.add(0, 0, 'b');
    A.add(0, 1, 'a');
    A.add(1, 3, 'a');
    A.add(3, 3, 'a');
    A.add(3, 3, 'b');
    A.add(3, 4, 'b');
    A.add(4, 5, 'b');
    A.add(5, 5, 'b');
    A.add(5, 6, 'a');
    A.add(6, 6, 'b');
    A.add(1, 2, 'b');
    A.add(2, 5, 'a');
    A.MakeTerminal(6);
    return A.MinDFA();
}

Automaton Task2() {
    Automaton A({'a', 'b'});
    A.add(0, 1, 'a');
    A.add(1, 0, '1');
    A.add(1, 2, 'a');
    A.add(2, 3, 'b');
    A.add(3, 1, '1');
    A.add(1, 4, 'b');
    A.add(4, 5, 'b');
    A.add(5, 4, 'a');
    A.add(4, 3, 'a');
    A.MakeTerminal(0);
    return A.GetDFA().GetComplement().MinDFA();
}

int main() {
    std::cout << Task2();
}