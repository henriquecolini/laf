# laf-dlgs
# Copyright (C) 2024  Igara Studio S.A.

add_library(laf-dlgs file_dialog.cpp)
target_link_libraries(laf-dlgs laf-base)

if(WIN32)
  target_sources(laf-dlgs PRIVATE file_dialog_win.cpp)
elseif(APPLE)
  target_sources(laf-dlgs PRIVATE file_dialog_osx.mm)
else()
  target_sources(laf-dlgs PRIVATE file_dialog_x11.cpp)
endif()
