#ifndef DWIN_EXMPL_TRANSACTION_HPP
#define DWIN_EXMPL_TRANSACTION_HPP

#include <string>
#include <cstdint>
#include <memory>
#include <vector>

#include "Database.hpp"

namespace bridge {

// Структура для хранения данных транзакции (заказа)
struct TransactionData {
    int64_t id = 0;
    std::string date;
    std::string processing;
    std::string fact_amount;
    std::string fact_price;
    std::string fact_quantity;
    std::string rqs_amount;
    std::string rqs_price;
    std::string rqs_quantity;
    std::string transaction_id;
    std::string card_number;
    std::string fuel_name;
    std::string trk_name;
    std::string type_sale;
    std::string id_tso;

    std::string after_density;
    std::string after_filling;
    std::string after_lower_level;
    std::string after_lower_volume;
    std::string after_temperature;
    std::string after_total_volume;
    std::string after_upper_level;
    std::string after_upper_volume;
    std::string after_weight;

    std::string before_density;
    std::string before_filling;
    std::string before_lower_level;
    std::string before_lower_volume;
    std::string before_temperature;
    std::string before_total_volume;
    std::string before_upper_level;
    std::string before_upper_volume;
    std::string before_weight;

    std::string discount;
    std::string dispenser_volume_begin;
    std::string dispenser_volume_end;
    std::string dsc;
    std::string receipt;
    
    // Флаг отправки на сервер
    bool sent_to_server = false;
    
    // Метка времени создания записи
    std::string created_at;
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

    std::vector<TransactionData> getUnsent();

    void markAsSent(int64_t id);

    void update(const TransactionData& transaction);

    void deleteById(int64_t id);

    int64_t count();

private:
    std::shared_ptr<Database> db_;
    
    TransactionData mapFromStatement(sqlite3_stmt* stmt);
};

} // namespace bridge

#endif // DWIN_EXMPL_TRANSACTION_HPP
