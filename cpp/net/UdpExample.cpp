// UDP client
//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;

//    udp::resolver resolver(io_service);
//    udp::resolver::query query(udp::v4(), argv[1], "daytime");
    int port = 2000;
    std::string ip = "192.168.1.2";
    if (argc > 2) {
    	ip.assign(argv[1]);
    	port = atoi(argv[2]);
    }
    else if (argc > 1) {
    	ip.assign(argv[1]);
//    	std::cout << "ip: " << ip << std::endl;
    }

    udp::endpoint receiver_endpoint;
    receiver_endpoint.address(boost::asio::ip::address::from_string(ip));
    receiver_endpoint.port(port);
    std::cout << "dest ip:" << receiver_endpoint.address().to_string() << ", port:" << receiver_endpoint.port() << std::endl;

    udp::socket socket(io_service);
    socket.open(udp::v4());

    boost::array<char, 1> send_buf  = { 'o' };
    socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

    boost::array<char, 128> recv_buf;
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(
        boost::asio::buffer(recv_buf), sender_endpoint);

    std::cout.write(recv_buf.data(), len);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

// UDP Server
//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

void sendHandler(const boost::system::error_code& error, std::size_t sentBytes) {

}

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;

    int port = 2000;
    if (argc > 1)
    	port = atoi(argv[1]);
    std::cout << "listening port:" << port << std::endl;

    udp::socket socket(io_service, udp::endpoint(udp::v4(), port));
//    udp::socket send_socket(io_service); //
//    send_socket.open(udp::v4());         //
//    udp::endpoint reply_endpoint;

    for (;;)
    {
      boost::array<char, 1> recv_buf;
      udp::endpoint remote_endpoint;
      boost::system::error_code error;
      socket.receive_from(boost::asio::buffer(recv_buf),
          remote_endpoint, 0, error);

      std::cout << recv_buf.data(); std::cout.flush();

      // copy with new port #
//      reply_endpoint.address(remote_endpoint.address());
//      reply_endpoint.port(port);

      if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);

      std::string message = make_daytime_string();

      boost::system::error_code ignored_error;
//      send_socket.send_to(boost::asio::buffer(message), reply_endpoint, 0, ignored_error); // NG
      socket.send_to(boost::asio::buffer(message), remote_endpoint, 0, ignored_error);     // OK, original
//      socket.send_to(boost::asio::buffer(message), reply_endpoint, 0, ignored_error);        // NG why?, new port #
//      socket.async_send_to(boost::asio::buffer(message), reply_endpoint, &sendHandler);        // NG why?, new port #
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

// Example

class DupClient {
public:
	DupClient(boost::asio::io_service& io_service, string ip) : socket(io_service) {
//		cout << __FUNCTION__ << endl;

	    receiver_endpoint.address(boost::asio::ip::address::from_string(ip));
	    receiver_endpoint.port(2000);

	    socket.open(udp::v4());
	}

	virtual ~DupClient() {
//		cout << __FUNCTION__ << endl;
	}

	int printMenu() {
		string cmd = getline();

		return stoi(cmd);
	}

	void sendCommand(int cmd) {
		switch (cmd) {
		// ping
		case 1:
			_sendBuffer[0] = 'o';
			socket.send_to(boost::asio::buffer(_sendBuffer, 1), receiver_endpoint);
			break;
		default:
			return;
		}
	}

	void recvMessage() {
		// recv command
		size_t len = socket.receive_from(boost::asio::buffer(&_recvBuffer, BUFFER_SIZE),
				sender_endpoint);

		if (len > 0) {
			cout << "received: ..." << endl;
		}
	}

private:
	enum {BUFFER_SIZE = 2048};
	unsigned char _sendBuffer[BUFFER_SIZE];
	unsigned char _recvBuffer[BUFFER_SIZE];

    udp::endpoint receiver_endpoint;	// to destination
    udp::endpoint sender_endpoint;		// from destination
    udp::socket socket;
};


int main(int argc, char* argv[]) {
	try {
		boost::asio::io_service io_service;

		string ip;
		if (argc > 1) {
			ip.assign(argv[1]);
		} else {
			cout << "usage: dupClient dupServerIp" << endl;
		}
		cout << "dest ip:" << ip << ", port:" << 2000 << endl;

		DupClient dupClient(io_service, ip);
		while (true) {
			int cmd = dupClient.printMenu();
			if (cmd == 9)
				break;

			// make and send a message
			dupClient.sendCommand(cmd);

			// recv a response message
			dupClient.recvMessage();
		}
	} catch (exception& e) {
		cerr << e.what() << endl;
	}

	return 0;
}
