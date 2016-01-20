project(plasma-containmentactions-customdesktopmenu)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Widgets)
find_package(ECM 1.8.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Plasma KIO)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)


set(customdesktopmenu_SRCS
  customdesktopmenu.cpp
)

add_library(plasma_containmentactions_customdesktopmenu MODULE ${customdesktopmenu_SRCS})

kcoreaddons_desktop_to_json(plasma_containmentactions_customdesktopmenu plasma-containmentactions-customdesktopmenu.desktop)

target_link_libraries(plasma_containmentactions_customdesktopmenu KF5::Plasma KF5::KIOCore KF5::KIOWidgets)

install(TARGETS plasma_containmentactions_customdesktopmenu DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES plasma-containmentactions-customdesktopmenu.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
