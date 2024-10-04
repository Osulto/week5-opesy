#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>
#include <atomic>
#include <conio.h> 

std::atomic<bool> stopMarquee(false);
std::mutex mtx;

const int WIDTH = 80;
const int HEIGHT = 20;


void drawHeader() {
    std::cout << "\033[1;1H";
    std::cout << "******************************\n";
    std::cout << "* Displaying marquee console! *\n";
    std::cout << "******************************\n";
    std::cout.flush();
}

void clearMarqueeArea() {

    for (int i = 0; i < HEIGHT; ++i) {
        std::cout << "\033[" << (4 + i) << ";1H";
        std::cout << std::string(WIDTH, ' ');
    }
}

void drawMarquee(const std::string& text, int x, int y) {
    std::cout << "\033[" << (4 + y) << ";1H";
    std::string padding(x, ' ');
    std::cout << padding << text;
    std::cout.flush();
}

void drawCommandPrompt(const std::string& currentInput) {
    std::cout << "\033[" << (HEIGHT + 6) << ";1H";
    std::cout << "Enter a command: " << currentInput;
    std::cout.flush();
}

void processCommand(const std::string& input) {
    std::cout << "\033[" << (HEIGHT + 7) << ";1H";
    std::cout << "Command processed: " << input << "        ";
    std::cout.flush();
}

void marquee(const std::string& text) {
    int x = 0, y = 0;
    int xDirection = 1;
    int yDirection = 1;

    while (!stopMarquee) {
        mtx.lock();
        clearMarqueeArea();
        drawMarquee(text, x, y);
        mtx.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));


        x += xDirection;
        y += yDirection;


        if (x <= 0 || x >= WIDTH - text.length()) {
            xDirection *= -1;
        }
        if (y <= 0 || y >= HEIGHT - 1) {
            yDirection *= -1;
        }
    }
}

void commandInput() {
    std::string input;
    while (!stopMarquee) {
        char ch;
        if (_kbhit()) {
            ch = _getch();
            if (ch == '\r') {
                if (!input.empty()) {

                    processCommand(input);
                    if (input == "stop") {
                        stopMarquee = true;
                    }
                    input.clear();
                }
            }
            else if (ch == '\r') {
                if (!input.empty()) {
                    input.pop_back();
                }
            }
            else {
                input += ch;
            }
        }

        mtx.lock();
        drawCommandPrompt(input);
        mtx.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

int main() {
    std::string text = "Hello world in marquee!";

    drawHeader();

    std::thread marqueeThread(marquee, text);

    std::thread inputThread(commandInput);

    marqueeThread.join();
    inputThread.join();

    return 0;
}

