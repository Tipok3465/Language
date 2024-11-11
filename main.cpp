#include "iostream"
#include "src/back/Translator.h"
#include "src/back/Fsm.h"

int main() {
//    Translator translator{};
//    translator.run();
    Fsm fsm;
    while (true) {
        int a;
        std::cin >> a;
        if (a == 1) fsm.update();
    }
    return 0;
}