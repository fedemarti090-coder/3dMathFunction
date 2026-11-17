#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <utility>

#include "payloadStruct.hpp"
#include "soketManager.hpp"



class SettingBridge
{
private:
    SoketManager soketManager;

    std::vector<std::string> split(const std::string &s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::string serializeData(const DataGive &payload)
    {
        std::ostringstream oss;
        oss << payload.rotateX << ";" << payload.rotateY;
        return oss.str();
    }

    DataGet deserializeData(const std::string &rawMessage)
    {
        DataGet data{};
        auto mainTokens = split(rawMessage, ';');

        if (mainTokens.size() != 11)
            throw std::invalid_argument("Formato DataGet non valido");

        data.function = mainTokens[0];
        data.dominioMax = std::stof(mainTokens[1]);
        data.dominioMin = std::stof(mainTokens[2]);
        data.step = std::stof(mainTokens[3]);
        data.rotateX = std::stof(mainTokens[4]);
        data.rotateY = std::stof(mainTokens[5]);
        data.rotateZ = std::stof(mainTokens[6]);

        auto rgb1 = split(mainTokens[7], ',');
        if (rgb1.size() != 3)
            throw std::invalid_argument("Colore iniziale non valido");
        data.col1 = {std::stof(rgb1[0]), std::stof(rgb1[1]), std::stof(rgb1[2])};

        auto rgb2 = split(mainTokens[8], ',');
        if (rgb2.size() != 3)
            throw std::invalid_argument("Colore finale non valido");
        data.col2 = {std::stof(rgb2[0]), std::stof(rgb2[1]), std::stof(rgb2[2])};

        data.alpha = std::stof(mainTokens[9]);

        auto booleans = split(mainTokens[10], ',');
        if (booleans.size() != 4)
            throw std::invalid_argument("Opzioni degli assi non valide");
        data.asseX = (std::stoi(booleans[0]) != 0);
        data.asseY = (std::stoi(booleans[1]) != 0);
        data.asseZ = (std::stoi(booleans[2]) != 0);
        data.Superfici0 = (std::stoi(booleans[3]) != 0);

        return data;
    }

public:

    void setOnDataReceivedCallback(std::function<void(const DataGet &)> callback)
    {
        soketManager.setOnMessageListener([this, callback](const std::string &rawMsg) {
            try {
                DataGet parsedData = deserializeData(rawMsg);
                callback(parsedData);
            } catch (const std::exception &e) {
                std::cerr << "[SettingBridge] Messaggio DataGet non valido: " << e.what() << '\n';
            }
        });
    }

    void setOnClientConnectedCallback(std::function<void()> callback)
    {
        soketManager.setOnClientConnectedListener(std::move(callback));
    }

    bool sendData(const DataGive &payload)
    {
        std::string rawData = serializeData(payload);
        return soketManager.sendRawData(rawData);
    }

    void start(int port = 8080)
    {
        soketManager.start(port);
    }

    void stop()
    {
        soketManager.stop();
    }
};