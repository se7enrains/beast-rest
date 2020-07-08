//
// Created by se7enrains on 2/22/20.
//

#include "httphandler.h"

HttpHandler::HttpHandler(tcp::acceptor &acceptor)
: acceptor(acceptor), socket(tcp::socket(acceptor.get_executor())) { }

void HttpHandler::accept() {
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

void HttpHandler::handleRequest(const beast::http::request<beast::http::string_body> &request) {
    switch  (request.method()) {
        case beast::http::verb::post: {
            //get file and save
            rapidjson::Document doc;
            std::string body = request.body();
            body[body.find_last_of('}') + 1] = '\0';
            doc.Parse(body.c_str());
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
            std::ofstream saveFileStream(filePath, std::ios::binary);
            std::string data(doc["file-data"].GetString(), doc["file-data"].GetStringLength());
            saveFileStream << data << std::endl;
            saveFileStream.close();

            //send same file in response
            //reading file
            std::ifstream openFileStream(filePath, std::ios::binary);
            std::string line;
            std::string sFileData;
            if (openFileStream.is_open()) {
                std::istreambuf_iterator<char> eos;
                std::istreambuf_iterator<char> iit (openFileStream);
                while (iit!=eos) sFileData+=*iit++;
                openFileStream.close();
            }
            //make json
            doc.SetObject();
            rapidjson::Value fileName, fileData;
            fileName.SetString(rapidjson::StringRef(pathParts.back().c_str()));
            fileData.SetString(rapidjson::StringRef(sFileData.c_str(), sFileData.size()));
            doc.AddMember("file-name", fileName, doc.GetAllocator());
            doc.AddMember("file-data", fileData, doc.GetAllocator());
            rapidjson::StringBuffer stringBuffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
            doc.Accept(writer);
            //generate response
            response = beast::http::response<beast::http::string_body>();
            response.result(beast::http::status::ok);
            response.keep_alive(false);
            response.set(beast::http::field::server, "Beast");
            response.set(beast::http::field::content_type, "application/json");
            response.body() = stringBuffer.GetString();
            response.prepare_payload();
            //write to socket
            responseSerializer.emplace(response);
            beast::http::async_write(socket,
                                     *responseSerializer,
                                     [this](boost::beast::error_code ec, std::size_t) {
//                                         socket.shutdown(tcp::socket::shutdown_send, ec);
                                         accept();
                                     });
        }
        break;
        default: {
            sendBadRespononse(beast::http::status::bad_request,
                              "Invalid request method '" + request.method_string().to_string() +"'\r\n");
        }
    }
}

void HttpHandler::sendBadRespononse(beast::http::status status,
                                    const std::string& error) {
    beast::http::response<beast::http::string_body> response;
    response.result(status);
    response.keep_alive(false);
    response.set(beast::http::field::server, "Beast");
    response.set(beast::http::field::content_type, "text/plain");
    response.body() = error;
    response.prepare_payload();

    responseSerializer.emplace(response);
    beast::http::async_write(socket,
                             *responseSerializer,
                             [this](boost::beast::error_code ec, std::size_t) {
        socket.shutdown(tcp::socket::shutdown_send, ec);
        accept();
    });
}

void HttpHandler::start() {
    accept();
}

HttpHandler::~HttpHandler() {
    if (socket.is_open()) {
        socket.close();
    }
}
