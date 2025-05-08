find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(
  ${Boost_INCLUDE_DIR}
)

add_executable(eblind_dlc eblind_dlc.cpp)
target_link_libraries(eblind_dlc watermark ${Boost_LIBRARIES})

add_executable(eblind_dlc_dctdniz_robust eblind_dlc_dctdniz_robust.cpp)
target_link_libraries(eblind_dlc_dctdniz_robust watermark ${Boost_LIBRARIES})

add_executable(dct_watermark dct_watermark.cpp)
target_link_libraries(dct_watermark watermark ${Boost_LIBRARIES})

add_executable(color_to_gray color_to_gray.cpp)
target_link_libraries(color_to_gray watermark ${Boost_LIBRARIES})

add_executable(e_perc_shape e_perc_shape.cpp)
target_link_libraries(e_perc_shape watermark ${Boost_LIBRARIES})

add_executable(change_lsb change_lsb.cpp)
target_link_libraries(change_lsb watermark ${Boost_LIBRARIES})
