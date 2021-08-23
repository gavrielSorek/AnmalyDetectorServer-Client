cmake_minimum_required(VERSION 3.17)
project(ass3NewTest1)

set(CMAKE_CXX_STANDARD 11)

add_executable(ass3NewTest1  timeseries.h timeseries.cpp
        AnomalyDetector.h anomaly_detection_util.cpp anomaly_detection_util.h
        SimpleAnomalyDetector.cpp SimpleAnomalyDetector.h HybridAnomalyDetector.cpp HybridAnomalyDetector.h minCircle.h
        MainTrain.cpp minCircle.cpp StandardIO.h CLI.cpp CLI.h commands.h
        timeseries.cpp timeseries.h Server.h Server.cpp)


#set(Sources my_lib.cpp)
#set(Headers header_of_my_lib.h)
#add_executable(superlib ${Sources} ${Headers})

find_library(lib -pthread)
target_link_libraries(${CMAKE_PROJECT_NAME} -pthread)
#find_library(lib Ws2_32.li)
#target_link_libraries(${CMAKE_PROJECT_NAME} ws2_32)
