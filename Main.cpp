#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include "ThreadPool.cpp"
#include "hcl/huffmantool.h"

#include "shared_mq.hpp"


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

			std::cout << "Ваш путь: " << put << std::endl;

			std::queue<std::string> paths;

			getRecurs(paths, put); //рекурсивно получаем файлы

            //кладем в общую очередь
            std::cout << "Starting client." << std::endl;
            try {
                std::cout << "Creating queue..." << std::endl;
                constexpr unsigned kQueueSize = 100;//////////////
                shared_mq mq{ "my_queue", kQueueSize };

                std::cout << "Sending ints..." << std::endl;

                while (!paths.empty()) {

                    mq.send(paths.front());

                    paths.pop();
                }
            }
            catch (boost::interprocess::interprocess_exception& ex) {
                std::cerr << ex.what() << std::endl;
                break;
            }

            std::cout << "Finished client." << std::endl;

            //берем из общей очереди
            std::cout << "Starting server." << std::endl;
            try {
                std::cout << "Opening queue..." << std::endl;
                shared_mq mq{ "my_queue" };

                std::cout << "Receiving ints..." << std::endl;


                for (int i = 0; i < mq.get_num_msg(); ++i) {
                    std::string x = "";
                    try {
                        paths.push(mq.receive());
                    }
                    catch (boost::interprocess::interprocess_exception& ex) {
                        std::cerr << ex.what() << std::endl;
                        break;
                    }
                    std::cout << "Received: " << x << std::endl;
                }

            }
            catch (boost::interprocess::interprocess_exception& ex) {
                std::cerr << ex.what() << std::endl;
                break;
            }

            std::cout << "Finished server." << std::endl;

			if (!paths.empty()) {

				int cores_count = std::thread::hardware_concurrency(); //Узнаем кол-во ядер

				Thread_pool t(cores_count);
				while (!paths.empty()) {

					std::cout << paths.front() << std::endl;

					t.add_task(Arhivator, paths.front());  //потоки пошли в бой
					paths.pop();
				}

				t.wait_all();
				std::cout << "Готово!";
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

    huffmantool ht;

    try {
        std::string compressedFile = path;
        size_t start{ compressedFile.find_first_of(".") }; // Находим конец подстроки
        compressedFile.insert(start, "_compress");    
        std::string output = ht.compressFile(path, compressedFile);
    }
    catch (const char* error_message) {
        std::cout << error_message << std::endl;
    }
    
}
