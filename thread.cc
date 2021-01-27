#include <iostream>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <mutex>
#include <boost/functional/hash.hpp>
#include <time.h>
#include <ctime>
#include <string>
//#include <zlib>
using namespace std;

int GetRandom(int max) {
    srand(time(NULL));
    return rand() % max;
}

void ExecuteThread(int id) {
    auto nowTime = std::chrono::system_clock::now();
    std::time_t sleepTime = std::chrono::system_clock::to_time_t(nowTime);
    tm myLocalTime = *localtime(&sleepTime);

    std::cout << "Thread " << id << " Sleep Time: " << std::ctime(&sleepTime) << endl;
    std::cout << "Month: " << myLocalTime.tm_mon + 1 << endl;
    std::cout << "Day: " << myLocalTime.tm_mday << endl;
    std::cout << "Year: " << myLocalTime.tm_year + 1900 << endl;
    std::cout << "Hours: " << myLocalTime.tm_hour << endl;
    std::cout << "Minutes: " << myLocalTime.tm_min << endl;
    std::cout << "Seconds: " << myLocalTime.tm_sec << endl;

    std::this_thread::sleep_for(std::chrono::seconds(GetRandom(3)));
    nowTime = std::chrono::system_clock::now();
    sleepTime = std::chrono::system_clock::to_time_t(nowTime);
    std::cout << "Thread: " << id << " Awake Time: " << std::ctime(&sleepTime) << endl;
}

double acctBalance = 100;
std::mutex acctLock;

std::string GetTime() {
    auto nowTime = std::chrono::system_clock::now();
    std::time_t sleepTime = std::chrono::system_clock::to_time_t(nowTime);
    return std::ctime(&sleepTime);
}

void GetMoney(int id, double withdrawal) {
    std::lock_guard<std::mutex> lock(acctLock);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << id << " tries to withdrawal $" << withdrawal << " on " << GetTime() << endl;

    if ((acctBalance - withdrawal) >= 0) {
        acctBalance -= withdrawal;
        std::cout << "New Accout Balance is $" << acctBalance << endl;
    } else {
        std::cout << "Not enough money in account\n";
        std::cout << "Current Balance is $" << acctBalance << "\n";
    }
}

int main() {

    std::thread threads[10];
    for (int i=0; i<10; i++) {
        threads[i] = std::thread(GetMoney, i, 15);
    }
    for (int i=0; i<10; i++)
        threads[i].join();

    return 0;
}
