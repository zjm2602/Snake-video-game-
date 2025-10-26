#include <iostream>
#include <vector>
#include <deque>
#include <conio.h> // _kbhit, _getch (Windows)
#include <windows.h> // Sleep
#include <cstdlib>
#include <ctime>

using namespace std;

const int WIDTH = 30;
const int HEIGHT = 20;

enum Direction { UP, RIGHT, DOWN, LEFT };

struct Point { int x, y; };

void clearScreen() {
    // 在 Windows 控制台使用 cls
    system("cls");
}

bool pointsEqual(const Point &a, const Point &b) {
    return a.x == b.x && a.y == b.y;
}

Point randomFruit(const deque<Point>& snake) {
    Point p;
    while (true) {
        p.x = rand() % WIDTH;
        p.y = rand() % HEIGHT;
        bool ok = true;
        for (const auto &s : snake) if (pointsEqual(s, p)) { ok = false; break; }
        if (ok) return p;
    }
}

int main() {
    srand((unsigned)time(NULL));

    deque<Point> snake;
    // 初始长度 3，位于中间
    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;
    for (int i = 0; i < 3; ++i) snake.push_back({startX - i, startY});

    Direction dir = RIGHT;
    Point fruit = randomFruit(snake);
    bool gameOver = false;
    int score = 0;
    int speedMs = 120; // 初始速度 (毫秒)

    while (!gameOver) {
        // 输入处理（非阻塞）
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 0 || ch == 224) {
                // 扩展按键：方向键
                int ch2 = _getch();
                if (ch2 == 72 && dir != DOWN) dir = UP;       // 上
                else if (ch2 == 80 && dir != UP) dir = DOWN;  // 下
                else if (ch2 == 75 && dir != RIGHT) dir = LEFT; // 左
                else if (ch2 == 77 && dir != LEFT) dir = RIGHT; // 右
            } else {
                // 字符按键：WASD
                if ((ch == 'w' || ch == 'W') && dir != DOWN) dir = UP;
                else if ((ch == 's' || ch == 'S') && dir != UP) dir = DOWN;
                else if ((ch == 'a' || ch == 'A') && dir != RIGHT) dir = LEFT;
                else if ((ch == 'd' || ch == 'D') && dir != LEFT) dir = RIGHT;
                else if (ch == 'q' || ch == 'Q') { gameOver = true; }
            }
        }

        // 移动蛇头
        Point head = snake.front();
        Point newHead = head;
        if (dir == UP) newHead.y -= 1;
        else if (dir == DOWN) newHead.y += 1;
        else if (dir == LEFT) newHead.x -= 1;
        else if (dir == RIGHT) newHead.x += 1;

        // 碰撞检测：墙壁
        if (newHead.x < 0 || newHead.x >= WIDTH || newHead.y < 0 || newHead.y >= HEIGHT) {
            gameOver = true;
        } else {
            // 碰撞检测：自身
            for (const auto &s : snake) if (pointsEqual(s, newHead)) { gameOver = true; break; }
        }

        if (gameOver) break;

        // 推入新头
        snake.push_front(newHead);

        // 吃到水果？
        if (pointsEqual(newHead, fruit)) {
            score += 10;
            // 每吃到若干个加速一点（可调）
            if (score % 50 == 0 && speedMs > 40) speedMs -= 10;
            fruit = randomFruit(snake);
        } else {
            // 正常移动：去掉尾部
            snake.pop_back();
        }

        // 绘制
        clearScreen();
        // 顶部信息
        cout << "Score: " << score << "    Speed: " << (140 - speedMs) / 10 << "\n";
        cout << "Controls: WASD or Arrow Keys, Q to quit\n";

        // 地图
        for (int y = -1; y <= HEIGHT; ++y) {
            for (int x = -1; x <= WIDTH; ++x) {
                if (y == -1 || y == HEIGHT) {
                    // 顶部或底部边框
                    if (x == -1) cout << '+'; else if (x == WIDTH) cout << '+'; else cout << '-';
                } else if (x == -1 || x == WIDTH) {
                    cout << '|';
                } else {
                    Point p{ x, y };
                    if (pointsEqual(p, snake.front())) cout << 'O';
                    else if (pointsEqual(p, fruit)) cout << '*';
                    else {
                        bool printed = false;
                        for (auto it = snake.begin() + 1; it != snake.end(); ++it) {
                            if (pointsEqual(*it, p)) { cout << 'o'; printed = true; break; }
                        }
                        if (!printed) cout << ' ';
                    }
                }
            }
            cout << '\n';
        }

        Sleep(speedMs);
    }

    clearScreen();
    cout << "Game Over! Final score: " << score << "\n";
    cout << "Press any key to exit..." << endl;
    _getch();
    return 0;
}