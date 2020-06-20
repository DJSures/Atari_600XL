#include <Keyboard.h>

// mapping of keyboard matrix to controller pins
int row_map[8] = {
  0,//yellow [1]
  1,//orange
  2,//red
  3, //brown
  4,//black
  5, //white
  6, //gray
  7 //purple [8]
};

int col_map[9] = {
  8,//blue [9]
  9, //green
  10,//yellow
  16, //ornage
  14,//red
  15, //brown
  A0,//black
  A1, //white
  A2,//gray [17]
};

// mapping of keyboard matrix to keys
const int rowSize = 8;
const int colSize = 9;

int key_codes[rowSize][colSize] = {
  { 0x00,  '6',   0x00,  '5',  '4',  '3',  '2',  '1',  KEY_ESC       },
  { 0x00,  '7',   0x00,  '8',  '9',  '0',  '<',  '>',  KEY_BACKSPACE },
  { 0x00,  'u',   0x00,  'i',  'o',  'p',  '-',  '=',  KEY_RETURN    },
  { 0x00,  'y',   0x00,  't',  'r',  'e',  'w',  'q',  KEY_TAB       },
  { 0x00,  '}',   'j',   'k',  'l',  ';',  '+',  '*',  '{'           },
  { 0x00,  0x00,  'h',   'g',  'f',  'd',  's',  'a',  KEY_CAPS_LOCK },
  { 0x00,  'n',   ' ',   'm',  ',',  '.',  '/',  0x00, 0x00          },
  { 0x00,  0x00,  '~',   'b',  'v',  'c',  'x',  'z',  0x00          }                                                                                                   
};

//current state of keys
int key_states[rowSize][colSize];

//mapping of special keycodes+mods to US layout keycodes+mods
typedef struct
{
  int modifier;
  int key_code;
} key_map_entry;

typedef struct
{
  key_map_entry in;
  key_map_entry out;
} key_mapping;

const int size_special = 20;
key_mapping special[size_special] =
{
  //TOP ROW
  {{KEY_LEFT_SHIFT, '2'},           {KEY_LEFT_SHIFT, '"'}},
  {{KEY_LEFT_SHIFT, '6'},           {KEY_LEFT_SHIFT, '7'}},
  {{KEY_LEFT_SHIFT, '7'},           {0x0, '"'}},
  {{KEY_LEFT_SHIFT, '8'},           {KEY_LEFT_SHIFT, '2'}},
  {{0x0, 0x01},                     {KEY_LEFT_SHIFT, ','}},
  {{0x0, 0x02 },                     {KEY_LEFT_SHIFT, '.'}},
  {{KEY_LEFT_SHIFT, 0x01},          {0x0, KEY_DELETE}}, //there is no CLEAR key on windows keyboards
  {{KEY_LEFT_SHIFT, 0x02},          {0x0, KEY_INSERT}},
  {{KEY_LEFT_SHIFT, KEY_BACKSPACE}, {0x0, KEY_DELETE}},

  //CURSOR BLOCK
  {{KEY_LEFT_CTRL, '-'},      {0x0, KEY_UP_ARROW}},

  {{KEY_LEFT_SHIFT, '='},     {KEY_LEFT_SHIFT, '\\'}},
  {{KEY_LEFT_CTRL, '='},      {0x0, KEY_DOWN_ARROW}},

  {{0x0, 0x03},                        {KEY_LEFT_SHIFT, '='}},
  {{KEY_LEFT_SHIFT, 0x03},          {0x0, '\\'}},
  {{KEY_LEFT_CTRL, 0x03},           {0x0, KEY_LEFT_ARROW}},

  {{0x0, 0x04},                        {KEY_LEFT_SHIFT, '8'}},
  {{KEY_LEFT_SHIFT, 0x04},          {KEY_LEFT_SHIFT, '6'}},
  {{KEY_LEFT_CTRL, 0x04},           {0x0, KEY_RIGHT_ARROW}},

  {{KEY_LEFT_SHIFT, ','},     {0x0, '['}},
  {{KEY_LEFT_SHIFT, '.'},    {0x0, ']'}}
};

void initPins() {

  for (int a = 0; a < rowSize; a++) {

    int pin = row_map[a];
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }

  for (int b = 0; b < colSize; b++)
    pinMode(col_map[b], INPUT_PULLUP);
}

void resetStates() {

  for (int a = 0; a < rowSize; a++)
    for (int b = 0; b < colSize; b++)
      key_states[a][b] = LOW;
}

void readStates() {

  for (int a = 0; a < rowSize; a++) {

    int pinA = row_map[a];
    digitalWrite(pinA, LOW);

    delay(10);

    for (int b = 0; b < colSize; b++) {

      int state = digitalRead(col_map[b]);
      int prev = key_states[a][b];

      if (state == 0) {

        Serial.print(a);
        Serial.print(" ");
        Serial.print(b);
        Serial.print(" ");
        Serial.print(state);
        Serial.println();
      }
      
      if (state != prev)
        notifyChange(a, b, state);

      key_states[a][b] = state;
    }

    digitalWrite(pinA, HIGH);
  }
}

void notifyChange(int a, int b, int state) {

  int modifier = 0;

  if (key_states[7][0] == LOW)
    modifier |= KEY_LEFT_SHIFT;

  if (key_states[4][0] == LOW)
    modifier |= KEY_LEFT_CTRL;

  if (key_states[6][7] == LOW)
    modifier |= KEY_LEFT_GUI;

  int keyCode = key_codes[a][b];

  if (keyCode != 0x00 && state == LOW)
    setKey(keyCode, modifier);
  else
    setKey(0, 0);
}

void setKey(int kc, int modifier) {

  if (kc == 0) {

    Keyboard.releaseAll();
  } else {

    Serial.print("Code: ");
    Serial.println(kc);

//    for (int i = 0; i < size_special; i++)
//      if (kc == special[i].in.key_code && modifier == special[i].in.modifier) {
//
//        Serial.print("modifier: ");
//        Serial.println(modifier);
//
//        kc = special[i].out.key_code;
//
//        modifier = special[i].out.modifier;
//
//        break;
//      }

    Keyboard.press(kc);
    //  Keyboard.press(modifier);
  }
}

void setup() {

  Keyboard.begin();

  Serial.begin(115200);

  initPins();

  resetStates();
}

void loop() {

  readStates();
}
