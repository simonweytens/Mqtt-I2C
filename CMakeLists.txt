cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Opdracht_MQTT)



add_executable(Opdracht_MQTT_Publish Mqtt_Pub.c)
add_executable(CGI CGI.c)
add_executable(Opdracht_MQTT_Receive Mqtt_receive.c)


include_directories(/usr/include/mariadb /usr/include/mariadb/mysql)

target_link_libraries(Opdracht_MQTT_Publish mariadb  -lpaho-mqtt3c)
target_link_libraries(Opdracht_MQTT_Receive mariadb  -lpaho-mqtt3c)
target_link_libraries(CGI mariadb)