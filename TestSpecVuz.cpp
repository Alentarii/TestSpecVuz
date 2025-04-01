#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <queue>

void getRecurs(std::queue<std::string>&, const std::string&);

int main(int argc, char** argv) {

    setlocale(LC_ALL, "rus");
    
    if (argc == 1) { // если в аргументах только имя программы
        std::cout << "Вы не ввели путь" << std::endl; // выводим, что нет аргументов
    }
    else {

        for (int i = 1; i < argc; i++) {

            std::string put = argv[i];
            std::queue<std::string> paths;

            getRecurs(paths, put);

            if (!paths.empty()) {
                for (int i = 0; i < paths.size(); ++i) {
                    std::cout << paths.front() << std::endl;
                    paths.pop(); //удаляем первый
                }
            }
        }
    }

    return 0;
}

void getRecurs(std::queue<std::string>& paths, const std::string& path) {

    try {
        for (const auto& file : std::filesystem::directory_iterator(path)) { //проходим по дирректории
            if (std::filesystem::is_directory(file)) { //если является дирректорией
                getRecurs(paths, file.path().string());
            }
            else {
                paths.push(file.path().string());
            }
        }
    }
    catch (std::filesystem::filesystem_error  const& ex) {
        std::cout << "Исключение: " << ex.what() << std::endl;
    }

}