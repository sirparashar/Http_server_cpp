#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

using std::cout;

int main()
{
    try
    {
        net::io_context ioc;

        tcp::acceptor acceptor{ioc, {tcp::v4(), 8080}};

        cout << "server staerted on port 8080\n";

        while (true)
        {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            cout << "new connection\n";
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            cout << "Request method: " << req.method_string() << "\n";
            cout << "Request target: " << req.target() << "\n";

            http::response<http::string_body> response{
                http::status::ok, req.version()};

            response.set(http::field::server, "Boost.Beast");
            response.set(http::field::content_type, "text/plain");
            response.body() = "hello world";
            response.prepare_payload();

            http::write(socket, response);

            socket.shutdown(tcp::socket::shutdown_send);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "error: " << e.what() << '\n';
    }
    return 0;
}