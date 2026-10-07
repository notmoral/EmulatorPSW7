cmake_minimum_required(VERSION 3.16)

project(EmulatorPSW7 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
    Core
    Network
    Sql
    Test
    Quick
    Qml
    QuickControls2
    QuickDialogs2
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Network
    Sql
    Test
    Quick
    Qml
    QuickControls2
    QuickDialogs2
)

add_executable(EmulatorPSW7
    main.cpp

    src/TcpServer.cpp
    src/TcpServer.h

    src/TcpClient.cpp
    src/TcpClient.h

    src/ScpiParser.cpp
    src/ScpiParser.h

    src/PowerSupply.cpp
    src/PowerSupply.h

    src/Database.cpp
    src/Database.h

    src/HistoryModel.cpp
    src/HistoryModel.h

    src/CommandImporter.cpp
    src/CommandImporter.h
)

qt_add_resources(EmulatorPSW7
    "qml_resources"
    PREFIX "/"
    FILES
        qml/Main.qml
)

target_link_libraries(EmulatorPSW7 PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::QuickDialogs2
)

# PowerSupply unit tests
add_executable(EmulatorPSW7Tests
    tests/tst_PowerSupply.cpp
    src/PowerSupply.cpp
    src/PowerSupply.h
)

target_link_libraries(EmulatorPSW7Tests PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
)

# Database unit tests
add_executable(EmulatorPSW7DatabaseTests
    tests/tst_Database.cpp
    src/Database.cpp
    src/Database.h
)

target_link_libraries(EmulatorPSW7DatabaseTests PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Test
)

# Integration tests
add_executable(EmulatorPSW7IntegrationTests
    tests/tst_Emulator.cpp
)

target_link_libraries(EmulatorPSW7IntegrationTests PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Test
)

include(GNUInstallDirs)

install(TARGETS EmulatorPSW7
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

find_package(Doxygen OPTIONAL_COMPONENTS dot)
