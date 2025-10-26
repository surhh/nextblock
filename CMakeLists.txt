cmake_minimum_required(VERSION 3.16)

project(nextblock VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts Xml Multimedia)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(nextblock
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        fieldpainter.h fieldpainter.cpp
        engine/shape.cpp engine/shape.h

        nextblockpainter.h nextblockpainter.cpp


        engine/helper.h
        engine/gamefieldthread.h engine/gamefieldthread.cpp
        scorenamedialog.h scorenamedialog.cpp scorenamedialog.ui
        topscoresdialog.h topscoresdialog.cpp topscoresdialog.ui
        scorefile.h scorefile.cpp
        nextblock.qrc
        aboutkeysdialog.h aboutkeysdialog.cpp aboutkeysdialog.ui
        aboutdialog.h aboutdialog.cpp aboutdialog.ui
        updatemanager/filedownloader.cpp updatemanager/filedownloader.h updatemanager/updatechecker.cpp updatemanager/updatechecker.h
        defsui.h


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET nextblock APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(nextblock SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(nextblock
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# windows build specific (MinGW)
if(WIN32)
    set(PROJECT_WIN_RESOURCE winappicon.rc)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${PROJECT_WIN_RESOURCE})
endif()
# end of windows build specific  (MinGW)

target_link_libraries(nextblock PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::Xml Qt${QT_VERSION_MAJOR}::Multimedia)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.nextblock)
endif()
set_target_properties(nextblock PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS nextblock
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(nextblock)
endif()
