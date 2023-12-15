#include <iostream>
#include <chrono>
/*������ Asio�� ���߿� �־� ���� Ÿ���� Win7�̿��� ������ Win7�� ���� 0x0601�� �Ǿ��־� �ش� ���� Win11�� ���� 0x0B00���� �ٲ��־���.*/
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
	/* asio ���� ���� �޽����� �޾ƿ��� ��ü */
	asio::error_code ec;
	/* asio�� �۾��� ������ context */
	asio::io_context context;

	/* asio�� ���� �� ���̾��ٸ� ���α׷��� �������̰� ��¥ ���� �־� ������ �ʵ��� �Ѵ�.*/
	asio::io_context::work idleWork(context);

	/* context�� ������ �۾��� �� �� �ְ� ������ش�. */
	std::thread thrContext = std::thread([&]() {context.run(); });

	/*ip ������� ������ �õ��� point �����*/
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address(/*"93.184.216.34"*/"51.38.81.49", ec), 80);

	asio::ip::tcp::socket socket(context);

	/* ip ���� */
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

		/* ������Ʈ�� ���� HTTP�ڵ� �ۼ�*/
		std::string sRequest = "GET /index.html HTTP/1.1\r\n""Host: example.com\r\n""Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		/*����*/
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);
	}
	else
	{

	}


	system("pause");
	return 0;
}