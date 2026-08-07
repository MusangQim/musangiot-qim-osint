#define KB_ROWS 6
#define KB_COLS 5

char kb_charsetPages[3][KB_ROWS][KB_COLS] = {
  {
    {'A', 'B', 'C', 'D', 'E'},
    {'F', 'G', 'H', 'I', 'J'},
    {'K', 'L', 'M', 'N', 'O'},
    {'P', 'Q', 'R', 'S', 'T'},
    {'U', 'V', 'W', 'X', 'Y'},
    {'Z', '<', '_', '^', '#'}
  },
  {
    {'a', 'b', 'c', 'd', 'e'},
    {'f', 'g', 'h', 'i', 'j'},
    {'k', 'l', 'm', 'n', 'o'},
    {'p', 'q', 'r', 's', 't'},
    {'u', 'v', 'w', 'x', 'y'},
    {'z', '<', '_', '^', '#'}
  },
  {
    {'1', '2', '3', '4', '5'},
    {'6', '7', '8', '9', '0'},
    {'!', '@', '#', '$', '%'},
    {'&', '*', '(', ')', '-'},
    {'+', '=', '/', '?', '.'},
    {',', '<', '_', '^', '#'}
  }
};

int kb_pageIndex = 0;
int kb_cursorRow = 0;
int kb_cursorCol = 0;
String kb_buffer = "";
const int KB_MAX_LEN = 12;

void kb_drawGrid()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("NAME:");
  display.print(kb_buffer);
  display.print("_");
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

  int startX = 6;
  int startY = 14;
  int spacingX = 24;
  int spacingY = 8;
  int r = 0;
  int c = 0;
  for (r; r < KB_ROWS; r++)
  {
    for (c; c < KB_COLS; c++)
    {
      int x = startX + (c * spacingX);
      int y = startY + (r * spacingY);
      char key = kb_charsetPages[kb_pageIndex][r][c];
      if (r == kb_cursorRow && c == kb_cursorCol)
      {
        display.setCursor(x - 6, y);
        display.print("[");
        display.setCursor(x + 6, y);
        display.print("]");
      }
      display.setCursor(x, y);
      if (key == '<')
        display.print("DL");
      else if (key == '_')
        display.print("SP");
      else if (key == '^')
        display.print("MD");
      else if (key == '#')
        display.print("OK");
      else
        display.print(key);
    }
  }
  display.display();
}