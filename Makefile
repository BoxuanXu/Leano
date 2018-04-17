MX_FLAG=3rdparty/libdmlc.a -I./3rdparty/ -I/usr/local/opencv-2.4.13.2/include/ -L/usr/local/opencv-2.4.13.2/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread

PYTHON2_INC=/usr/include/python2.7


all:
	g++ -std=c++11  -o convert src/convert_main.cpp src/connection_pool.cpp src/convert_mx.cpp src/convert_base.cpp src/config.cpp src/DBOperation.cpp src/common.cpp src/thread_pool.cpp convert_model/MX/im2rec.cc src/http_server.cpp src/http_requests.cpp -I${MYSQLCPPCONN_HOME}/include -L${MYSQLCPPCONN_HOME}/lib -lmysqlcppconn -lmysqlcppconn-static -I3rdparty -I${JSONCPP_HOME}/include -L${JSONCPP_HOME}/libs/linux-gcc-5.4.0/ -ljson_linux-gcc-5.4.0_libmt -DWITH_FILECONF -lfdfsclient ${MX_FLAG} -I${PYTHON2_INC} -lpython2.7 -I. -lpthread -I${CROW_HOME}/include -lboost_system -lcurl
