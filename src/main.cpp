#include "bridge/transport/SerialTransport.hpp"
#include "bridge/core/MessageLayer.hpp"
#include "bridge/parser/DwinParser.hpp"
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <boost/make_unique.hpp>
#include <iostream>

#include "bridge/logic/DwinLogic.hpp"
#include "bridge/logic/PrimeLogic.hpp"
#include "bridge/parser/PrimeParser.hpp"
#include "bridge/transport/HttpTransport.hpp"
#include "bridge/transport/ArkaimTransport.hpp"
#include "bridge/parser/ArkaimParser.hpp"
#include "bridge/logic/ArkaimLogic.hpp"
#include "bridge/core/constant.hpp"
#include "bridge/core/Logger.hpp"
#include "bridge/database/Database.hpp"
#include "bridge/database/Transaction.hpp"
#include "bridge/database/MetrologicalRecord.hpp"
#include "bridge/database/BosRepository.hpp"
#include <Poco/Net/SSLManager.h>

#include <itp/named_pipe.hpp>
#include <itp/logger.hpp>

#include "bridge/core/Settings.hpp"
#include "bridge/database/LevelGaugeRepository.hpp"
#include "bridge/httpAuth/BasicAuthenticator.hpp"
#include "bridge/httpAuth/DigestAuthenticator.hpp"
#include "bridge/logic/BosLogic.hpp"
#include "bridge/parser/BosParser.hpp"

using namespace bridge;

std::vector<uint8_t> makeIntPayload(uint16_t pageId) {
    return {
        static_cast<uint8_t>((pageId >> 8) & 0xFF),
        static_cast<uint8_t>(pageId & 0xFF)
    };
}

void sendTextToVp(MessageLayer& core, uint16_t vp, const std::string& text) {
    Message msg;
    msg.type = DWIN_MESSAGE_TYPE_WRITE_VP;

    msg.payload.push_back((vp >> 8) & 0xFF);
    msg.payload.push_back(vp & 0xFF);

    for (char c : text) {
        msg.payload.push_back(static_cast<uint8_t>(c));
    }

    if (msg.payload.size() % 2 != 0) {
        msg.payload.push_back(0x00);
    }

    core.sendTo(UART_LAYER, msg);
}


int main() {
    try {
        initLogger("logs");
        
        LOG_SYSTEM_INFO << "Application started";
        Poco::Net::initializeSSL();

        // INIT DB
        auto db = std::make_shared<Database>("Standalone.db");
        auto transactionRepo = std::make_shared<TransactionRepository>(db);
        //auto metrologicalRepo = std::make_shared<LevelGaugeRepository>(db);
        auto bosRepo = std::make_shared<BosRepository>(db);

        transactionRepo->createTable();
//        metrologicalRepo->createTable();
        bosRepo->createTable();
        
        LOG_SYSTEM_INFO << "Database initialized successfully";
        
        boost::asio::io_service ios;
        auto work = std::make_unique<boost::asio::io_service::work>(ios);
        MessageLayer core;

        Settings cfg = Settings::load(CONFIG);
        LOG_SYSTEM_INFO << "Configuration loaded successfully";

        auto dwin_trans = std::make_shared<SerialTransport>(ios, cfg);
        auto dwin_pars  = std::make_shared<DwinParser>();

        auto digest_authenticator = std::make_shared<DigestAuthenticator>(cfg);
        auto basic_authenticator = std::make_shared<BasicAuthenticator>(cfg);

        auto http_trans_prime = std::make_shared<HttpTransport>(PRIME_HTTP_LAYER, cfg.server_prime.ip, cfg.server_prime.port, false, digest_authenticator);
        auto http_trans_bos = std::make_shared<HttpTransport>(BOS_HTTP_LAYER, cfg.server_bos.ip, cfg.server_bos.port, true, basic_authenticator);

        auto http_pars_prime = std::make_shared<PrimeParser>();
        auto http_pars_bos = std::make_shared<BosParser>();


        auto arkaim_trans = std::make_shared<ArkaimTransport>(itp::named_pipe::uptr());
        auto arkaim_pars  = std::make_shared<ArkaimParser>();

        core.registerChannel(UART_LAYER, dwin_trans, dwin_pars);
        core.registerChannel(PRIME_HTTP_LAYER, http_trans_prime, http_pars_prime);
        core.registerChannel(BOS_HTTP_LAYER, http_trans_bos, http_pars_bos);
        core.registerChannel(PIPE_LAYER, arkaim_trans, arkaim_pars);

        auto dwin_logic = std::make_shared<DwinLogic>(cfg);
        core.registerLogic(UART_LAYER, dwin_logic);

        auto http_logic = std::make_shared<PrimeLogic>(cfg, ios);
        core.registerLogic(PRIME_HTTP_LAYER, http_logic);

        auto bos_logic = std::make_shared<BosLogic>(cfg);
        core.registerLogic(BOS_HTTP_LAYER, bos_logic);

        auto arkaim_logic = std::make_shared<ArkaimLogic>(ios, arkaim_trans);
        core.registerLogic(PIPE_LAYER, arkaim_logic);

        // ITP Logger для удаленного логирования (кроме ArkaimLogic)
        auto itp_logger_instance = std::make_shared<itp::logger>();

        std::thread ioThread([&ios](){
            LOG_SYSTEM_INFO << "IO Thread starting";
            ios.run();
            LOG_SYSTEM_INFO << "IO Thread finished";
        });

        std::thread coreThread([&core](){ 
            LOG_SYSTEM_INFO << "Core Thread starting";
            core.run(); 
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        bool keepRunning = true;
        Message pageMsg;

        LOG_SYSTEM_INFO << "Starting Arkaim payment processing";
        arkaim_logic->startLoop(core);

        itp_logger_instance->start(arkaim_logic->getItpRoot(), true);
        http_logic->setItpLogger(itp_logger_instance);
        // Устанавливаем BosRepository в HttpLogic
        //http_logic->setBosRepository(bosRepo);
        dwin_logic->setItpLogger(itp_logger_instance);
        LOG_SYSTEM_INFO << "ITP Logger initialized for HttpLogic and DwinLogic";
        
        LOG_SYSTEM_INFO << "Starting HTTP logic loop";
        http_logic->startLoop(core);


        while (keepRunning) {
            char choice;
            std::cout << "\n================ MENU ================" << std::endl;
            std::cout << "1 - Change page (VP 0x0084)" << std::endl;
            std::cout << "2 - Write by VP" << std::endl;
            std::cout << "3 - Test Database (Insert Transaction)" << std::endl;
            std::cout << "4 - Test Database (Insert Metrological Record)" << std::endl;
            std::cout << "5 - Show all Transactions" << std::endl;
            std::cout << "6 - Show all Metrological Records" << std::endl;
            std::cout << "q - Quit" << std::endl;
            std::cout << "Selection: ";
            std::cin >> choice;

            switch (choice) {
                case '1': {
                    int page_id = 0;
                    std::cout << "Type number of page" << std::endl;
                    std::cin >> page_id;

                    LOG_SYSTEM_INFO << "Changing page to: " << page_id;
                    pageMsg.type = DWIN_MESSAGE_TYPE_CHANGE_PAGE;
                    pageMsg.payload = makeIntPayload(page_id);
                    core.sendTo(UART_LAYER, pageMsg);
                    break;
                }
                case '2': {
                    uint16_t vp = 0;
                    std::string data = "";

                    std::cout << "Type vp (decimal): " << std::endl;
                    std::cin >> std::hex >> vp;

                    std::cout << "Type data text: " << std::endl;
                    std::cin >> data;

                    LOG_SYSTEM_INFO << "Writing to VP 0x" << std::hex << vp << ": " << data;
                    sendTextToVp(core, vp, data);

                    std::cout << "[INFO] Sent text '" << data << "' to VP " << vp << std::endl;
                    break;
                }
                case '3': {
                    // Тестовая вставка транзакции
                    TransactionData testTransaction;
                    // testTransaction.shiftNumber = 5;
                    // testTransaction.isReversalTransaction = false;
                    // testTransaction.openWayCardType = 1;
                    // testTransaction.cardIdHash = "9E2B0DF2E88A64DF2624FB27CCB2F9F8F846173D";
                    // testTransaction.cardIdHashSalt = "8F127B07BAA49C215A17EBA9C5C22EB01CA67B05";
                    // testTransaction.terminalId = "1KA00101";
                    // testTransaction.mti = "0200";
                    // testTransaction.year = 2022;
                    // testTransaction.month = 2;
                    // testTransaction.day = 22;
                    // testTransaction.hour = 15;
                    // testTransaction.minute = 20;
                    // testTransaction.second = 56;
                    // testTransaction.amountInKops = 500;
                    // testTransaction.goodsPumpNumber = 1;
                    // testTransaction.goodsProductCode = "0001000095";
                    // testTransaction.goodsProductNameUtf8 = "АИ-95";
                    // testTransaction.goodsQuantityInMilliliters = 5000;
                    // testTransaction.goodsPriceInKopsByLiter = 100;
                    // testTransaction.rrn = "205377107042";
                    // testTransaction.authCode = "363249";
                    // testTransaction.responseCode = "00";
                    //
                    // int64_t id = transactionRepo->insert(testTransaction);
                    // std::cout << "[INFO] Transaction inserted with ID: " << id << std::endl;
                    break;
                }
                case '4': {
                    // Тестовая вставка метрологической записи
                    MetrologicalRecordData testRecord;
                    testRecord.date = "2022-04-29T17:40:16.070";
                    testRecord.density = "0";
                    testRecord.filling = "0";
                    testRecord.fuelName = "ДТ";
                    testRecord.idTso = "757586";
                    testRecord.lowerLevel = "0";
                    testRecord.lowerVolume = "0";
                    testRecord.namePmp = "101";
                    testRecord.temperature = "0";
                    testRecord.totalVolume = "10";
                    testRecord.upperLevel = "0";
                    testRecord.upperVolume = "10";
                    testRecord.weight = "0";
                    
                    //int64_t id = metrologicalRepo->insert(testRecord);
                    std::cout << "[INFO] Metrological record inserted with ID: " << std::endl;
                    break;
                }
                case '5': {
                    // Показать все транзакции
                    // auto transactions = transactionRepo->getAll();
                    // std::cout << "\n=== Transactions (Total: " << transactions.size() << ") ===" << std::endl;
                    // for (const auto& t : transactions) {
                    //     std::cout << "ID: " << t.id
                    //               << " | Shift: " << t.shiftNumber
                    //               << " | RRN: " << t.rrn
                    //               << " | Product: " << t.goodsProductNameUtf8
                    //               << " | Amount: " << t.amountInKops << " kops"
                    //               << std::endl;
                    // }
                    break;
                }
                case 'q': {
                    LOG_SYSTEM_INFO << "User requested shutdown";
                    keepRunning = false;
                    break;
                }
                default:
                    break;
            }
        }
        LOG_SYSTEM_INFO << "Stopping systems...";

        core.stop();
        work.reset();

        if (coreThread.joinable()) coreThread.join();
        if (ioThread.joinable()) ioThread.join();

        LOG_SYSTEM_INFO << "=== Application stopped ===";

    } catch (std::exception& e) {
        LOG_SYSTEM_FATAL << "Fatal Error: " << e.what();
        std::cerr << "Fatal Error: " << e.what() << std::endl;
    }
    return 0;
}
