# examples/CMakeLists.txt

add_executable(bixit_examples
    decode_rbc_pre_announcement.cpp
)

target_include_directories(bixit_examples PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(bixit_examples PRIVATE bixit)

add_test(NAME BixitExamples COMMAND bixit_examples)
