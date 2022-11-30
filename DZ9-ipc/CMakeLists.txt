cmake_minimum_required(VERSION 3.18)

project(MonitoringDaemon C)

set(MYDAEMON_SRC
        Daemonizer.h            Daemonizer.c
        FileInfoMonitoring.h    FileInfoMonitoring.c)

add_executable(MonitoringDaemon MyDaemon.c ${MYDAEMON_SRC})

add_executable(TestClient TestClient.c)
add_executable(Server Server.c)

include(InstallDaemon.cmake)
