// =============================================
// TOLISS A320neo PANEL PLUGIN (FINAL - STABLE SAFE)
// =============================================

#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include <windows.h>
#include <string>

// -------- CONFIG --------
static constexpr const char* PORT_LIGHTS = "COM7";
static constexpr const char* PORT_ALERTS = "COM5";
static constexpr DWORD BAUD_RATE = CBR_115200;

// -------- SERIAL --------
static HANDLE hLights = INVALID_HANDLE_VALUE;
static HANDLE hAlerts = INVALID_HANDLE_VALUE;

static std::string bufLights;
static std::string bufAlerts;

// -------- LOOP --------
static XPLMFlightLoopID loopID = nullptr;

// -------- COMMANDS --------
static XPLMCommandRef S_up, S_dn;
static XPLMCommandRef N_up, N_dn;
static XPLMCommandRef LL_up, LL_dn;
static XPLMCommandRef RL_up, RL_dn;
static XPLMCommandRef O_up, O_dn;

static XPLMCommandRef B_on, B_off;
static XPLMCommandRef W_on, W_off;
static XPLMCommandRef R_on, R_off;

static XPLMCommandRef cmd_warn;
static XPLMCommandRef cmd_caut;
static XPLMCommandRef cmd_chrono;

// -------- DATAREFS --------
static XPLMDataRef dr_strobe, dr_nav, dr_LL, dr_RL, dr_nose;
static XPLMDataRef dr_beacon, dr_wing, dr_rwy;
static XPLMDataRef dr_warn, dr_caut;

// -------- TARGET STATES --------
static int tgtS = -1, tgtN = -1, tgtLL = -1, tgtRL = -1, tgtO = -1;
static int tgtB = -1, tgtW = -1, tgtR = -1;

// -------- TIMERS --------
static float commandTimer = 0.0f;
static float cdS = 0, cdN = 0, cdLL = 0, cdRL = 0, cdO = 0;
static float cdB = 0, cdW = 0, cdR = 0;

// -------- SERIAL OPEN --------
bool openSerial(HANDLE& h, const char* port) {

    std::string p = "\\\\.\\" + std::string(port);

    h = CreateFileA(p.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (h == INVALID_HANDLE_VALUE) {
        XPLMDebugString(("FAILED TO OPEN " + std::string(port) + "\n").c_str());
        return false;
    }

    XPLMDebugString(("OPENED " + std::string(port) + "\n").c_str());

    DCB dcb = {};
    dcb.DCBlength = sizeof(DCB);
    GetCommState(h, &dcb);

    dcb.BaudRate = BAUD_RATE;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

    SetCommState(h, &dcb);

    // 🔥 NON-BLOCKING SERIAL
    COMMTIMEOUTS t = {};
    t.ReadIntervalTimeout = MAXDWORD;
    t.ReadTotalTimeoutConstant = 0;
    t.ReadTotalTimeoutMultiplier = 0;
    SetCommTimeouts(h, &t);

    return true;
}

// -------- INIT --------
void initRefs() {
    static bool done = false;
    if (done) return;

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

    cmd_warn = XPLMFindCommand("sim/annunciator/clear_master_warning");
    cmd_caut = XPLMFindCommand("sim/annunciator/clear_master_caution");
    cmd_chrono = XPLMFindCommand("AirbusFBW/CaptChronoButton");

    dr_strobe = XPLMFindDataRef("ckpt/oh/strobeLight/anim");
    dr_nav = XPLMFindDataRef("ckpt/oh/navLight/anim");
    dr_LL = XPLMFindDataRef("ckpt/oh/ladningLightLeft/anim");
    dr_RL = XPLMFindDataRef("ckpt/oh/ladningLightRight/anim");
    dr_nose = XPLMFindDataRef("ckpt/oh/taxiLight/anim");
    dr_beacon = XPLMFindDataRef("ckpt/oh/beaconLight/anim");
    dr_wing = XPLMFindDataRef("ckpt/oh/wingLight/anim");
    dr_rwy = XPLMFindDataRef("ckpt/oh/rwyTurnOff/anim");

    dr_warn = XPLMFindDataRef("AirbusFBW/MasterWarn");
    dr_caut = XPLMFindDataRef("AirbusFBW/MasterCaut");

    done = true;
}

// -------- SERIAL READ --------
void readSerial(HANDLE h, std::string& buffer, void(*handler)(const std::string&)) {

    if (h == INVALID_HANDLE_VALUE) return;

    char buf[128];
    DWORD bytesRead = 0;

    if (!ReadFile(h, buf, sizeof(buf) - 1, &bytesRead, NULL)) return;
    if (bytesRead == 0) return;

    buf[bytesRead] = '\0';
    buffer += buf;

    size_t pos;
    while ((pos = buffer.find('\n')) != std::string::npos) {

        std::string line = buffer.substr(0, pos);
        buffer.erase(0, pos + 1);

        while (!line.empty() && line.back() == '\r')
            line.pop_back();

        handler(line);
    }
}

// -------- HANDLERS --------
void handleAlerts(const std::string& line) {
    if (line == "W") XPLMCommandOnce(cmd_warn);
    if (line == "C") XPLMCommandOnce(cmd_caut);
    if (line == "T") XPLMCommandOnce(cmd_chrono);
}

void handleLights(const std::string& line) {

    if (line.rfind("LL:", 0) == 0) { tgtLL = atoi(line.substr(3).c_str()); return; }
    if (line.rfind("RL:", 0) == 0) { tgtRL = atoi(line.substr(3).c_str()); return; }

    char id = line[0];
    int v = atoi(line.substr(2).c_str());

    if (id == 'S') tgtS = v;
    if (id == 'N') tgtN = v;
    if (id == 'O') tgtO = v;
    if (id == 'B') tgtB = v;
    if (id == 'W') tgtW = v;
    if (id == 'R') tgtR = v;
}

// -------- SEND LED --------
void sendAlert(const std::string& msg) {
    if (hAlerts == INVALID_HANDLE_VALUE) return;
    DWORD w;
    std::string m = msg + "\n";
    WriteFile(hAlerts, m.c_str(), m.size(), &w, NULL);
}

// -------- DRIVE --------
void drive3(int tgt, XPLMDataRef dr, XPLMCommandRef up, XPLMCommandRef dn) {
    if (tgt < 0 || !dr) return;
    int cur = XPLMGetDatai(dr);
    if (cur < tgt) XPLMCommandOnce(up);
    else if (cur > tgt) XPLMCommandOnce(dn);
}

void drive2(int tgt, XPLMDataRef dr, XPLMCommandRef on, XPLMCommandRef off) {
    if (tgt < 0 || !dr) return;
    int cur = XPLMGetDatai(dr);
    if (cur != tgt) {
        if (tgt) XPLMCommandOnce(on);
        else XPLMCommandOnce(off);
    }
}

// -------- LED --------
static int lastWarn = -1, lastCaut = -1;

void updateLEDs() {
    if (!dr_warn || !dr_caut) return;

    int w = XPLMGetDatai(dr_warn);
    int c = XPLMGetDatai(dr_caut);

    if (w != lastWarn) {
        sendAlert(w ? "LED_WARN_ON" : "LED_WARN_OFF");
        lastWarn = w;
    }

    if (c != lastCaut) {
        sendAlert(c ? "LED_CAUT_ON" : "LED_CAUT_OFF");
        lastCaut = c;
    }
}

// -------- LOOP --------
float loopFunc(float elapsed, float, int, void*) {

    initRefs();

    readSerial(hLights, bufLights, handleLights);
    readSerial(hAlerts, bufAlerts, handleAlerts);

    commandTimer += elapsed;

    cdS += elapsed; cdN += elapsed; cdLL += elapsed; cdRL += elapsed; cdO += elapsed;
    cdB += elapsed; cdW += elapsed; cdR += elapsed;

    if (commandTimer > 0.12f) {

        if (cdS > 0.2f) { drive3(tgtS, dr_strobe, S_up, S_dn); cdS = 0; }
        if (cdN > 0.2f) { drive3(tgtN, dr_nav, N_up, N_dn); cdN = 0; }
        if (cdLL > 0.2f) { drive3(tgtLL, dr_LL, LL_up, LL_dn); cdLL = 0; }
        if (cdRL > 0.2f) { drive3(tgtRL, dr_RL, RL_up, RL_dn); cdRL = 0; }
        if (cdO > 0.2f) { drive3(tgtO, dr_nose, O_up, O_dn); cdO = 0; }

        if (cdB > 0.2f) { drive2(tgtB, dr_beacon, B_on, B_off); cdB = 0; }
        if (cdW > 0.2f) { drive2(tgtW, dr_wing, W_on, W_off); cdW = 0; }
        if (cdR > 0.2f) { drive2(tgtR, dr_rwy, R_on, R_off); cdR = 0; }

        updateLEDs();

        commandTimer = 0.0f;
    }

    return 0.05f;
}

// -------- START --------
PLUGIN_API int XPluginStart(char* n, char* s, char* d) {

    strcpy_s(n, 256, "A320neo Panel");
    strcpy_s(s, 256, "aryan.panel");
    strcpy_s(d, 256, "Lights + Alerts");

    if (!openSerial(hLights, PORT_LIGHTS)) return 0;
    openSerial(hAlerts, PORT_ALERTS);

    XPLMCreateFlightLoop_t p = {};
    p.structSize = sizeof(p);
    p.callbackFunc = loopFunc;

    loopID = XPLMCreateFlightLoop(&p);
    XPLMScheduleFlightLoop(loopID, -1, true);

    return 1;
}

// -------- STOP --------
PLUGIN_API void XPluginStop() {
    if (loopID) XPLMDestroyFlightLoop(loopID);
    if (hLights != INVALID_HANDLE_VALUE) CloseHandle(hLights);
    if (hAlerts != INVALID_HANDLE_VALUE) CloseHandle(hAlerts);
}

PLUGIN_API int XPluginEnable() { return 1; }
PLUGIN_API void XPluginDisable() {}
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, int, void*) {}
