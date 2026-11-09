# OPtimer

Đồng hồ Pomodoro viết bằng c/C++ và raylib.

## Tính năng
- Ba chế độ: Pomodoro, Short Break, Long Break
- Chỉnh thời gian khi đang dừng (nút "unlock" để mở)
- Danh sách công việc, hoàn thành bằng nút Done
- Chuông khi hết giờ, chạy được khi thu nhỏ cửa sổ
- Hiệu ứng tiếng click, hoạt ảnh nhấn nút start 

## Yêu cầu
- CMake 3.16 trở lên, trình biên dịch C++17
- raylib (cài từ nguồn: https://github.com/raysan5/raylib)

## Build trên Linux
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    ./build/OPtimer

## Build bản Windows (trên Linux, dùng MinGW)
Cần build raylib cho Windows trước, rồi trỏ `RAYLIB_WIN_DIR` trong `CMakeLists.txt` tới thư mục đó.
    cmake -B build-win -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build build-win

## Giấy phép
MIT
