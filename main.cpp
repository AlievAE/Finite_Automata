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

std::string Test() {
    Automaton A({'a', 'b'});
    A.add(0, 1, 'a');
    A.add(0, 2, 'a');
    A.add(1, 1, 'b');
    A.add(1, 2, 'b');
    A.add(0, 3, 'b');
    A.add(3, 3, 'a');
    A.add(3, 2, 'a');
    A.MakeTerminal(2);
    Automaton B({'a', 'b', 'c'});
    B.add(0, 1, 'a');
    B.add(0, 2, 'b');
    B.add(0, 3, 'b');
    B.add(1, 1, 'b');
    B.add(1, 2, 'b');
    B.add(3, 3, 'a');
    B.add(3, 2, 'a');
    B.MakeTerminal(2);
    return B.Discriminator(A);
}

int main() {
    std::cout << Test();
}