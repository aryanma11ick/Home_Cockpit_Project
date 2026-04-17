// =============================================
// TOLISS A320Neo LIGHT PANEL (FINAL - CLOSED LOOP)
// =============================================

#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include <windows.h>
#include <string>

// -------- CONFIG --------
static constexpr const char* SERIAL_PORT = "COM7";
static constexpr DWORD BAUD_RATE = CBR_115200;

// -------- GLOBALS --------
static HANDLE hSerial = INVALID_HANDLE_VALUE;
static XPLMFlightLoopID loopID = nullptr;
static std::string serialBuffer;

// -------- COMMANDS --------
static XPLMCommandRef S_up, S_dn;
static XPLMCommandRef N_up, N_dn;
static XPLMCommandRef LL_up, LL_dn;
static XPLMCommandRef RL_up, RL_dn;
static XPLMCommandRef O_up, O_dn;

static XPLMCommandRef B_on, B_off;
static XPLMCommandRef W_on, W_off;
static XPLMCommandRef R_on, R_off;

// -------- DATAREFS (STATE) --------
static XPLMDataRef dr_strobe;
static XPLMDataRef dr_nav;
static XPLMDataRef dr_LL;
static XPLMDataRef dr_RL;
static XPLMDataRef dr_nose;
static XPLMDataRef dr_beacon;
static XPLMDataRef dr_wing;

// -------- TARGET STATES (from Arduino) --------
static int tgtS = -1;
static int tgtN = -1;
static int tgtLL = -1;
static int tgtRL = -1;
static int tgtO = -1;
static int tgtB = -1;
static int tgtW = -1;
static int tgtR = -1;

// -----------------------------------------------
// SERIAL SETUP
// -----------------------------------------------
static bool openSerial() {

    std::string port = "\\\\.\\" + std::string(SERIAL_PORT);

    hSerial = CreateFileA(port.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) return false;

    DCB dcb = {};
    dcb.DCBlength = sizeof(DCB);
    GetCommState(hSerial, &dcb);

    dcb.BaudRate = BAUD_RATE;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

    SetCommState(hSerial, &dcb);

    COMMTIMEOUTS t = {};
    t.ReadIntervalTimeout = MAXDWORD;
    t.ReadTotalTimeoutConstant = 0;
    t.ReadTotalTimeoutMultiplier = 0;
    SetCommTimeouts(hSerial, &t);

    return true;
}

// -----------------------------------------------
// FIND COMMANDS + DATAREFS
// -----------------------------------------------
static void initRefs() {
    static bool done = false;
    if (done) return;

    // Commands
    S_up = XPLMFindCommand("toliss_airbus/lightcommands/StrobeLightUp");
    S_dn = XPLMFindCommand("toliss_airbus/lightcommands/StrobeLightDown");

    N_up = XPLMFindCommand("toliss_airbus/lightcommands/NavLightUp");
    N_dn = XPLMFindCommand("toliss_airbus/lightcommands/NavLightDown");

    LL_up = XPLMFindCommand("toliss_airbus/lightcommands/LLandLightUp");
    LL_dn = XPLMFindCommand("toliss_airbus/lightcommands/LLandLightDown");

    RL_up = XPLMFindCommand("toliss_airbus/lightcommands/RLandLightUp");
    RL_dn = XPLMFindCommand("toliss_airbus/lightcommands/RLandLightDown");

    O_up = XPLMFindCommand("toliss_airbus/lightcommands/NoseLightUp");
    O_dn = XPLMFindCommand("toliss_airbus/lightcommands/NoseLightDown");

    B_on = XPLMFindCommand("toliss_airbus/lightcommands/BeaconOn");
    B_off = XPLMFindCommand("toliss_airbus/lightcommands/BeaconOff");

    W_on = XPLMFindCommand("toliss_airbus/lightcommands/WingLightOn");
    W_off = XPLMFindCommand("toliss_airbus/lightcommands/WingLightOff");

    R_on = XPLMFindCommand("toliss_airbus/lightcommands/TurnoffLightOn");
    R_off = XPLMFindCommand("toliss_airbus/lightcommands/TurnoffLightOff");

    // DataRefs (anim)
    dr_strobe = XPLMFindDataRef("ckpt/oh/strobeLight/anim");
    dr_nav = XPLMFindDataRef("ckpt/oh/navLight/anim");
    dr_LL = XPLMFindDataRef("ckpt/oh/ladningLightLeft/anim");
    dr_RL = XPLMFindDataRef("ckpt/oh/ladningLightRight/anim");
    dr_nose = XPLMFindDataRef("ckpt/oh/taxiLight/anim");
    dr_beacon = XPLMFindDataRef("ckpt/oh/beaconLight/anim");
    dr_wing = XPLMFindDataRef("ckpt/oh/wingLight/anim");

    done = true;
}

// -----------------------------------------------
// PARSE SERIAL → SET TARGETS
// -----------------------------------------------
static void handleCommand(const std::string& line) {

    if (line.size() < 3) return;

    if (line.rfind("LL:", 0) == 0) { tgtLL = atoi(line.substr(3).c_str()); return; }
    if (line.rfind("RL:", 0) == 0) { tgtRL = atoi(line.substr(3).c_str()); return; }

    char id = line[0];
    int val = atoi(line.substr(2).c_str());

    switch (id) {
    case 'S': tgtS = val; break;
    case 'N': tgtN = val; break;
    case 'O': tgtO = val; break;
    case 'B': tgtB = val; break;
    case 'W': tgtW = val; break;
    case 'R': tgtR = val; break;
    }
}

// -----------------------------------------------
// CLOSED LOOP CONTROL
// -----------------------------------------------
void drive3Pos(int target, XPLMDataRef dr,
    XPLMCommandRef up, XPLMCommandRef dn)
{
    if (target < 0 || !dr) return;

    int current = XPLMGetDatai(dr);

    if (current < target) XPLMCommandOnce(up);
    else if (current > target) XPLMCommandOnce(dn);
}

void drive2Pos(int target, XPLMDataRef dr,
    XPLMCommandRef on, XPLMCommandRef off)
{
    if (target < 0 || !dr) return;

    int current = XPLMGetDatai(dr);

    if (current != target) {
        if (target == 1) XPLMCommandOnce(on);
        else XPLMCommandOnce(off);
    }
}

// -----------------------------------------------
// FLIGHT LOOP
// -----------------------------------------------
static float loopFunc(float, float, int, void*) {

    initRefs();

    if (hSerial == INVALID_HANDLE_VALUE) return 0.5f;

    // ---- SERIAL READ ----
    COMSTAT status;
    DWORD errors;
    ClearCommError(hSerial, &errors, &status);

    if (status.cbInQue > 0) {
        char buf[64];
        DWORD read = 0;

        if (ReadFile(hSerial, buf, sizeof(buf) - 1, &read, NULL) && read > 0) {
            buf[read] = '\0';
            serialBuffer += buf;

            size_t pos;
            while ((pos = serialBuffer.find('\n')) != std::string::npos) {
                std::string line = serialBuffer.substr(0, pos);
                serialBuffer.erase(0, pos + 1);
                handleCommand(line);
            }
        }
    }

    // ---- DRIVE SYSTEM ----
    drive3Pos(tgtS, dr_strobe, S_up, S_dn);
    drive3Pos(tgtN, dr_nav, N_up, N_dn);
    drive3Pos(tgtLL, dr_LL, LL_up, LL_dn);
    drive3Pos(tgtRL, dr_RL, RL_up, RL_dn);
    drive3Pos(tgtO, dr_nose, O_up, O_dn);

    drive2Pos(tgtB, dr_beacon, B_on, B_off);
    drive2Pos(tgtW, dr_wing, W_on, W_off);

    // RWY (no dataref → direct)
    if (tgtR == 1) XPLMCommandOnce(R_on);
    else if (tgtR == 0) XPLMCommandOnce(R_off);

    return 0.05f;
}

// -----------------------------------------------
// START / STOP
// -----------------------------------------------
PLUGIN_API int XPluginStart(char* n, char* s, char* d) {

    strcpy_s(n, 256, "ToLiss Light Panel");
    strcpy_s(s, 256, "aryan.toliss.panel");
    strcpy_s(d, 256, "Closed-loop Arduino lights");

    if (!openSerial()) return 0;

    XPLMCreateFlightLoop_t p = {};
    p.structSize = sizeof(p);
    p.callbackFunc = loopFunc;

    loopID = XPLMCreateFlightLoop(&p);
    XPLMScheduleFlightLoop(loopID, -1, true);

    return 1;
}

PLUGIN_API void XPluginStop() {
    if (loopID) XPLMDestroyFlightLoop(loopID);
    if (hSerial != INVALID_HANDLE_VALUE) CloseHandle(hSerial);
}

PLUGIN_API int XPluginEnable() { return 1; }
PLUGIN_API void XPluginDisable() {}
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, int, void*) {}
