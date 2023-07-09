// Shamelessly copy pasted how to listen to keyboard events in C
// from https://stackoverflow.com/a/29723614/14837948 😅

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

static struct termios oldSettings;

void kbcleanup(void) {
  tcsetattr(0, TCSAFLUSH, &oldSettings); /* restore old settings */
}

void kbsetup(void) {
  tcgetattr(0, &oldSettings);

  struct termios newSettings = oldSettings;

  newSettings.c_lflag &= ~ICANON; /* disable line-at-a-time input */
  newSettings.c_lflag &= ~ECHO;   /* disable echo */
  newSettings.c_cc[VMIN] = 0;     /* don't wait for characters */
  newSettings.c_cc[VTIME] = 0;    /* no minimum wait time */

  if (tcsetattr(0, TCSAFLUSH, &newSettings) == 0) {
    atexit(
        kbcleanup); /* restore the terminal settings when the program exits */
  } else {
    fprintf(stderr, "Unable to set terminal mode\n");
    exit(1);
  }
}

int getkey(void) {
  char c;

  if (read(STDIN_FILENO, &c, 1) == 0)
    return '\0';
  else
    return c;
}

int main(void) {
  int c;

  kbsetup();

  time_t start = time(NULL);
  time_t previous = start;
  for (;;) {
    usleep(1000);
    time_t current = time(NULL);

    if (current != previous) {
      previous = current;
    } else if ((c = getkey()) != '\0') {

      if (c == 27) {
        c = getkey();
        if (c == 91) {
          c = getkey();
          if (c == 65) {
            system("adb shell input keyevent 19");
          } else if (c == 66) {
            system("adb shell input keyevent 20");
          } else if (c == 67) {
            system("adb shell input keyevent 22");
          } else if (c == 68) {
            system("adb shell input keyevent 21");
          }
        }
      } else if (c == 10) { // Enter Key
        system("adb shell input keyevent 66");
      } else if (c == 127) { // Backspace
        system("adb shell input keyevent 67");
      } else if (c == 32) { // Space
        system("adb shell input keyevent 62");
      } else if (c == 96) { // Escape
        system("adb shell input keyevent 4"); // Back Button
      } else {
        char command[100];
        sprintf(command, "adb shell input text '%c'", c);
        system(command);
      }
    }
  }
}
