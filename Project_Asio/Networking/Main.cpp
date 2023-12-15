#include <iostream>
#include <chrono>
/*기존의 Asio의 개발에 있어 기존 타겟이 Win7이였기 때문에 Win7의 값인 0x0601로 되어있어 해당 값을 Win11의 값인 0x0B00으로 바꿔주었다.*/
#if _WIN32
#define _WIN32_WINNT 0x0B00
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

std::vector<char> vBuffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket)
{
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size())
		, [&](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				std::cout << "\n\nRead" << length << "bytes\n\n";
				for (int i = 0; i < length; i++)std::cout << vBuffer[i];

				GrabSomeData(socket);
			}
		}
	);
}

int main()
{
	/* asio 관련 에러 메시지를 받아오는 객체 */
	asio::error_code ec;
	/* asio가 작업을 수행할 context */
	asio::io_context context;

	/* asio가 당장 할 일이없다면 프로그램이 꺼질것이고 가짜 일을 주어 꺼지지 않도록 한다.*/
	asio::io_context::work idleWork(context);

	/* context가 쓰레드 작업을 할 수 있게 만들어준다. */
	std::thread thrContext = std::thread([&]() {context.run(); });

	/*ip 기반으로 접속을 시도할 point 만들기*/
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address(/*"93.184.216.34"*/"51.38.81.49", ec), 80);

	asio::ip::tcp::socket socket(context);

	/* ip 연결 */
	socket.connect(endpoint, ec);

	if (!ec)
	{
		std::cout << "Connected!" << std::endl;
	}
	else
	{
		std::cout << "Failed to connected to Address : \n"<< ec.message() << std::endl;
	}

	if (socket.is_open())
	{
		GrabSomeData(socket);

		/* 웹사이트에 보낼 HTTP코드 작성*/
		std::string sRequest = "GET /index.html HTTP/1.1\r\n""Host: example.com\r\n""Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		/*종료*/
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);
	}
	else
	{

	}


	system("pause");
	return 0;
}