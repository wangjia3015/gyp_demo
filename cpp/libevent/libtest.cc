#include <stdio.h>
#include <algorithm>


void change_test() {
  char *buf = NULL;
  size_t bsize = 0;
  int size = getline(&buf, &bsize, stdin);
  printf("original %s\n", buf);
  std::transform(buf, buf + size, buf, rot_13);
  printf("convert %s\n", buf);
  free(buf);
  return 0;
}

int main() {
  return 0;
}
