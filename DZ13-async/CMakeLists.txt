project(HTTPServer C)

set(UTILS_SRC
        Utils/HTTP.c    Utils/Utils.c)

set(SERVER_SRC
        Server/Server.c    Server/Worker.c)

add_executable(HTTPServer main.c ${SERVER_SRC} ${UTILS_SRC})
target_link_libraries(HTTPServer PRIVATE -pthread)
target_include_directories(HTTPServer
        PRIVATE
            Utils
            Server)

add_executable(HTTPClient Client.c ${UTILS_SRC})
target_include_directories(HTTPClient
        PRIVATE Utils)
