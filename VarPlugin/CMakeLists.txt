# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================

#导入变量 base_util__SOURCES
include("${CMAKE_CURRENT_SOURCE_DIR}/../base_home/base_util.cmake")

set(Var_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarAstAct.cpp"

"${base_util__SOURCES}"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/FnVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/RetVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/UtilBusz.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/ClConst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarDeclVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarTypeFlag.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarTypeDesc.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/RangeHasMacroAstVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/CollectIncMacro_PPCb.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarAstCnsm.cpp"
)


    # Create a library corresponding to 'plugin'
    add_library(
            VarPlugin
      SHARED
      ${Var_SOURCES}
      )

target_link_libraries(VarPlugin
        "${LIBFMT_STATIC}"   #/pubx/fmt/build/libfmt.a
        )

    # Configure include directories for 'plugin'
    target_include_directories(VarPlugin
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../base_home/include"
      "${LIBFMT_INCLUDE}"
    )

