#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include "ThreadPool.cpp"


void getRecurs(std::queue<std::string>&, const std::string&);
void Arhivator(std::string);


int main(int argc, char** argv) {

    setlocale(LC_ALL, "rus");

    if (argc == 1) { // если в аргументах только имя программы
        std::cout << "Вы не ввели путь" << std::endl; // выводим, что нет аргументов
    }
    else {

        for (int i = 1; i < argc; i++) {

            std::string put = argv[i]; //получаем путь

            std::queue<std::string> paths;

            getRecurs(paths, put); //рекурсивно получаем файлы

            if (!paths.empty()) {

                int cores_count = std::thread::hardware_concurrency(); //Узнаем кол-во ядер

                Thread_pool t(cores_count);

                for (int i = 0; i < paths.size(); ++i) {
                    t.add_task(Arhivator, paths.front());  //потоки пошли в бой
                    paths.pop();
                }

                t.wait_all();
            }
        }

    }

    return 0;
}

void getRecurs(std::queue<std::string>& paths, const std::string& path) {

    try {
        for (const auto& file : std::filesystem::directory_iterator(path)) { //проходим по директории
            if (std::filesystem::is_directory(file)) { //если является директорией
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

void Arhivator(std::string path) { //выполняем архивирование файлов, поточная функция
    //сжатие
}
