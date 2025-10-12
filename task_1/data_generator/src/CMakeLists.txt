add_executable(data_generator main.cpp)
target_compile_features(data_generator PRIVATE cxx_std_11)

add_custom_command(TARGET data_generator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/optimal_n_results.txt
        ${CMAKE_BINARY_DIR}/data/
)