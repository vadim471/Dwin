#include "bridge/database/Transaction.hpp"
#include "bridge/core/Logger.hpp"
#include <sstream>
#include <vector>

namespace bridge {

TransactionRepository::TransactionRepository(std::shared_ptr<Database> db)
    : db_(db) {
}

void TransactionRepository::createTable() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            shift_number INTEGER NOT NULL,
            is_reversal_transaction INTEGER NOT NULL DEFAULT 0,
            open_way_card_type INTEGER,
            card_id_hash TEXT,
            card_id_hash_salt TEXT,
            terminal_id TEXT,
            mti TEXT,
            
            transaction_year INTEGER,
            transaction_month INTEGER,
            transaction_day INTEGER,
            transaction_hour INTEGER,
            transaction_minute INTEGER,
            transaction_second INTEGER,
            
            amount_in_kops INTEGER,
            goods_pump_number INTEGER,
            goods_product_code TEXT,
            goods_product_name_utf8 TEXT,
            goods_quantity_in_milliliters INTEGER,
            goods_price_in_kops_by_liter INTEGER,
            rrn TEXT,
            auth_code TEXT,
            response_code TEXT,
            
            secure_data TEXT,
            secure_data_initialization_vector TEXT,
            secure_data_hash TEXT,
            secure_data_hash_salt TEXT,
            
            sent_to_server INTEGER DEFAULT 0,
            
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";
    
    db_->execute(sql);
    
    // Создать индексы для быстрого поиска
    db_->execute("CREATE INDEX IF NOT EXISTS idx_shift_number ON transactions(shift_number);");
    db_->execute("CREATE INDEX IF NOT EXISTS idx_rrn ON transactions(rrn);");
    db_->execute("CREATE INDEX IF NOT EXISTS idx_created_at ON transactions(created_at);");
    db_->execute("CREATE INDEX IF NOT EXISTS idx_sent_to_server ON transactions(sent_to_server);");
    
    LOG_SYSTEM_INFO << "Transactions table created/verified";
}

int64_t TransactionRepository::insert(const TransactionData& transaction) {
    const char* sql = R"(
        INSERT INTO transactions (
            shift_number, is_reversal_transaction, open_way_card_type,
            card_id_hash, card_id_hash_salt, terminal_id, mti,
            transaction_year, transaction_month, transaction_day,
            transaction_hour, transaction_minute, transaction_second,
            amount_in_kops, goods_pump_number, goods_product_code,
            goods_product_name_utf8, goods_quantity_in_milliliters,
            goods_price_in_kops_by_liter, rrn, auth_code, response_code,
            secure_data, secure_data_initialization_vector,
            secure_data_hash, secure_data_hash_salt, sent_to_server
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    )";
    
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int(stmt, 1, transaction.shiftNumber);
    sqlite3_bind_int(stmt, 2, transaction.isReversalTransaction ? 1 : 0);
    sqlite3_bind_int(stmt, 3, transaction.openWayCardType);
    sqlite3_bind_text(stmt, 4, transaction.cardIdHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, transaction.cardIdHashSalt.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, transaction.terminalId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, transaction.mti.c_str(), -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_int(stmt, 8, transaction.year);
    sqlite3_bind_int(stmt, 9, transaction.month);
    sqlite3_bind_int(stmt, 10, transaction.day);
    sqlite3_bind_int(stmt, 11, transaction.hour);
    sqlite3_bind_int(stmt, 12, transaction.minute);
    sqlite3_bind_int(stmt, 13, transaction.second);
    
    sqlite3_bind_int64(stmt, 14, transaction.amountInKops);
    sqlite3_bind_int(stmt, 15, transaction.goodsPumpNumber);
    sqlite3_bind_text(stmt, 16, transaction.goodsProductCode.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 17, transaction.goodsProductNameUtf8.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 18, transaction.goodsQuantityInMilliliters);
    sqlite3_bind_int64(stmt, 19, transaction.goodsPriceInKopsByLiter);
    sqlite3_bind_text(stmt, 20, transaction.rrn.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 21, transaction.authCode.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 22, transaction.responseCode.c_str(), -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_text(stmt, 23, transaction.secureData.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 24, transaction.secureDataInitializationVector.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 25, transaction.secureDataHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 26, transaction.secureDataHashSalt.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 27, transaction.sent_to_server ? 1 : 0);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        LOG_SYSTEM_ERROR << "Failed to insert transaction";
        throw DatabaseException("Failed to insert transaction");
    }
    
    int64_t id = db_->getLastInsertId();
    LOG_SYSTEM_INFO << "Transaction inserted with ID: " << id;
    
    return id;
}

TransactionData TransactionRepository::mapFromStatement(sqlite3_stmt* stmt) {
    TransactionData data;
    
    data.id = sqlite3_column_int64(stmt, 0);
    data.shiftNumber = sqlite3_column_int(stmt, 1);
    data.isReversalTransaction = sqlite3_column_int(stmt, 2) != 0;
    data.openWayCardType = sqlite3_column_int(stmt, 3);
    
    const unsigned char* text;
    
    text = sqlite3_column_text(stmt, 4);
    if (text) data.cardIdHash = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 5);
    if (text) data.cardIdHashSalt = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 6);
    if (text) data.terminalId = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 7);
    if (text) data.mti = reinterpret_cast<const char*>(text);
    
    data.year = sqlite3_column_int(stmt, 8);
    data.month = sqlite3_column_int(stmt, 9);
    data.day = sqlite3_column_int(stmt, 10);
    data.hour = sqlite3_column_int(stmt, 11);
    data.minute = sqlite3_column_int(stmt, 12);
    data.second = sqlite3_column_int(stmt, 13);
    
    data.amountInKops = sqlite3_column_int64(stmt, 14);
    data.goodsPumpNumber = sqlite3_column_int(stmt, 15);
    
    text = sqlite3_column_text(stmt, 16);
    if (text) data.goodsProductCode = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 17);
    if (text) data.goodsProductNameUtf8 = reinterpret_cast<const char*>(text);
    
    data.goodsQuantityInMilliliters = sqlite3_column_int64(stmt, 18);
    data.goodsPriceInKopsByLiter = sqlite3_column_int64(stmt, 19);
    
    text = sqlite3_column_text(stmt, 20);
    if (text) data.rrn = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 21);
    if (text) data.authCode = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 22);
    if (text) data.responseCode = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 23);
    if (text) data.secureData = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 24);
    if (text) data.secureDataInitializationVector = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 25);
    if (text) data.secureDataHash = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 26);
    if (text) data.secureDataHashSalt = reinterpret_cast<const char*>(text);
    
    data.sent_to_server = sqlite3_column_int(stmt, 27) != 0;
    
    text = sqlite3_column_text(stmt, 28);
    if (text) data.createdAt = reinterpret_cast<const char*>(text);
    
    return data;
}

TransactionData TransactionRepository::getById(int64_t id) {
    const char* sql = "SELECT * FROM transactions WHERE id = ?;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        db_->finalize(stmt);
        throw DatabaseException("Transaction not found");
    }
    
    TransactionData data = mapFromStatement(stmt);
    db_->finalize(stmt);
    
    return data;
}

std::vector<TransactionData> TransactionRepository::getAll() {
    const char* sql = "SELECT * FROM transactions ORDER BY created_at DESC;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    std::vector<TransactionData> results;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return results;
}

std::vector<TransactionData> TransactionRepository::getByShiftNumber(int shiftNumber) {
    const char* sql = "SELECT * FROM transactions WHERE shift_number = ? ORDER BY created_at DESC;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int(stmt, 1, shiftNumber);
    
    std::vector<TransactionData> results;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return results;
}

std::vector<TransactionData> TransactionRepository::getByRRN(const std::string& rrn) {
    const char* sql = "SELECT * FROM transactions WHERE rrn = ? ORDER BY created_at DESC;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, rrn.c_str(), -1, SQLITE_TRANSIENT);
    
    std::vector<TransactionData> results;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return results;
}

void TransactionRepository::update(const TransactionData& transaction) {
    const char* sql = R"(
        UPDATE transactions SET
            shift_number = ?, is_reversal_transaction = ?, open_way_card_type = ?,
            card_id_hash = ?, card_id_hash_salt = ?, terminal_id = ?, mti = ?,
            transaction_year = ?, transaction_month = ?, transaction_day = ?,
            transaction_hour = ?, transaction_minute = ?, transaction_second = ?,
            amount_in_kops = ?, goods_pump_number = ?, goods_product_code = ?,
            goods_product_name_utf8 = ?, goods_quantity_in_milliliters = ?,
            goods_price_in_kops_by_liter = ?, rrn = ?, auth_code = ?, response_code = ?,
            secure_data = ?, secure_data_initialization_vector = ?,
            secure_data_hash = ?, secure_data_hash_salt = ?
        WHERE id = ?;
    )";
    
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int(stmt, 1, transaction.shiftNumber);
    sqlite3_bind_int(stmt, 2, transaction.isReversalTransaction ? 1 : 0);
    sqlite3_bind_int(stmt, 3, transaction.openWayCardType);
    sqlite3_bind_text(stmt, 4, transaction.cardIdHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, transaction.cardIdHashSalt.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, transaction.terminalId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, transaction.mti.c_str(), -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_int(stmt, 8, transaction.year);
    sqlite3_bind_int(stmt, 9, transaction.month);
    sqlite3_bind_int(stmt, 10, transaction.day);
    sqlite3_bind_int(stmt, 11, transaction.hour);
    sqlite3_bind_int(stmt, 12, transaction.minute);
    sqlite3_bind_int(stmt, 13, transaction.second);
    
    sqlite3_bind_int64(stmt, 14, transaction.amountInKops);
    sqlite3_bind_int(stmt, 15, transaction.goodsPumpNumber);
    sqlite3_bind_text(stmt, 16, transaction.goodsProductCode.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 17, transaction.goodsProductNameUtf8.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 18, transaction.goodsQuantityInMilliliters);
    sqlite3_bind_int64(stmt, 19, transaction.goodsPriceInKopsByLiter);
    sqlite3_bind_text(stmt, 20, transaction.rrn.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 21, transaction.authCode.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 22, transaction.responseCode.c_str(), -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_text(stmt, 23, transaction.secureData.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 24, transaction.secureDataInitializationVector.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 25, transaction.secureDataHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 26, transaction.secureDataHashSalt.c_str(), -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_int64(stmt, 27, transaction.id);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        LOG_SYSTEM_ERROR << "Failed to update transaction";
        throw DatabaseException("Failed to update transaction");
    }
    
    LOG_SYSTEM_INFO << "Transaction updated: " << transaction.id;
}

void TransactionRepository::deleteById(int64_t id) {
    const char* sql = "DELETE FROM transactions WHERE id = ?;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        LOG_SYSTEM_ERROR << "Failed to delete transaction";
        throw DatabaseException("Failed to delete transaction");
    }
    
    LOG_SYSTEM_INFO << "Transaction deleted: " << id;
}

int64_t TransactionRepository::count() {
    const char* sql = "SELECT COUNT(*) FROM transactions;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    int64_t count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int64(stmt, 0);
    }
    
    db_->finalize(stmt);
    return count;
}

std::vector<TransactionData> TransactionRepository::getUnsent() {
    const char* sql = "SELECT * FROM transactions WHERE sent_to_server = 0 ORDER BY created_at ASC;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    std::vector<TransactionData> transactions;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        transactions.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return transactions;
}

void TransactionRepository::markAsSent(int64_t id) {
    const char* sql = "UPDATE transactions SET sent_to_server = 1 WHERE id = ?;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        LOG_SYSTEM_ERROR << "Failed to mark transaction as sent";
        throw DatabaseException("Failed to mark transaction as sent");
    }
    
    LOG_SYSTEM_INFO << "Transaction marked as sent: " << id;
}

} // namespace bridge
