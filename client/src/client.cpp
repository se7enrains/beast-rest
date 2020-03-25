//
// Created by se7enrains on 3/3/20.
//

#include "client.h"

void Client::getFilePath() {
    std::cout << "Enter path to file for sending: ";
    std::cin >> sFilePath;
    std::cout << "Enter path for saving on server: ";
    std::cin >> path;
}

Client::Client(const std::string& ip, int port) :
    ip(ip),
    port(port) { }

void Client::run() {
    connect();
    getFilePath();
    prepareMessage();
    sendMessage();
    getResponse();
    saveFile();
    closeConnection();
}

void Client::connect() {
    socket = new boost::asio::ip::tcp::socket(ioContext);
    socket->connect(boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address::from_string(ip),
            port));

}

void Client::prepareMessage() {
    std::ifstream openFileStream(sFilePath, std::ios::binary);
    std::string sFileData;
    if (openFileStream.is_open()) {
        std::istreambuf_iterator<char> eos;
        std::istreambuf_iterator<char> iit (openFileStream);
        while (iit!=eos) sFileData+=*iit++;
        openFileStream.close();
    }

    dom.SetObject();
    rapidjson::Value filePath, fileData;
    filePath.SetString(rapidjson::StringRef(path.c_str()));
    fileData.SetString(rapidjson::StringRef(sFileData.c_str(), sFileData.size()));
    dom.AddMember("file-path", filePath, dom.GetAllocator());
    dom.AddMember("file-data", fileData, dom.GetAllocator());
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    dom.Accept(writer);

    request.method(beast::http::verb::post);
    request.keep_alive(true);
    request.set(beast::http::field::host, ip);
    request.target("1.1");
    request.version(11);
    request.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request.set(beast::http::field::content_type, "application/json");
    request.body() = stringBuffer.GetString();
    request.prepare_payload();
}

void Client::sendMessage() {
    boost::beast::error_code ec;
    beast::http::request_serializer<boost::beast::http::string_body> requestSerializer(request);
    beast::http::write(*socket, requestSerializer, ec);
    if (ec) {
        std::cerr << ec << std::endl;
        socket->close();
    }
}

void Client::getResponse() {
    beast::http::read(*socket,
                      buffer,
                      parser);
    response = parser.get();
    parser.release();
}

void Client::saveFile() {
    rapidjson::Document doc;
    std::string body = (char*) response.body().data().data();
    body[body.find_last_of('}') + 1] = '\0';
    doc.Parse(body.c_str());
    std::string fileName = std::string("files/").append(doc["file-name"].GetString());
    std::vector<std::string> pathParts;
    boost::split(pathParts, fileName, boost::is_any_of("/"));
    if (pathParts.size() > 1) {
        std::string dirConstructorPath;
        for (int i = 0; i < pathParts.size() - 1; i++) {
            dirConstructorPath.append(pathParts[i]).append("/");
            boost::filesystem::create_directory(dirConstructorPath);
        }
    }
    //creating file at specified directory and writing data to it
    std::ofstream saveFileStream(fileName, std::ios::binary);
    std::string data(doc["file-data"].GetString(), doc["file-data"].GetStringLength());
    saveFileStream << data << std::endl;
    saveFileStream.close();
}

void Client::closeConnection() {
    socket->close();
    request.clear();
    response.clear();
}

