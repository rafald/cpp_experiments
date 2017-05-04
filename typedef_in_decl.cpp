#include <iostream> 

typedef long long ll;

void foo(unsigned ll) {
    std::cout << "1";
}

void foo(unsigned long long) {
    std::cout << "2";
}

void foo(const ll) {   //  <== NOTE!
    std::cout << "3";
}

int main() {
    foo(2ull); // 2
    foo(7ll);  // 3
}


