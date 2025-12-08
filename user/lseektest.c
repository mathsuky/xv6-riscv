#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int fd, pos, n;
  char buf[32];

  // テスト用ファイルを削除
  unlink("ltest.txt");

  printf("=== lseek system call test ===\n\n");

  // 1. ファイルを作成して "Hello" を書き込む
  printf("1. Create file and write 'Hello'\n");
  fd = open("ltest.txt", O_RDWR | O_CREATE);
  if(fd < 0){
    fprintf(2, "open failed\n");
    exit(1);
  }
  write(fd, "Hello", 5);
  printf("   Written: Hello (5 bytes)\n\n");

  // 2. SEEK_SET: 先頭に戻る
  printf("2. lseek(fd, 0, SEEK_SET) - Move to beginning\n");
  pos = lseek(fd, 0, SEEK_SET);
  printf("   New offset: %d\n", pos);
  n = read(fd, buf, 5);
  buf[n] = 0;
  printf("   Read: %s\n\n", buf);

  // 3. SEEK_CUR: 現在位置を取得（オフセット変更なし）
  printf("3. lseek(fd, 0, SEEK_CUR) - Get current offset\n");
  pos = lseek(fd, 0, SEEK_CUR);
  printf("   Current offset: %d\n\n", pos);

  // 4. SEEK_SET: 位置2に移動して上書き
  printf("4. lseek(fd, 2, SEEK_SET) then write 'XYZ'\n");
  lseek(fd, 2, SEEK_SET);
  write(fd, "XYZ", 3);
  lseek(fd, 0, SEEK_SET);
  n = read(fd, buf, 10);
  buf[n] = 0;
  printf("   File content: %s\n\n", buf);

  // 5. SEEK_END: ファイル末尾に移動（サイズ取得）
  printf("5. lseek(fd, 0, SEEK_END) - Get file size\n");
  pos = lseek(fd, 0, SEEK_END);
  printf("   File size: %d bytes\n\n", pos);

  // 6. SEEK_CUR: 相対移動（-3バイト戻る）
  printf("6. lseek(fd, -3, SEEK_CUR) - Move back 3 bytes\n");
  pos = lseek(fd, -3, SEEK_CUR);
  printf("   New offset: %d\n", pos);
  n = read(fd, buf, 10);
  buf[n] = 0;
  printf("   Read from there: %s\n\n", buf);

  // 7. SEEK_END: ファイル拡張（10バイト追加）
  printf("7. lseek(fd, 10, SEEK_END) - Extend file by 10 bytes\n");
  pos = lseek(fd, 10, SEEK_END);
  printf("   New offset: %d\n", pos);
  // 末尾に文字を書き込む
  write(fd, "END", 3);
  // ファイルサイズを確認
  pos = lseek(fd, 0, SEEK_END);
  printf("   New file size: %d bytes\n\n", pos);

  // 8. 拡張部分が0で埋められているか確認
  printf("8. Check that extended area is filled with zeros\n");
  lseek(fd, 5, SEEK_SET);  // 元の"Hello"の直後
  n = read(fd, buf, 10);
  printf("   Bytes at offset 5-14: ");
  for(int i = 0; i < n; i++){
    printf("%d ", (unsigned char)buf[i]);
  }
  printf("\n   (should be zeros)\n\n");

  close(fd);

  // 9. エラーケース: 負のオフセット
  printf("9. Error case: negative offset\n");
  fd = open("ltest.txt", O_RDWR);
  pos = lseek(fd, -100, SEEK_SET);
  printf("   lseek(fd, -100, SEEK_SET) = %d (expected -1)\n\n", pos);
  close(fd);

  printf("=== lseek test completed ===\n");
  exit(0);
}
