

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
#CMAKE_SOURCE_DIR: /pubx/clang-var/

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /pubx/clang-var/tools

#导入变量 base_util__SOURCES
include("${CMAKE_CURRENT_SOURCE_DIR}/../base_home/base_util.cmake")

set(VarAlone_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/ActMain.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../VarAlone/VarMain.cpp"

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
    add_executable(
        VarAlone
      ${VarAlone_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
    VarAlone
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../base_home/include"
    "${LIBFMT_INCLUDE}"
    )

    # Link in the required libraries
    target_link_libraries(
    VarAlone
      "clangTooling"
   "${LIBFMT_STATIC}"  #/pubx/fmt/build/libfmt.a
    )
