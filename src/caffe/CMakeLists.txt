# generate protobuf sources
file(GLOB proto_files proto/*.proto)
caffe_protobuf_generate_cpp_py(${proto_gen_folder} proto_srcs proto_hdrs proto_python ${proto_files})

# include python files either to force generation
add_library(proto STATIC ${proto_hdrs} ${proto_srcs} ${proto_python})
set(Caffe_LINKER_LIBS proto ${Caffe_LINKER_LIBS}) # note, crucial to prepend!
caffe_default_properties(proto)

# --[ Caffe library


file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(caffe STATIC ${srcs})
target_link_libraries(caffe proto ${Caffe_LINKER_LIBS})
caffe_default_properties(caffe)

#INSTALL(TARGETS caffe DESTINATION lib)
