#include "main_test.h"

class TestAutomaton : public ::testing::Test {
    protected: 
        void  SetUp() {
            A = new Automaton ({'a', 'b'}); // (w|a - w|b) % 3 == 0, 9 states, DFA
            for (int i = 0; i < 9; ++i) {
                int j = i + 1;
                if (i % 3 == 2) j = i - 2;
                A->add(i, j, 'a');
                j = i + 3;
                if (i >= 6) j = i - 6;
                A->add(i, j, 'b');
                if (i % 4 == 0) {
                    A->MakeTerminal(i);
                }
            }

            B = new Automaton({'B', 'c'}); // small DFA with different alphabet
            B->add(0, 1, 'B');
            B->add(1, 2, 'c');
            B->add(2, 1, 'c');
        } 

        void TearDown() {
            delete A;
            delete B;
        }
        Automaton* A;
        Automaton* B;
};

TEST_F(TestAutomaton, GetComplete_test1){
    Automaton aim = A->GetComplete();
    EXPECT_TRUE(*A == aim);
}

TEST_F(TestAutomaton, GetComplete_test2) {
    Automaton ans({'B', 'c'});
    ans.add(0, 1, 'B');
    ans.add(1, 2, 'c');
    ans.add(2, 1, 'c');
    ans.add(0, 3, 'c');
    ans.add(1, 3, 'B');
    ans.add(2, 3, 'B');
    ans.add(3, 3, 'B');
    ans.add(3, 3, 'c');
    Automaton CompleteB = B->GetComplete();
    EXPECT_TRUE(CompleteB == ans);
}

TEST_F(TestAutomaton, MinDFA_test1) {
    Automaton ans({'a', 'b'});
    ans.add(0, 0, 'a');
    ans.add(0, 0, 'b');
    A->ClearTerminal();
    EXPECT_TRUE(ans==A->MinDFA());
}

TEST_F(TestAutomaton, MinDFA_test2) 
{
	Automaton ans({'a', 'b'});
    ans.add(0, 1, 'a');
    ans.add(0, 2, 'b');
    ans.add(1, 2, 'a');
    ans.add(1, 0, 'b');
    ans.add(2, 0, 'a');
    ans.add(2, 1, 'b');
    ans.MakeTerminal(0);
    EXPECT_TRUE(ans==A->MinDFA());
}

TEST_F(TestAutomaton, GetComplement_test1) {
    Automaton C ({'a', 'b'}); // almost random large DFA
    for (int i = 0; i < 10; ++i) {
        C.add(i, (i * i + i + 17) % 10, 'a');
        C.add(i, ((1 << i) + 3 + i) % 10, 'b');
        if (i % 2 == 0) {
            C.MakeTerminal(i);
        }
    }
    Automaton ComplementC({'a', 'b'});
    for (int i = 0; i < 10; ++i) {
        ComplementC.add(i, (i * i + i + 17) % 10, 'a');
        ComplementC.add(i, ((1 << i) + 3 + i) % 10, 'b');
        if (i % 2 != 0) {
            ComplementC.MakeTerminal(i);
        }
    }
    EXPECT_TRUE(C.GetComplement() == ComplementC);
}

TEST_F(TestAutomaton, GetComplement_test2) {
    Automaton D({'a', 'b'}); // a(a+b)* + b
    D.add(0, 1, 'a');
    D.add(1, 1, 'a');
    D.add(1, 1, 'b');
    D.add(0, 2, 'b');
    D.MakeTerminal(1);
    D.MakeTerminal(2);

    Automaton ComplementD({'a', 'b'});
    ComplementD.add(0, 1, 'a');
    ComplementD.add(0, 2, 'b');
    ComplementD.add(1, 1, 'a');
    ComplementD.add(1, 1, 'b');
    ComplementD.add(2, 3, 'a');
    ComplementD.add(2, 3, 'b');
    ComplementD.add(3, 3, 'a');
    ComplementD.add(3, 3, 'b');
    ComplementD.MakeTerminal(0);
    ComplementD.MakeTerminal(3);

    EXPECT_TRUE(D.GetComplement() == ComplementD);
}

TEST_F(TestAutomaton, DeleteEmpty_test1) {
    Automaton E({'a', 'b'}); // small senseless NFA
    E.add(0, 0, '1');
    E.add(0, 1, 'a');
    E.add(1, 0, '1');
    E.add(1, 2, 'b');
    E.add(1, 2, '1');
    E.add(2, 2, 'a');
    E.MakeTerminal(2);

    Automaton NoEmptyE({'a', 'b'});
    NoEmptyE.add(0, 1, 'a');
    NoEmptyE.add(1, 2, 'b');
    NoEmptyE.add(1, 1, 'a');
    NoEmptyE.add(1, 2, 'a');
    NoEmptyE.add(2, 2, 'a');
    NoEmptyE.MakeTerminal(1);
    NoEmptyE.MakeTerminal(2);
    EXPECT_TRUE(E.DeleteEmpty() == NoEmptyE);
}

TEST_F(TestAutomaton, DeleteEmpty_test2) {
    EXPECT_TRUE(*A == A->DeleteEmpty());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
