// C 头文件
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  char & get(int x, int y, char *data_) {
    return data_[15 * (y % 15) + (x % 15)];
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
    print(1);
  }

  // 判断玩家是否获胜
  bool isWin(char s) {
    for (int i = 0; i < 15; i++)
      for (int j = 0; j < 15; j++)
        if (get(j, i) == s) {
          // 从左到右
          if (get(j + 13, i) == s && get(j + 14, i) == s &&
              get(j + 1, i) == s && get(j + 2, i) == s)
            return 1;
          // 从上到下
          if (get(j, i + 13) == s && get(j, i + 14) == s &&
              get(j, i + 1) == s && get(j, i + 2) == s)
            return 1;
          // 从左上到右下
          if (get(j + 13, i + 13) == s &&
              get(j + 14, i + 14) == s &&
              get(j + 1, i + 1) == s &&
              get(j + 2, i + 2) == s)
            return 1;
          // 从右上到左下
          if (get(j + 2, i + 13) == s &&
              get(j + 1, i + 14) == s &&
              get(j + 14, i + 1) == s &&
              get(j + 13, i + 2) == s)
            return 1;
        }
    return 0;
  }

  // 对模式的评估
  int patternEval(char *pattern) {
    char modify[8] = {'.', '.', '.', '.', '.', '.', '.', '\0'};
    int tempVal = 0;
    for (int i = 0; i < 7; i++) {
      if (pattern[i] == pattern[3])
        modify[i] = 'Y';
      else if (pattern[i] != '.')
        modify[i] = 'N';
    }
    if (strstr(modify, ".YYYY.") != NULL || strstr(modify, "YYYYY"))
      tempVal = 6000;
    else if (strstr(modify, "YYYY.") != NULL || strstr(modify, ".YYYY") != NULL)
      tempVal = 50;
    else if (strstr(modify, "YYY.Y") != NULL || strstr(modify, "Y.YYY") != NULL)
      tempVal = 60;
    else if (strstr(modify, "YY.YY") != NULL)
      tempVal = 52;
    else if (strstr(modify, "..YYY..") != NULL)
      tempVal = 60;
    else if (strstr(modify, "YYY..") != NULL || strstr(modify, "..YYY") != NULL)
      tempVal = 10;
    else if (strstr(modify, ".Y.YY.") != NULL ||
             strstr(modify, ".YY.Y.") != NULL)
      tempVal = 16;
    else if (strstr(modify, "Y..YY") != NULL || strstr(modify, "YY..Y") != NULL)
      tempVal = 12;
    else if (strstr(modify, "Y.Y.Y") != NULL)
      tempVal = 11;
    else if (strstr(modify, "...YY..") != NULL ||
             strstr(modify, "..YY...") != NULL)
      tempVal = 13;
    else if (strstr(modify, "YY...") != NULL)
      tempVal = 3;
    else if (strstr(modify, "..Y.Y..") != NULL)
      tempVal = 5;
    else if (strstr(modify, ".Y..Y.") != NULL)
      tempVal = 4;
    if (pattern[3] == 'B')
      return -tempVal;
    else
      return tempVal;
  }

  // 对情形的评估
  int eval(char *data_) {
    int value = 0;
    for (int i = 0; i < 15; i++)
      for (int j = 0; j < 15; j++)
        if (get(j, i, data_) == 'B' || get(j, i, data_) == 'W') {
          // 用字符串进行模式匹配
          char pattern[8];
          pattern[3] = get(j, i, data_);
          pattern[7] = '\0';
          // 从左到右
          pattern[0] = get(j + 12, i, data_);
          pattern[1] = get(j + 13, i, data_);
          pattern[2] = get(j + 14, i, data_);
          pattern[4] = get(j + 1, i, data_);
          pattern[5] = get(j + 2, i, data_);
          pattern[6] = get(j + 3, i, data_);
          value += patternEval(pattern);
          // 从上到下
          pattern[0] = get(j, i + 12, data_);
          pattern[1] = get(j, i + 13, data_);
          pattern[2] = get(j, i + 14, data_);
          pattern[4] = get(j, i + 1, data_);
          pattern[5] = get(j, i + 2, data_);
          pattern[6] = get(j, i + 3, data_);
          value += patternEval(pattern);
          // 从左上到右下
          pattern[0] = get(j + 12, i + 12, data_);
          pattern[1] = get(j + 13, i + 13, data_);
          pattern[2] = get(j + 14, i + 14, data_);
          pattern[4] = get(j + 1, i + 1, data_);
          pattern[5] = get(j + 2, i + 2, data_);
          pattern[6] = get(j + 3, i + 3, data_);
          value += patternEval(pattern);
          // 从右上到左下
          pattern[0] = get(j + 3, i + 12, data_);
          pattern[1] = get(j + 2, i + 13, data_);
          pattern[2] = get(j + 1, i + 14, data_);
          pattern[4] = get(j + 14, i + 1, data_);
          pattern[5] = get(j + 13, i + 2, data_);
          pattern[6] = get(j + 12, i + 3, data_);
          value += patternEval(pattern);
        }
    return value;
  }

  // 对抗树搜索
  int searchTree(int depth, int a, int b, char *data_) {
    // 叶子节点直接返回估值
    if (depth == 0)
      return eval(data_);
    int limit = depth % 2 == 1 ? INT_MAX : INT_MIN;
    int limitX, limitY;
    bool flag = 1;  // 用于一次性 break 两重循环
    for (int i = 0; i < 15 && flag; i++)
      for (int j = 0; j < 15 && flag; j++)
        // 只遍历处于子的 8-领域 范围内的点
        if (get(j, i, data_) == '.' &&  // 首先确保该点为空，才能落子
            (get(j, i + 14, data_) != '.' || get(j + 1, i + 14, data_) != '.' ||
             get(j + 1, i, data_) != '.' || get(j + 1, i + 1, data_) != '.' ||
             get(j, i + 1, data_) != '.' || get(j + 14, i + 1, data_) != '.' ||
             get(j + 14, i, data_) != '.' ||
             get(j + 14, i + 14, data_) != '.')) {
          char *data__ = (char*)malloc(15 * 15);
          for (int k = 0; k < 15 * 15; k++)  // 拷贝棋盘数据，保证信息安全
            data__[k] = data_[k];
          depth % 2 == 1 ? data__[15 * i + j] = 'B' : data__[15 * i + j] = 'W';
          int tempVal = searchTree(depth - 1, a, b, data__);
          if (depth % 2 == 0) {
            if (limit < tempVal) {
              limit = tempVal;
              limitX = j; limitY = i;
            }
            if (a < limit) a = limit;
          } else {
            if (limit > tempVal) {
              limit = tempVal;
              limitX = j; limitY = i;
            }
            if (b > limit) b = limit;
          }
          if (b <= a) flag = 0;
        }
    depth % 2 == 1 ? data_[15 * limitY + limitX] = 'B' :
                     data_[15 * limitY + limitX] = 'W';
    return limit;
  }

  // 落子
  void hit() {
    if (data[15 * currentY + currentX] == '.') {  // 判断落子处是否合法
      data[15 * currentY + currentX] = 'B';
      if (isWin('B')) {  // 输出获胜信息，退出程序
        get(3, 7) = 'Y'; get(4, 7) = 'O'; get(5, 7) = 'U'; get(6, 7) = ' ';
        get(7, 7) = 'W'; get(8, 7) = 'I'; get(9, 7) = 'N'; get(10, 7) = '!';
        print(0);
        exit(0);
      }
      // 进行对抗树搜索
      searchTree(4, INT_MIN, INT_MAX, data);
      if (isWin('W')) {  // 输出获胜信息，退出程序
        get(3, 7) = 'Y'; get(4, 7) = 'O'; get(5, 7) = 'U'; get(6, 7) = ' ';
        get(7, 7) = 'L'; get(8, 7) = 'O'; get(9, 7) = 'S'; get(10, 7) = 'E';
        get(11, 7) = '!';
        print(0);
        exit(0);
      }
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
