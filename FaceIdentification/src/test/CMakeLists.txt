aux_source_directory (. SRC_LIST)

message(${SRC_LIST})

# add external libraries
find_package(OpenCV REQUIRED)

enable_testing ()
foreach (f ${SRC_LIST})
  string(REGEX REPLACE "[.]cpp" ".bin" BIN ${f})
  add_executable(${BIN} ${f})
  target_link_libraries(${BIN} viplnet ${OpenCV_LIBS})
  target_link_libraries(${BIN} viplnet "/home/muyu/Documents/github/SeetaFaceEngine-master/FaceDetection/build/libseeta_facedet_lib.so") # 这条
  target_link_libraries(${BIN} viplnet "/home/muyu/Documents/github/SeetaFaceEngine-master/FaceAlignment/build/libseeta_fa_lib.so") #这条
endforeach ()
