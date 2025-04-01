#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <queue>


int main(int argc, char** argv) {

    if (argc == 1) { // если в аргументах только имя программы
        std::cout << "no arguments!" << std::endl; // выводим, что нет аргументов
    }
    else {
        // иначе выводим все аргументы, которые переданы
        for (int i = 1; i < argc; i++) {
            std::cout << "argv[" << i << "] - " << argv[i] << std::endl;
        }
    }

    return 0;
}