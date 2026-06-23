# mod-sentinelcore
# AzerothCore WotLK observability and audit module.
#
# Created by xtr4ng3.

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/SentinelCore.cpp")
AC_ADD_SCRIPT_LOADER("SentinelCore" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")
