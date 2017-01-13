// C 头文件
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Linux / Unix 头文件
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

// 棋盘
class board {
private:
  char *data;
  int currentX, currentY;
public:
  // 为棋盘分配空间，将光标设置在棋盘中心，初始化棋盘数据
  board() {
    data = (char*)malloc(15 * 15);
    currentX = currentY = 7;
    for (int i = 0; i < 15 * 15; i++)
      data[i] = '.';
  }
  // 回收棋盘的空间
  ~board() {
    if (data != NULL)
      free(data);
  }

  // 获取棋盘数据
  char & get(int x, int y) {
    return data[15 * (y % 15) + (x % 15)];
  }

  // 输出棋盘
  void print(bool isBlink) {
    system("clear");
    for (int i = 0; i < 15; i++) {
      for (int j = 0; j < 15; j++) {
        if (i == currentY && j == currentX && isBlink)
          printf("_ ");
        else
          printf("%c ", get(j, i));
      }
      printf("\n");
    }
  }

  // 移动光标
  void moveCur(char dir) {
    switch (dir) {
      char nextX, nextY;
      case 'A':  // 上
        currentY = (currentY + 14) % 15;
        break;
      case 'C':  // 右
        currentX = (currentX + 1) % 15;
        break;
      case 'B':  // 下
        currentY = (currentY + 1) % 15;
        break;
      case 'D':  // 左
        currentX = (currentX + 14) % 15;
        break;
    }
  }

  // 对情形的评估
  int eval(char *data_)  {
    return 10;
  }

  // 对抗树搜索
  int searchTree(char *data_, int depth) {
    int limit;
    limit = depth % 2 == 0 ? INT_MAX : 0;
    for (int i = 0; i < 15; i++)
      for (int j = 0; j < 15; j++)
        // 只遍历处于子的 8-领域 范围内的点
        if (get(j, i) == '.' &&  // 首先确保该点为空，才能落子
            (get(j, i + 14) != '.' || get(j + 1, i + 14) != '.' ||
             get(j + 1, i) != '.' || get(j + 1, i + 1) != '.' ||
             get(j, i + 1) != '.' || get(j + 14, i + 1) != '.' ||
             get(j + 14, i) != '.' || get(j + 14, i + 14) != '.')) {
          char *data__ = (char*)malloc(15 * 15);
          for (int k = 0; k < 15 * 15; k++)  // 拷贝棋盘数据，保证信息安全
            data__[k] = data_[k];
          depth % 2 == 0 ? data__[15 * i + j] = 'B' : data__[15 * i + j] = 'W';
          int value = depth == 1 ? eval(data__) : searchTree(data__, depth - 1);
          if ((depth % 2 == 0 && value < limit) ||
              (depth % 2 != 0 && value > limit))
            limit = value;
        }
    return limit;
  }

  // 对抗树搜索，最外层
  void searchTree() {
    int max = 0, maxX, maxY;
    for (int i = 0; i < 15; i++)
      for (int j = 0; j < 15; j++)
        // 只遍历处于子的 8-领域 范围内的点
        if (get(j, i) == '.' &&  // 首先确保该点为空，才能落子
            (get(j, i + 14) != '.' || get(j + 1, i + 14) != '.' ||
             get(j + 1, i) != '.' || get(j + 1, i + 1) != '.' ||
             get(j, i + 1) != '.' || get(j + 14, i + 1) != '.' ||
             get(j + 14, i) != '.' || get(j + 14, i + 14) != '.')) {
          char *data_ = (char*)malloc(15 * 15);
          for (int k = 0; k < 15 * 15; k++)  // 拷贝棋盘数据，保证信息安全
            data_[k] = data[k];
          data_[15 * i + j] = 'W';
          int value = searchTree(data_, 2);
          if (value > max) {
            max = value;
            maxX = j;
            maxY = i;
          }
        }
    data[15 * maxY + maxX] = 'W';
  }

  bool isPlayerWin() {
    for (int i = 0; i < 15; i++)
      for (int j = 0; j < 15; j++) {
        if (get(j, i) == 'B') {
          // 从左到右
          if (get((j + 13) % 15, i) == 'B' && get((j + 14) % 15, i) == 'B' &&
              get((j + 1) % 15, i) == 'B' && get((j + 2) % 15, i) == 'B')
            return 1;
          // 从上到下
          if (get(j, (i + 13) % 15) == 'B' && get(j, (i + 14) % 15) == 'B' &&
              get(j, (i + 1) % 15) == 'B' && get(j, (i + 2) % 15) == 'B')
            return 1;
          if (get((j + 13) % 15, (i + 13) % 15) == 'B' &&
              get((j + 14) % 15, (i + 14) % 15) == 'B' &&
              get((j + 1) % 15, (i + 1) % 15) == 'B' &&
              get((j + 2) % 15, (i + 2) % 15) == 'B')
            return 1;
        }
      }
    return 0;
  }

  // 落子
  void hit() {
    if (data[15 * currentY + currentX] == '.') {  // 判断落子处是否合法
      data[15 * currentY + currentX] = 'B';
      if (isPlayerWin()) {  // 输出获胜信息，退出程序
        get(3, 7) = 'Y'; get(4, 7) = 'O'; get(5, 7) = 'U'; get(6, 7) = ' ';
        get(7, 7) = 'W'; get(8, 7) = 'I'; get(9, 7) = 'N'; get(10, 7) = '!';
        print(0);
        exit(0);
      }
      // 进行对抗树搜索
      searchTree();
    }
  }
} game;  // 棋盘实例

// 光标闪烁
void* blink(void* args) {
  bool isBlink = 1;
  while (1) {
    game.print(isBlink);
    isBlink = !isBlink;
    usleep(500000);
  }
  return 0;
}

// 实现键盘敲击反馈函数
char getch_() {
  char buf = 0;
  struct termios old = {0};
  if (tcgetattr(0, &old) < 0)
    perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
    perror("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsetattr ~ICANON");
  return buf;
}
// 键盘监听
void* listen(void* args) {
  while (1) {
    char key = getch_();
    if (key == 'A' || key == 'B' || key == 'C' || key == 'D')
      game.moveCur(key);
    else if (key == ' ')
      game.hit();
  }
  return 0;
}

int main() {
  pthread_t tids[2];  // 创建两个线程，一个线程实现光标闪烁，一个线程实现键盘监听
  int ret = pthread_create(&tids[0], NULL, blink, NULL);  // 光标闪烁
  if (ret != 0) {
    printf("pthread_create error: blink\n");
    exit(EXIT_FAILURE);
  }
  ret = pthread_create(&tids[1], NULL, listen, NULL);  // 键盘监听
  if (ret != 0) {
    printf("pthread_create error: listen\n");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < 2; i++)
    pthread_join(tids[i], NULL);  // 等待线程结束

  return 0;
}
