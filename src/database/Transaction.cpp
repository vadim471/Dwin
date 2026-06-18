#include "bridge/database/Transaction.hpp"
#include <sqlite3.h>
#include <iostream>

namespace bridge {
    auto safeGetText = [](sqlite3_stmt *stmt, int col) -> std::string {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    TransactionRepository::TransactionRepository(std::shared_ptr<Database> db)
        : db_(std::move(db)) {
    }

    void TransactionRepository::createTable() {
        const char *sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            date TEXT, processing TEXT, fact_amount TEXT, fact_price TEXT, fact_quantity TEXT,
            rqs_amount TEXT, rqs_price TEXT, rqs_quantity TEXT, tracnsaction_id TEXT,
            card_number TEXT, fuel_name TEXT, trk_name TEXT, type_sale TEXT, id_tso TEXT,
            after_density TEXT, after_filling TEXT, after_lower_level TEXT, after_lower_volume TEXT,
            after_temperature TEXT, after_total_volume TEXT, after_upper_level TEXT, after_upper_volume TEXT, after_weight TEXT,
            before_density TEXT, before_filling TEXT, before_lower_level TEXT, before_lower_volume TEXT,
            before_temperature TEXT, before_total_volume TEXT, before_upper_level TEXT, before_upper_volume TEXT, before_weight TEXT,
            discount TEXT, dispenser_volume_begin TEXT, dispenser_volume_end TEXT, dsc TEXT, receipt TEXT,
            sent_to_server INTEGER, created_at TEXT,
            shift_number INTEGER DEFAULT 0, rrn TEXT DEFAULT ''
        );
    )";

        db_->execute(sql);
        db_->execute("CREATE INDEX IF NOT EXISTS idx_created_at ON transactions(created_at);");

        LOG_SYSTEM_INFO << "Transactions table created/verified";
    }

    int64_t TransactionRepository::insert(const TransactionData &t) {
        const char *sql = R"(
        INSERT INTO transactions (
            date, processing, fact_amount, fact_price, fact_quantity, rqs_amount, rqs_price, rqs_quantity,
            tracnsaction_id, card_number, fuel_name, trk_name, type_sale, id_tso,
            after_density, after_filling, after_lower_level, after_lower_volume, after_temperature, after_total_volume, after_upper_level, after_upper_volume, after_weight,
            before_density, before_filling, before_lower_level, before_lower_volume, before_temperature, before_total_volume, before_upper_level, before_upper_volume, before_weight,
            discount, dispenser_volume_begin, dispenser_volume_end, dsc, receipt, sent_to_server, created_at
        ) VALUES (
            ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?
        );
    )";

        sqlite3_stmt *stmt = db_->prepare(sql);

        int col = 1;
        sqlite3_bind_text(stmt, col++, t.date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.processing.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.fact_amount.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.fact_price.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.fact_quantity.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.rqs_amount.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.rqs_price.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.rqs_quantity.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.transaction_id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.card_number.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.fuel_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.trk_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.type_sale.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.id_tso.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, col++, t.after_density.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_filling.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_lower_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_lower_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_temperature.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_total_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_upper_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_upper_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_weight.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, col++, t.before_density.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_filling.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_lower_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_lower_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_temperature.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_total_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_upper_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_upper_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_weight.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, col++, t.discount.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.dispenser_volume_begin.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.dispenser_volume_end.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.dsc.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.receipt.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_int(stmt, col++, t.sent_to_server ? 1 : 0);
        sqlite3_bind_text(stmt, col++, t.created_at.c_str(), -1, SQLITE_TRANSIENT);

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

    TransactionData TransactionRepository::getById(int64_t id) {
        const char *sql = "SELECT * FROM transactions WHERE id = ?;";
        sqlite3_stmt *stmt = db_->prepare(sql);

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
        const char *sql = "SELECT * FROM transactions ORDER BY created_at DESC;";
        sqlite3_stmt *stmt = db_->prepare(sql);

        std::vector<TransactionData> results;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            results.push_back(mapFromStatement(stmt));
        }

        db_->finalize(stmt);
        return results;
    }

    std::vector<TransactionData> TransactionRepository::getUnsent() {
        std::vector<TransactionData> list;
        const char *sql = "SELECT * FROM transactions WHERE sent_to_server = 0 ORDER BY id ASC;";
        sqlite3_stmt *stmt = db_->prepare(sql);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            list.push_back(mapFromStatement(stmt));
        }

        db_->finalize(stmt);
        return list;
    }

    void TransactionRepository::markAsSent(int64_t id) {
        const char *sql = "UPDATE transactions SET sent_to_server = 1 WHERE id = ?;";
        sqlite3_stmt *stmt = db_->prepare(sql);

        sqlite3_bind_int64(stmt, 1, id);

        int rc = sqlite3_step(stmt);
        db_->finalize(stmt);

        if (rc != SQLITE_DONE) {
            LOG_SYSTEM_ERROR << "Failed to update transaction";
            throw DatabaseException("Failed to update transaction");
        }
        LOG_SYSTEM_INFO << "Marked as sent: " << id;
    }

    void TransactionRepository::update(const TransactionData &t) {
        const char *sql = R"(
        UPDATE transactions SET
            date=?, processing=?, fact_amount=?, fact_price=?, fact_quantity=?,
            rqs_amount=?, rqs_price=?, rqs_quantity=?, tracnsaction_id=?,
            card_number=?, fuel_name=?, trk_name=?, type_sale=?, id_tso=?,
            after_density=?, after_filling=?, after_lower_level=?, after_lower_volume=?, after_temperature=?, after_total_volume=?, after_upper_level=?, after_upper_volume=?, after_weight=?,
            before_density=?, before_filling=?, before_lower_level=?, before_lower_volume=?, before_temperature=?, before_total_volume=?, before_upper_level=?, before_upper_volume=?, before_weight=?,
            discount=?, dispenser_volume_begin=?, dispenser_volume_end=?, dsc=?, receipt=?, sent_to_server=?, created_at=?
        WHERE id=?;
        )";

        sqlite3_stmt *stmt = db_->prepare(sql);
        int col = 1;
        // Bind все поля
        sqlite3_bind_text(stmt, col++, t.date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.processing.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.fact_amount.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.fact_price.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.fact_quantity.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.rqs_amount.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.rqs_price.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.rqs_quantity.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.transaction_id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.card_number.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.fuel_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.trk_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.type_sale.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.id_tso.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, col++, t.after_density.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_filling.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_lower_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_lower_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_temperature.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_total_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_upper_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_upper_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.after_weight.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, col++, t.before_density.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_filling.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_lower_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_lower_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_temperature.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_total_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_upper_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_upper_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.before_weight.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, col++, t.discount.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.dispenser_volume_begin.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.dispenser_volume_end.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.dsc.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, t.receipt.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_int(stmt, col++, t.sent_to_server ? 1 : 0);
        sqlite3_bind_text(stmt, col++, t.created_at.c_str(), -1, SQLITE_TRANSIENT);

        // Bind WHERE id=?
        sqlite3_bind_int64(stmt, col++, t.id);

        int rc = sqlite3_step(stmt);
        db_->finalize(stmt);

        if (rc != SQLITE_DONE) {
            LOG_SYSTEM_ERROR << "Failed to update transaction";
            throw DatabaseException("Failed to update transaction");
        }

        LOG_SYSTEM_INFO << "Transaction updated: " << t.id;
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

    TransactionData TransactionRepository::mapFromStatement(sqlite3_stmt *stmt) {
        TransactionData t;

        t.id = sqlite3_column_int64(stmt, 0);
        t.date = safeGetText(stmt, 1);
        t.processing = safeGetText(stmt, 2);
        t.fact_amount = safeGetText(stmt, 3);
        t.fact_price = safeGetText(stmt, 4);
        t.fact_quantity = safeGetText(stmt, 5);
        t.rqs_amount = safeGetText(stmt, 6);
        t.rqs_price = safeGetText(stmt, 7);
        t.rqs_quantity = safeGetText(stmt, 8);
        t.transaction_id = safeGetText(stmt, 9);
        t.card_number = safeGetText(stmt, 10);
        t.fuel_name = safeGetText(stmt, 11);
        t.trk_name = safeGetText(stmt, 12);
        t.type_sale = safeGetText(stmt, 13);
        t.id_tso = safeGetText(stmt, 14);

        t.after_density = safeGetText(stmt, 15);
        t.after_filling = safeGetText(stmt, 16);
        t.after_lower_level = safeGetText(stmt, 17);
        t.after_lower_volume = safeGetText(stmt, 18);
        t.after_temperature = safeGetText(stmt, 19);
        t.after_total_volume = safeGetText(stmt, 20);
        t.after_upper_level = safeGetText(stmt, 21);
        t.after_upper_volume = safeGetText(stmt, 22);
        t.after_weight = safeGetText(stmt, 23);

        t.before_density = safeGetText(stmt, 24);
        t.before_filling = safeGetText(stmt, 25);
        t.before_lower_level = safeGetText(stmt, 26);
        t.before_lower_volume = safeGetText(stmt, 27);
        t.before_temperature = safeGetText(stmt, 28);
        t.before_total_volume = safeGetText(stmt, 29);
        t.before_upper_level = safeGetText(stmt, 30);
        t.before_upper_volume = safeGetText(stmt, 31);
        t.before_weight = safeGetText(stmt, 32);

        t.discount = safeGetText(stmt, 33);
        t.dispenser_volume_begin = safeGetText(stmt, 34);
        t.dispenser_volume_end = safeGetText(stmt, 35);
        t.dsc = safeGetText(stmt, 36);
        t.receipt = safeGetText(stmt, 37);

        t.sent_to_server = sqlite3_column_int(stmt, 38) != 0;
        t.created_at = safeGetText(stmt, 39);

        return t;
    }
}
