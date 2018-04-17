PROJECT(convert)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

option(DEBUG "set debug " OFF)
option(WITH_FILECONF "set parameters with config file" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CONFIGURATION_TYPES Release)

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${MYSQLCPPCONN_HOME}/include)

set(CROW_HOME /usr/local/crow)
include_directories(${CROW_HOME}/include)
include_directories(${PYTHON2_INC})

set(OPENCV2_HOME /usr/local/opencv2)
include_directories(${OPENCV2_HOME}/include)

include_directories(/usr/local/python-2.7.13/include/python2.7/)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/)

set(MYSQLCPPCONN_HOME /usr/local/mysqlcppconn)
link_directories(${MYSQLCPPCONN_HOME}/lib)
link_directories(${OPENCV2_HOME}/lib)
link_directories(${PYTHON2_LIB})
link_directories(${PROJECT_SOURCE_DIR}/3rdparty)


link_libraries("opencv_highgui")
link_libraries("opencv_core")
link_libraries("opencv_imgproc")
#link_libraries("opencv_imgcodecs")
link_libraries("mysqlcppconn")
link_libraries("mysqlcppconn-static")
link_libraries("fdfsclient")
link_libraries("pthread")
link_libraries("python2.7")
link_libraries("boost_system")
link_libraries("curl")


if (WITH_FILECONF STREQUAL "ON")
	ADD_DEFINITIONS(-DWITH_FILECONF)
	set(JSONCPP_HOME /usr/local/jsoncpp)
	include_directories(${JSONCPP_HOME}/include)
	link_directories(${JSONCPP_HOME}/libs/linux-gcc-5.4.0/)
	link_libraries("json_linux-gcc-5.4.0_libmt")
endif()


if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O2 -m64 -DOS_LINUX")
endif()

aux_source_directory(./src DIR_SRCS)
add_executable(convert ${DIR_SRCS})

target_link_libraries(convert)
target_link_libraries(convert libdmlc.a)
