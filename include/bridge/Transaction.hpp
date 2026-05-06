#ifndef DWIN_EXMPL_TRANSACTION_HPP
#define DWIN_EXMPL_TRANSACTION_HPP

#include <string>
#include <cstdint>
#include <memory>
#include "Database.hpp"

namespace bridge {

// Структура для хранения данных транзакции (заказа)
struct TransactionData {
    int64_t id = 0;
    int shiftNumber = 0;
    bool isReversalTransaction = false;
    int openWayCardType = 0;
    std::string cardIdHash;
    std::string cardIdHashSalt;
    std::string terminalId;
    std::string mti;
    
    // Дата и время транзакции
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    
    int64_t amountInKops = 0;
    int goodsPumpNumber = 0;
    std::string goodsProductCode;
    std::string goodsProductNameUtf8;
    int64_t goodsQuantityInMilliliters = 0;
    int64_t goodsPriceInKopsByLiter = 0;
    std::string rrn;
    std::string authCode;
    std::string responseCode;
    
    // Сырые данные транзакции
    std::string secureData;
    std::string secureDataInitializationVector;
    std::string secureDataHash;
    std::string secureDataHashSalt;
    
    // Метка времени создания записи
    std::string createdAt;
};

class TransactionRepository {
public:
    explicit TransactionRepository(std::shared_ptr<Database> db);

    void createTable();

    int64_t insert(const TransactionData& transaction);

    TransactionData getById(int64_t id);

    std::vector<TransactionData> getAll();

    std::vector<TransactionData> getByShiftNumber(int shiftNumber);
    
    // Получить транзакции по RRN
    std::vector<TransactionData> getByRRN(const std::string& rrn);

    void update(const TransactionData& transaction);

    void deleteById(int64_t id);

    int64_t count();

private:
    std::shared_ptr<Database> db_;
    
    TransactionData mapFromStatement(sqlite3_stmt* stmt);
};

} // namespace bridge

#endif // DWIN_EXMPL_TRANSACTION_HPP
