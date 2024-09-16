
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 "6.7.0" REQUIRED COMPONENTS Qml Quick)

find_package(EdupalsBase REQUIRED)
find_package(EdupalsN4D REQUIRED)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fexceptions")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -foperator-names")

add_library(edupalsn4dplugin SHARED plugin.cpp variant.cpp)

target_link_libraries (edupalsn4dplugin
    Qt6::Qml
    Qt6::Quick
    Edupals::Base
    Edupals::N4D
)

install(TARGETS edupalsn4dplugin DESTINATION ${QT6_INSTALL_QML}/Edupals/N4D)
install(FILES qmldir DESTINATION ${QT6_INSTALL_QML}/Edupals/N4D)
