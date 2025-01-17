

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")


set(runtime_cpp_core__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/runtime_cpp__vars_fn.cpp"
)

set(runtime_cpp__SOURCES
${runtime_cpp_core__SOURCES}
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/TestMain_Cpp.cpp"
)

#本c运行时 依赖模块 runtime_c__TmPnt_ThreadLocal 的头文件路径
set(runtime_c__TmPnt_ThreadLocal__include "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include")
#本c运行时 依赖模块 runtime_c__TmPnt_ThreadLocal 的静态库路径
set(runtime_c__TmPnt_ThreadLocal__staticLib "/fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/build/libclangPlgVar_runtime_c_TmPnt_ThrLcl.a")

#测试main 开始
add_executable(
runtime_cpp__test_main
    ${runtime_cpp__SOURCES}
)

target_include_directories(
runtime_cpp__test_main PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/include"
    "${runtime_c__TmPnt_ThreadLocal__include}"
)

target_link_libraries(
runtime_cpp__test_main
    "${runtime_c__TmPnt_ThreadLocal__staticLib}"
)
#测试main 结束

#静态库 开始
# 添加静态库目标
# clangPlgVar_runtime_cxx 意思是 runtime_cxx__staticLib , 但该这个名字 被 cmd_wrap 中用到了
add_library(clangPlgVar_runtime_cxx STATIC
${runtime_cpp_core__SOURCES}
)

# 设置静态库目标的包含目录
target_include_directories(clangPlgVar_runtime_cxx PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/include"
#本c++运行时 依赖模块 runtime_c__TmPnt_ThreadLocal， 所以依赖 本c++运行时 的应用 一定要添加依赖 runtime_c__TmPnt_ThreadLocal
    "${runtime_c__TmPnt_ThreadLocal__include}"
)

# 设置静态库编译选项
target_compile_options(clangPlgVar_runtime_cxx PRIVATE
-fPIC
)
#静态库 结束