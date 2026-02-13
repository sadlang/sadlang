# ╔════════════════════════════════════════════════════════════════════╗
# ║     CMakeLists.txt للكود الأصلي - مُولَّد تلقائياً بواسطة لغة ص      ║
# ╚════════════════════════════════════════════════════════════════════╝

cmake_minimum_required(VERSION 3.22.1)
project("test_app")

add_library(sad_app SHARED
    sad_bridge.cpp
)

target_link_libraries(sad_app
    android
    log
)
