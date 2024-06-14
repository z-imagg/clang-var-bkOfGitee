
MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

#本c运行时 依赖模块 runtime_c__TmPnt_ThreadLocal 的头文件路径
set(runtime_c__TmPnt_ThreadLocal__include "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include")
#本c运行时 依赖模块 runtime_c__TmPnt_ThreadLocal 的静态库路径
set(runtime_c__TmPnt_ThreadLocal__staticLib "/fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/build/libclangPlgVar_runtime_c_TmPnt_ThrLcl.a")
#本c运行时   静态库路径
set(runtime_cpp__vars_fn__staticLib "/fridaAnlzAp/clang-var/build/runtime_cpp__vars_fn/libclangPlgVar_runtime_cxx.a")

set(TestIn_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../test_in/test_main.cpp"
    MyClz.cpp
)

add_executable( TestIn
    ${TestIn_SOURCES}
)

target_include_directories( TestIn
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/include"
    "${runtime_c__TmPnt_ThreadLocal__include}"
)

target_link_libraries(  TestIn
#    "${runtime_c__TmPnt_ThreadLocal__staticLib}"
#    "${runtime_cpp__vars_fn__staticLib}"
)