//
// Created by se7enrains on 2/22/20.
//

#include "httphandler.h"

HttpHandler::HttpHandler(tcp::acceptor &acceptor) :
    acceptor(acceptor),
    socket(tcp::socket(acceptor.get_executor())) { }

void HttpHandler::accept() {
    std::cout << "accept started" << std::endl;
    beast::error_code ec;
    socket.close(ec);

    acceptor.async_accept(
            socket,
            [this](boost::beast::error_code ec){
                if (ec) {
                    std::cout << "Error code: " << ec.value() << " " << ec.message() << std::endl;
                    accept();
                } else {
                    std::cout << "Accepted request from " << socket.remote_endpoint().address()
                              << ":" << socket.remote_endpoint().port() << std::endl;
                    readRequest();
                }
            });
}

void HttpHandler::readRequest() {
    std::cout << "readRequest started " << socket.is_open() << std::endl;
    parser.emplace(
            std::piecewise_construct,
            std::make_tuple(),
            std::make_tuple(std::allocator<char>()));

    beast::http::async_read(socket,
                            buffer,
                            *parser,
                            [this](beast::error_code ec, std::size_t){
        if (ec) {
            std::cout << "readRequest failed. Error code: " << ec.value() << " " << ec.message() << std::endl;
            accept();
        } else {
            handleRequest(parser->get());
        }
    });
}

void HttpHandler::handleRequest(const beast::http::request<requestBodyType> &request) {
    std::cout << "handleRequest started" << std::endl;
    switch  (request.method()) {
        case beast::http::verb::post: {
            //get file and save
            rapidjson::Document doc;
            //TO DO
            //Check whether json is valid for parsing
            std::string body = (char*) request.body().data().data();
            body[body.find_last_of('}') + 1] = '\0';
            doc.Parse(body.c_str());
            assert(doc.IsObject());
            //getting path to file and file name
            std::string filePath = std::string("files/").append(doc["file-path"].GetString());
            std::vector<std::string> pathParts;
            boost::split(pathParts, filePath, boost::is_any_of("/"));
            if (pathParts.size() > 1) {
                std::string dirConstructorPath;
                for (int i = 0; i < pathParts.size() - 1; i++) {
                    dirConstructorPath.append(pathParts[i]).append("/");
                    boost::filesystem::create_directory(dirConstructorPath);
                }
            }
            //creating file at specified directory and writing data to it
            std::ofstream saveFileStream(filePath);
            saveFileStream << doc["file-data"].GetString();
            saveFileStream.close();

            //send same file in response
            //reading file
            std::ifstream openFileStream(filePath);
            std::string line;
            std::string sFileData;
            if (openFileStream.is_open()) {
                while (std::getline(openFileStream, line)) {
                    sFileData.append(line).append("\n");
                }
                openFileStream.close();
            }
            //make json
            doc.SetObject();
            rapidjson::Value fileName, fileData;
            fileName.SetString(rapidjson::StringRef(pathParts.back().c_str()));
            fileData.SetString(rapidjson::StringRef(sFileData.c_str()));
            doc.AddMember("file-name", fileName, doc.GetAllocator());
            doc.AddMember("file-data", fileData, doc.GetAllocator());
            rapidjson::StringBuffer stringBuffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
            doc.Accept(writer);
            //generate response
            std::cout << "generate response" << std::endl;
            beast::http::response<beast::http::string_body> response;
            response.result(beast::http::status::ok);
            response.keep_alive(false);
            response.set(beast::http::field::server, "Beast");
            response.set(beast::http::field::content_type, "application/json");
            response.body() = stringBuffer.GetString();
            response.prepare_payload();
            //write to socket
            std::cout << "Write to socket" << std::endl;
            beast::http::response_serializer<boost::beast::http::string_body> responseSerializer(response);
            beast::http::async_write(socket,
                                     responseSerializer,
                                     [this](boost::beast::error_code ec, std::size_t) {
                                         socket.shutdown(tcp::socket::shutdown_send, ec);
                                         accept();
                                     });
        }
        break;
        default: {
            std::cout << "bad response" << std::endl;
            sendBadRespononse(beast::http::status::bad_request,
                              "Invalid request method '" + request.method_string().to_string() +"'\r\n");
        }
    }
}

void HttpHandler::sendBadRespononse(beast::http::status status,
                                    const std::string& error) {
    std::cout << "send bad response" << std::endl;
    beast::http::response<beast::http::string_body> response;
    response.result(status);
    response.keep_alive(false);
    response.set(beast::http::field::server, "Beast");
    response.set(beast::http::field::content_type, "text/plain");
    response.body() = error;
    response.prepare_payload();

    beast::http::response_serializer<boost::beast::http::string_body> responseSerializer(response);
    beast::http::async_write(socket,
                             responseSerializer,
                             [this](boost::beast::error_code ec, std::size_t) {
        socket.shutdown(tcp::socket::shutdown_send, ec);
        accept();
    });
}

void HttpHandler::start() {
    accept();
}
