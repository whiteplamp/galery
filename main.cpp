#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <chrono>
#include <Windows.h>

using namespace std;

mutex mtx; // Mutex для синхронизации одновременно выполняющихся потоков доступа к галерее

class Gallery {
private:
    int visitorsCount; // Кол-во посетителей в галерее
    vector<int> pictures; // Список картин и кол-во посетителей, которые её рассматривают
public:
    Gallery() {
        visitorsCount = 0;
        pictures = vector<int>(5, 0); // Изначально ни одна картина не рассматривается
    }


    void enterGallery() { //Вход в гелерею
        unique_lock<mutex> lock(mtx); //В C ++ мьютекс создается путем создания экземпляра std :: mutex, блокируется вызовом функции-члена lock () и разблокируется функцией unlock ()
        if (visitorsCount > 50) { // Если уже 50 посетителей в галерее, то посетитель ждет
            lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(200));
            enterGallery();
        } else {
            visitorsCount++; // Увеличиваем количество посетителей в галерее
            lock.unlock();

            cout << "Visitor " << this_thread::get_id() << " came in gallery" << endl;
            viewPictures(); // Посетитель рассматривает картину

            lock.lock();
            visitorsCount--; // Уменьшаем кол-во посетителей в галерее
            lock.unlock();
            cout << "Visitor " << this_thread::get_id() << " exit from gallery" << endl;
        }
    }

    void viewPictures() { //Просмотр картин
        unique_lock<mutex> lock(mtx);
        int pictureIndex = rand() % 5; // Выбираем случайную картину из 5

        while (pictures[pictureIndex] >= 10) { // Если на картину уже смотрит 10 или более посетителей, ждем
            lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(200)); //milliseconds
            lock.lock();
        }

        pictures[pictureIndex]++; // Увеличиваем количество посетителей, которые рассматривают данную картину
        lock.unlock();

        cout << "Visitor " << this_thread::get_id() << " watch painting " << pictureIndex + 1 << endl;
        this_thread::sleep_for(chrono::milliseconds(500)); // Время на расссмотрение картины

        lock.lock();
        pictures[pictureIndex]--; // Уменьшаем кол-во посетителей, которые рассматривают данную картину
        lock.unlock();
    }
};

void simulateVisitor(Gallery& gallery) {
    gallery.enterGallery();
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Gallery gallery; //Создаем объект класса Gallery, который будет представлять галерею с картинами и посетителями.

    vector<thread> threads; //Создается вектор (список) потоков, чтобы имитировать действия множества посетителей.
    for (int i = 0; i < 100; i++) { //Начинается цикл, который прокрутится 100 раз, чтобы создать 100 отдельных потоков.
        threads.push_back(thread(simulateVisitor, ref(gallery))); //Создается новый поток (thread). Этот поток начинает выполнение функции simulateVisitor, которая имитирует действия посетителя галереи. В качестве аргумента функции передается ссылка на объект gallery, чтобы поток мог работать с галереей. Новый поток добавляется в вектор threads.
    }
    for (auto& thread : threads) { //Начинается еще один цикл, который проходит по всем потокам в векторе threads.
        thread.join(); //Внутри цикла выполняется метод join() для каждого потока. Это означает, что программа будет ждать завершения каждого потока перед продолжением.
    }
    return 0;

}
