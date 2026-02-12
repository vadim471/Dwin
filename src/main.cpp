#include "bridge/SerialTransport.hpp"
#include "bridge/MessageLayer.hpp"
#include "bridge/DwinParser.hpp"
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <iostream>
#include "bridge/DwinLogic.hpp"
#include "bridge/HttpLogic.hpp"
#include "bridge/HttpParser.hpp"
#include "bridge/HttpTransport.hpp"
#include "bridge/constant.hpp"

using namespace bridge;

std::vector<uint8_t> makeIntPayload(uint16_t pageId) {
    return {
        static_cast<uint8_t>((pageId >> 8) & 0xFF),
        static_cast<uint8_t>(pageId & 0xFF)
    };
}


int main() {
    try {
        boost::asio::io_service ios;
        auto work = std::make_unique<boost::asio::io_service::work>(ios);
        MessageLayer core;

        Settings cfg = Settings::load(CONFIG);

        auto dwin_trans = std::make_shared<SerialTransport>(ios, 115200, "/dev/ttyUSB0");
        auto dwin_pars  = std::make_shared<DwinParser>();

        auto http_trans = std::make_shared<HttpTransport>("10.9.7.228", 12080, "user", "cloud");
        auto http_pars  = std::make_shared<HttpParser>(cfg);

        core.registerChannel(UART_LAYER, dwin_trans, dwin_pars);
        core.registerChannel(HTTP_LAYER, http_trans, http_pars);

        auto dwin_logic = std::make_shared<DwinLogic>(cfg);
        core.registerLogic(UART_LAYER, dwin_logic);

        auto http_logic = std::make_shared<HttpLogic>(cfg, ios);
        core.registerLogic(HTTP_LAYER, http_logic);

        std::thread ioThread([&ios](){
            std::cerr << "[IO Thread] Starting ios.run()" << std::endl;
            ios.run();
            std::cerr << "[IO Thread] ios.run() finished!" << std::endl;
        });

        std::thread coreThread([&core](){ core.run(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        bool keepRunning = true;
        Message pageMsg;
        http_logic->startLoop(core);

        while (keepRunning) {
            char choice;
            std::cout << "\n================ MENU ================" << std::endl;
            std::cout << "1 - Change page (VP 0x0084)" << std::endl;
            std::cout << "2 - Get parameters TRK" << std::endl;
            std::cout << "q - Quit" << std::endl;
            std::cout << "Selection: ";
            std::cin >> choice;

            switch (choice) {
                case '1': {
                    int page_id = 0;
                    std::cout << "Type number of page" << std::endl;
                    std::cin >> page_id;

                    pageMsg.type = "change_page";
                    pageMsg.payload = makeIntPayload(page_id);
                    core.sendTo(UART_LAYER, pageMsg);
                    break;
                }
                case '2': {
                    break;
                }
                case '3': {
                    break;
                }
                case 'q': {
                    keepRunning = false;
                    break;
                }
                default:
                    break;
            }
        }
        std::cout << "[MAIN] Stopping systems..." << std::endl;

        core.stop();
        work.reset();

        if (coreThread.joinable()) coreThread.join();
        if (ioThread.joinable()) ioThread.join();

        std::cout << "[MAIN] Bye!" << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
    }
    return 0;
}