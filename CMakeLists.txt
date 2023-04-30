cmake_minimum_required(VERSION 3.16)

project(AndroidContactsApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

qt_add_executable(appAndroidContactsApp
    main.cpp
    contactsmodel.cpp
    contactsmodel.h
    contact.cpp
    android/src/com/test/ContactsActivity.java
    android/AndroidManifest.xml
)

qt_add_qml_module(appAndroidContactsApp
    URI AndroidContactsApp
    VERSION 1.0
    QML_FILES Main.qml
)

set_target_properties(appAndroidContactsApp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

target_link_libraries(appAndroidContactsApp
    PRIVATE Qt6::Quick
)

install(TARGETS appAndroidContactsApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
