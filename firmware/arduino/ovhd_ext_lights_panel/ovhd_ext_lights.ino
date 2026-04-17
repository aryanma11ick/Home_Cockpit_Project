// =============================================
// A333 OVERHEAD LIGHT PANEL (FINAL)
// =============================================

// -------- 3-POS SWITCHES --------
const int STROBE_UP   = 46;
const int STROBE_DOWN = 48;

const int NAV_UP   = 32;
const int NAV_DOWN = 30;

const int L_LAND_UP   = 44;
const int L_LAND_DOWN = 42;

const int R_LAND_UP   = 40;
const int R_LAND_DOWN = 38;

const int NOSE_UP   = 36;  // TAKEOFF
const int NOSE_DOWN = 34;  // TAXI

// -------- 2-POS SWITCHES --------

const int WING   = 24;
const int BEACON = 26;
const int RWY    = 28;

// -------- STATE MEMORY --------
int lastS  = -1;
int lastN  = -1;
int lastLL = -1;
int lastRL = -1;
int lastO  = -1;

int lastB = -1;
int lastW = -1;
int lastR = -1;

void setup() {
  Serial.begin(115200);

  // 3-pos
  pinMode(STROBE_UP, INPUT_PULLUP);
  pinMode(STROBE_DOWN, INPUT_PULLUP);

  pinMode(NAV_UP, INPUT_PULLUP);
  pinMode(NAV_DOWN, INPUT_PULLUP);

  pinMode(L_LAND_UP, INPUT_PULLUP);
  pinMode(L_LAND_DOWN, INPUT_PULLUP);

  pinMode(R_LAND_UP, INPUT_PULLUP);
  pinMode(R_LAND_DOWN, INPUT_PULLUP);

  pinMode(NOSE_UP, INPUT_PULLUP);
  pinMode(NOSE_DOWN, INPUT_PULLUP);

  // 2-pos
  pinMode(BEACON, INPUT_PULLUP);
  pinMode(WING,   INPUT_PULLUP);
  pinMode(RWY,    INPUT_PULLUP);
}

void loop() {

  // -------- 3-POS --------
  handle3Pos("S",  STROBE_UP, STROBE_DOWN, lastS);
  handle3Pos("N",  NAV_UP, NAV_DOWN, lastN);
  handle3Pos("LL", L_LAND_UP, L_LAND_DOWN, lastLL);
  handle3Pos("RL", R_LAND_UP, R_LAND_DOWN, lastRL);
  handle3Pos("O",  NOSE_UP, NOSE_DOWN, lastO);

  // -------- 2-POS (REVERSED) --------
  handle2Pos("B", BEACON, lastB);
  handle2Pos("W", WING,   lastW);
  handle2Pos("R", RWY,    lastR);

  delay(20);
}

// =============================================
// 3-POS HANDLER
// =============================================
void handle3Pos(const char* label, int pinUp, int pinDown, int &lastState) {
  bool up   = !digitalRead(pinUp);
  bool down = !digitalRead(pinDown);

  int state = -1;

  if (up && !down) state = 2;
  else if (!up && !down) state = 1;
  else if (!up && down) state = 0;

  if (state != lastState && state != -1) {
    lastState = state;
    Serial.print(label);
    Serial.print(":");
    Serial.println(state);
  }
}

// =============================================
// 2-POS HANDLER (REVERSED)
// =============================================
void handle2Pos(const char* label, int pin, int &lastState) {

  // reversed logic
  int state = digitalRead(pin);

  if (state != lastState) {
    lastState = state;
    Serial.print(label);
    Serial.print(":");
    Serial.println(state);
  }
}