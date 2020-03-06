//
// Created by se7enrains on 3/3/20.
//

#include "client.h"

void Client::getFilePath() {
    std::cout << "Enter path to file for sending: ";
    std::cin >> filePath;
    std::cout << "Enter path for saving on server: ";
    std::cin >> path;
}

Client::Client(const std::string& ip, int port) :
    ip(ip),
    port(port) { }

void Client::start() {
    connect();
    prepareMessage();

}

void Client::connect() {
    socket = new boost::asio::ip::tcp::socket(ioContext);
    socket->connect(boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address::from_string(ip),
            port));

}

void Client::prepareMessage() {
    std::ifstream openFileStream(sFilePath);
    std::string line;
    std::string sFileData;
    if (openFileStream.is_open()) {
        while (std::getline(openFileStream, line)) {
            sFileData.append(line).append("\n");
        }
        openFileStream.close();
    }

    dom.SetObject();
    rapidjson::Value filePath, fileData;
    filePath.SetString(rapidjson::StringRef(sFilePath.c_str()));
    fileData.SetString(rapidjson::StringRef(sFileData.c_str()));
    dom.AddMember("file-path", filePath, dom.GetAllocator());
    dom.AddMember("file-data", fileData, dom.GetAllocator());
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    dom.Accept(writer);

    beast::http::request<beast::http::string_body> request;
    request.method(beast::http::verb::post);
    request.keep_alive(true);
    request.set(beast::http::field::host, ip);
    request.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request.set(beast::http::field::content_type, "application/json");
    request.body() = stringBuffer.GetString();
    request.prepare_payload();

    boost::beast::error_code ec;
    beast::http::request_serializer<boost::beast::http::string_body> requestSerializer(request);
    beast::http::write(*socket, requestSerializer, ec);
    if (ec) {
        std::cerr << ec << std::endl;
    }
}

const beast::http::response<Client::responseBodyType>& Client::getResponse() {
    beast::http::read(*socket,
                      buffer,
                      parser);
    return parser.get();
}

void Client::saveFile(const beast::http::response<Client::responseBodyType> & response) {
    rapidjson::Document doc;
    doc.Parse((char*) response.body().data().data());
}