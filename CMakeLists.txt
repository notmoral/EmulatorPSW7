cmake_minimum_required(VERSION 3.16)

project(EmulatorPSW7 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Test)

add_executable(EmulatorPSW7
    main.cpp
    src/TcpServer.cpp
    src/TcpServer.h
    src/ScpiParser.h
    src/ScpiParser.cpp
    src/PowerSupply.h
    src/PowerSupply.cpp
    tests/tst_PowerSupply.cpp
)

target_link_libraries(EmulatorPSW7 PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

add_executable(EmulatorPSW7Tests
    tests/tst_PowerSupply.cpp
    src/PowerSupply.h
    src/PowerSupply.cpp
)

target_link_libraries(EmulatorPSW7Tests PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
)

include(GNUInstallDirs)
install(TARGETS EmulatorPSW7
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)