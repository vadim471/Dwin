//
// Created by vadim.tissen on 08.05.2026.
//


#pragma once

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <string>
#include <ostream>

namespace bridge {
    class IHttpAuthenticator {
    public:
        virtual ~IHttpAuthenticator() = default;

        // Модификация запроса перед отправкой (добавление заголовков, кук, токенов)
        virtual void prepareRequest(Poco::Net::HTTPRequest& request) = 0;

        // Обработка успешного ответа (сохранение новых кук)
        virtual void processResponse(const Poco::Net::HTTPResponse& response) = 0;

        // Обработка ошибки авторизации (401).
        // Возвращает true, если удалось переавторизоваться и получить новый ответ.
        virtual bool handleUnauthorized(Poco::Net::HTTPClientSession& session,
                                        Poco::Net::HTTPRequest& request,
                                        Poco::Net::HTTPResponse& response,
                                        const std::string& body,
                                        std::ostream& responseStream) = 0;
        virtual void reset() = 0;
    };
}