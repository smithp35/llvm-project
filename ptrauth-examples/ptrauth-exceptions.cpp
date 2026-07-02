#include <cstdint>
#include <cstdio>

int f3(int x) {
  if (x < 3)
    throw x;
  return x + 4;
}

int f2(int x) {
  return x + f3(x - 1);
}

int f1(int x) {
  return x + f2(x - 1);
}

int main(void) {
  int x = 0;
  try {
    x = f1(3);
  } catch (...) {
    printf("Caught exception\n");
  }
  printf("%d\n", x);
  return 0;
}
