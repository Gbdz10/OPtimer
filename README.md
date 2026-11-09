# OPtimer

A Pomodoro timer written in C++ with raylib.

**Features**
- Three modes: Pomodoro, Short Break, Long Break
- Adjust the time while stopped (press "Unlock" first)
- Task list: finish a task with the Done button
- Chime when time is up, still rings when the window is minimized
- Click sound effect and start button animation

**Requirements**
- CMake 3.16 or newer, a C++17 compiler
- raylib (build from source: https://github.com/raysan5/raylib)

**Build on Linux**

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/OPtimer


**Build for Windows (on Linux, using MinGW)**
Build raylib for Windows first, into `~/raylib-win/src`, then run:

cmake -B build-win -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build-win


**License**
MIT

---

# OPtimer (Tiếng Việt)

Đồng hồ Pomodoro viết bằng C++ và raylib.

**Tính năng**
- Ba chế độ: Pomodoro, Short Break, Long Break
- Chỉnh thời gian khi đang dừng (nhấn "Unlock" để mở khóa)
- Danh sách công việc, hoàn thành bằng nút Done
- Chuông khi hết giờ, vẫn kêu khi thu nhỏ cửa sổ
- Hiệu ứng tiếng click và hoạt ảnh nhấn nút Start

**Yêu cầu**
- CMake 3.16 trở lên, trình biên dịch C++17
- raylib (cài từ nguồn: https://github.com/raysan5/raylib)

**Build trên Linux**

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/OPtimer


**Build bản Windows (trên Linux, dùng MinGW)**
Trước tiên build raylib cho Windows vào thư mục `~/raylib-win/src`, rồi chạy:

cmake -B build-win -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build-win


**Giấy phép**
MIT
