// server
#include <iostream>
#include <thread>

using namespace std;

int main(int argc, char **argv) {

    short port = 502;
    cout << "create MCP Modbus Server, port:" << port << endl;

    ModServer modServer("modHmi", port);
    thread thServer(&ModServer::run,  &modServer);
    cout << "start MCP Modbus Server ... " << endl;
    modServer.start();

    if (thServer.joinable()) {
        thServer.join();
    };

    cout << "not implemented ... " << endl;

    return 0;
}
