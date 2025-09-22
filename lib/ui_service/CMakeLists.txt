add_library(ui_service SHARED
        ServiceWidget.cpp ServiceWidget.h
)

target_include_directories(ui_service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ui_service PRIVATE Qt::Core Qt::Gui Qt::Widgets)

install(TARGETS ui_service
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
)
