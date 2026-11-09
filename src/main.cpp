#include "raylib.h"
#include <vector>
#include <string>
#include <cmath>
#include <atomic>
#include <thread>
#include <chrono>

// making the end-session chime
static Sound MakeChime() {
    const int sampleRate = 44100;
    const float step = 0.2f;
    const float noteLen = 0.18f;
    const float freqs[4] = {660.0f, 740.0f, 830.0f, 990.0f};

    const int total = (int)(sampleRate * (step * 3 + noteLen + 0.1f));

    short* samples = (short*)MemAlloc(total * sizeof(short));
    for (int i = 0; i < total; i++) samples[i] = 0;

    for (int n = 0; n < 4; n++) {
        int start = (int)(sampleRate * step * n);
        int len   = (int)(sampleRate * noteLen);
        for (int i = 0; i < len && start + i < total; i++) {
            float t = (float)i / sampleRate;
            float fade = 1.0f - (float)i / len;
            samples[start + i] = (short)(sinf(2.0f * PI * freqs[n] * t) * fade * 12000);
        }
    }

    Wave w = {0};
    w.frameCount = total;
    w.sampleRate = sampleRate;
    w.sampleSize = 16;
    w.channels = 1;
    w.data = samples;

    Sound s = LoadSoundFromWave(w);
    MemFree(samples);
    return s;
}

// making the click sound
static Sound MakeClick() {
    const int sampleRate = 44100;
    const float freq = 250.0f;
    const float len = 0.06f;
    const int total = (int)(sampleRate * len);

    short* samples = (short*)MemAlloc(total * sizeof(short));
    for (int i = 0; i < total; i++) {
        float t = (float)i / sampleRate;
        float env = 1.0f - (float)i / total;
        env = env * env;
        samples[i] = (short)(sinf(2.0f * PI * freq * t) * env * 10000);
    }

    Wave w = {0};
    w.frameCount = total;
    w.sampleRate = sampleRate;
    w.sampleSize = 16;
    w.channels = 1;
    w.data = samples;

    Sound s = LoadSoundFromWave(w);
    MemFree(samples);
    return s;
}

enum Mode { POMODORO = 0, SHORT_BREAK, LONG_BREAK };

static const char* kLabels[3] = {"Pomodoro", "Short Break", "Long Break"};
int kDurations[3] = {25 * 60, 5 * 60, 15 * 60};

// Center text follow X
static void DrawCentered(const char* text, int centerX, int y, int size, Color color) {
    int w = MeasureText(text, size);
    DrawText(text, centerX - w / 2, y, size, color);
}

// doing the text cursor trick
static bool CursorVisible() {
    return fmod(GetTime(), 1.0) < 0.5;
}

// fixing the end_chime error when minimize app
static Sound gChime;
static std::atomic<int> gTimerGen{0};

static void ChimeWorker(int myGen, double seconds) {
    auto deadline = std::chrono::steady_clock::now()
                  + std::chrono::duration<double>(seconds);
    while (gTimerGen.load() == myGen) {
        if (std::chrono::steady_clock::now() >= deadline) {
            if (gTimerGen.load() == myGen) PlaySound(gChime);
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {

    const int screenW = 420;
    const int screenH = 720;

    InitWindow(screenW, screenH, "OPtimer");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    InitAudioDevice();
    gChime = MakeChime();
    Sound click = MakeClick();

    // default states
    Mode mode = POMODORO;
    double endTime = 0.0;
    float remaining = (float)kDurations[mode];
    bool running = false;
    bool lock = true;
    bool dark = false;
    int session = 1;
    
    // colors pallete
    const Color bg_1      = {175, 74,  73,  255};
    const Color bg_2      = {43,  115, 121, 255};
    const Color bg_3      = {48,  106, 149, 255};
    const Color tab_1     = {156, 78,  78,  255};
    const Color tab_2     = {59,  109, 115, 255};
    const Color tab_3     = {61,  103, 136, 255};
    const Color card_1    = {184, 92,  91,  255};
    const Color card_2    = {68,  129, 135, 255};
    const Color card_3    = {73,  121, 159, 255};
    const Color softWhite = {255, 255, 255, 170};
    const Color bg_d      = {1,   1,   0,   255};
    const Color tab_d     = {22,  22,  22,  255};
    const Color card_d    = {26,  26,  27,  255};

    Rectangle startBtn    = {(float)(screenW / 2 - 100), 250, 200, 60};
    Rectangle optMinus    = {40,  260, 50,  40};
    Rectangle optPlus     = {330, 260, 50,  40};
    Rectangle lockTimer   = {20,  340, 85,  38};
    Rectangle stop        = {315, 340, 85,  38};
    Rectangle plusBtn     = {340, 512, 36,  36};
    Rectangle inputBox    = {35,  580, 350, 44};
    Rectangle taskCard    = {35,  580, 350, 56};
    Rectangle doneBtn     = {305, 592, 70,  32};
    Rectangle darkMode    = {150, 340, 120, 38};


    // main timer background
    Rectangle card = {20, 20, 380, 310};

    // 3 buttons tab
    Rectangle tabRects[3];
    for (int i = 0; i < 3; i++) {
        tabRects[i] = Rectangle{(float)(26 + i * 125), 25, 118, 36};
    }

    std::vector<std::string> tasks;
    bool addingTask = false;

    std::string taskInput;
    const int kMaxTaskLen = 20;

    while (!WindowShouldClose()) {
        // timer part
        int secs = (int)std::ceil(remaining);

        // making the start button animation
        startBtn.y = running ? 253.0f : 250.0f;

        // Updater
        if (running) {
            remaining = (float)(endTime - GetTime());
            if (remaining <= 0.0f) {
                remaining = 0.0f;
                running = false;
                if (mode == POMODORO) session++;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetMousePosition();

            // check to change mode
            for (int i = 0; i < 3; i++) {
                if (CheckCollisionPointRec(mouse, tabRects[i]) && !running) {
                    mode = (Mode)i;
                    remaining = (float)kDurations[mode];
                    running = false;
                }
            }

            // check to start/stop
            if (CheckCollisionPointRec(mouse, startBtn)) {
                if (remaining <= 0.0f) remaining = (float)kDurations[mode];
                if (!running) {
                    endTime = GetTime() + remaining;
                    int myGen = ++gTimerGen;
                    std::thread(ChimeWorker, myGen, (double)remaining).detach();
                } else {
                    remaining = (float)(endTime - GetTime());
                    if (remaining < 0.0f) remaining = 0.0f;
                    ++gTimerGen;
                }
                lock ? lock = true : lock = true;
                PlaySound(click);
                running = !running;
            }

            if (CheckCollisionPointRec(mouse, stop)) if (remaining != (float)kDurations[mode] && !running) remaining = (float)kDurations[mode];

            // lock the adjust buttons by default
            if (CheckCollisionPointRec(mouse, lockTimer)) if (!running) lock = !lock;
            if (CheckCollisionPointRec(mouse, darkMode))  if (!running) dark = !dark;

            // check for timer adjustments
            if (CheckCollisionPointRec(mouse, optPlus)) {
                if (!lock && !running) PlaySound(click);
                if (mode == POMODORO    && !lock && remaining <= 60*60 && !running && secs <= 55*60) remaining += 5*60;
                if (mode == SHORT_BREAK && !lock && remaining <= 10*60 && !running && secs <= 9*60)  remaining += 1*60;
                if (mode == LONG_BREAK  && !lock && remaining <= 20*60 && !running && secs <= 19*60) remaining += 1*60;
            }

            if (CheckCollisionPointRec(mouse, optMinus)) {
                if (!lock && !running) PlaySound(click);
                if (mode == POMODORO    && !lock && remaining >= 15*60 && !running && secs >= 20*60) remaining -= 5*60;
                if (mode == SHORT_BREAK && !lock && remaining >= 3*60  && !running && secs >= 4*60)  remaining -= 1*60;
                if (mode == LONG_BREAK  && !lock && remaining >= 15*60 && !running && secs >=16*60)  remaining -= 1*60;
            }

            if (CheckCollisionPointRec(mouse, plusBtn) && !running) {
                addingTask = !addingTask;
                taskInput.clear();
            }

            if (!tasks.empty() && !addingTask && CheckCollisionPointRec(mouse, doneBtn)) tasks.erase(tasks.begin());
        }

        if (addingTask) {
            int ch = GetCharPressed();
            while (ch > 0) {
                bool printable = ch >= 32 && ch <= 126;   // ASCII only
                if (printable && (int)taskInput.size() < kMaxTaskLen) {
                    taskInput += (char)ch;
                }
                ch = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !taskInput.empty()) taskInput.pop_back();

            if (IsKeyPressed(KEY_ENTER) && !taskInput.empty()) {
                tasks.push_back(taskInput);
                taskInput.clear();
                addingTask = false;
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                taskInput.clear();
                addingTask = false;
            }
        }

        BeginDrawing();

        const char* timeText = TextFormat("%02d:%02d", secs / 60, secs % 60);

        Color bgcolor = mode == POMODORO ? bg_1   : mode == SHORT_BREAK ? bg_2   : bg_3;
        Color cardbg  = mode == POMODORO ? card_1 : mode == SHORT_BREAK ? card_2 : card_3;
        Color tabsbg  = mode == POMODORO ? tab_1  : mode == SHORT_BREAK ? tab_2  : tab_3;

        if (dark) {
            bgcolor = bg_d;
            cardbg  = card_d;
            tabsbg  = tab_d;
        }

        // button colors base on running
        Color btnBg   = running ? tabsbg    : cardbg;
        Color fontBtn = running ? softWhite : WHITE;

        ClearBackground (bgcolor);
        DrawRectangleRounded(card, 0.06f, 12, cardbg);

        // upper tabs
        for (int i = 0; i < 3; i++) {
            if (i == (int)mode) {
                DrawRectangleRounded(tabRects[i], 0.25f, 8, tabsbg);
            }
            DrawCentered(kLabels[i], (int)(tabRects[i].x + tabRects[i].width / 2), (int)(tabRects[i].y + 9), 18, WHITE);
        }

        // countdown
        DrawCentered(timeText, screenW / 2, 105, 110, WHITE);

        // start pause button /w shadow
        Rectangle shadow = startBtn;
        shadow.y += 5;

        if (!running) DrawRectangleRounded(shadow, 0.1f, 8, Color{220, 220, 220, 255});
        DrawRectangleRounded(startBtn, 0.1f, 8, WHITE);
        DrawCentered(running ? "PAUSE" : "START", (int)(startBtn.x + startBtn.width / 2), (int)(startBtn.y + 16), 30, tabsbg);

        // sessions numbers n notification
        DrawCentered(TextFormat("#%d", session), screenW / 2, 440, 20, softWhite);

        const char* msg;
        if (remaining <= 0.0f) msg = "Session complete!";
        else if (mode == POMODORO) msg = "Time to focus!";
        else msg = "Time for a break!";
        DrawCentered(msg, screenW / 2, 472, 24, WHITE);

        // tasks
        DrawText("Tasks", 35, 520, 24, WHITE);
        DrawRectangle(35, 565, 350, 2, WHITE);

        // adjust timer buttons
        DrawRectangleRounded(optPlus, 0.1f, 8, tabsbg);
        DrawRectangleRounded(optMinus, 0.1f, 8, tabsbg);
        DrawCentered("+", (int)(optPlus.x  + optPlus.width  / 2), (int)(optPlus.y),  45, WHITE);
        DrawCentered("-", (int)(optMinus.x + optMinus.width / 2), (int)(optMinus.y), 45, WHITE);

        // lock button
        DrawRectangleRounded(lockTimer, 0.3f, 8, btnBg);
        DrawCentered(lock ? "Unlock" : "Lock", (int)(lockTimer.x + lockTimer.width / 2), (int)(lockTimer.y + lockTimer.height / 2 - 7), 18, fontBtn);
        DrawRectangleRounded(stop, 0.3f, 8, btnBg);
        DrawCentered("Stop", (int)(stop.x + stop.width / 2), (int)(stop.y + stop.height / 2 - 7), 18, fontBtn);
        DrawRectangleRounded(darkMode, 0.3f, 8, btnBg);
        DrawCentered("Toggle Dark", (int)(darkMode.x + darkMode.width / 2), (int)(darkMode.y + darkMode.height / 2 - 7), 18, fontBtn);

        DrawRectangleRounded(plusBtn, 0.3f, 8, cardbg);
        DrawCentered("+", (int)(plusBtn.x + plusBtn.width / 2), (int)(plusBtn.y + 2), 30, WHITE);

        if (addingTask) {
            // typing box
            DrawRectangleRounded(inputBox, 0.2f, 8, cardbg);
            DrawText(taskInput.c_str(), (int)inputBox.x + 12, (int)inputBox.y + 12, 20, WHITE);
            DrawText(TextFormat("%d/%d", (int)taskInput.size(), kMaxTaskLen), (int)(inputBox.x + inputBox.width) - 60, (int)inputBox.y + 52, 14, softWhite);

            // typing cursor
            if (CursorVisible()) {
                int textW = MeasureText(taskInput.c_str(), 20);
                int cursorX = (int)inputBox.x + 12 + textW + 2;
                int cursorY = (int)inputBox.y + 10;
                DrawRectangle(cursorX, cursorY, 2, 24, WHITE);
            }
        }

        else if (!tasks.empty()) {
            // current task card
            const std::string& current = tasks.front();
            DrawRectangleRounded(taskCard, 0.2f, 8, cardbg);
            DrawText(current.c_str(), (int)taskCard.x + 14, (int)taskCard.y + 18, 20, WHITE);

            DrawRectangleRounded(doneBtn, 0.3f, 8, WHITE);
            DrawCentered("Done", (int)(doneBtn.x + doneBtn.width / 2), (int)(doneBtn.y + 7), 18, tabsbg);

            // task(s) number left
            int more = (int)tasks.size() - 1;
            if (more > 0) DrawCentered(TextFormat("%d more task(s) left, keep going!", more), screenW / 2, 660, 18, softWhite);
        }

        else DrawCentered("No tasks yet. Press + to add one.", screenW / 2, 600, 18, softWhite);
        DrawText("Made by Gbeo68 and Claude (free model) /w <3", 90, 700, 10, softWhite);

        EndDrawing();
    }

    ++gTimerGen;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    UnloadSound(gChime);
    UnloadSound(click);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}