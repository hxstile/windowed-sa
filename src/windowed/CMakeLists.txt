include(FetchContent)

FetchContent_Declare(
    im_mem
    GIT_REPOSITORY  https://github.com/hxstile/im_mem.git
    GIT_TAG         2db21ef954e3eb15718a5d07836fae5438d116df
)

FetchContent_MakeAvailable(im_mem)

target_include_directories(${PROJECT_NAME} PRIVATE ${im_mem_SOURCE_DIR}/include)

file(GLOB_RECURSE ${PROJECT_NAME}_LIST RELATIVE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME} *.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_LIST})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    SUFFIX ".asi"
)
