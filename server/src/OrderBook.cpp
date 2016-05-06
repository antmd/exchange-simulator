#include "OrderBook.h"
#include "APIUtil.h"

const double OrderBook::MAX_PRICE_DELTA = 0.1;
const double OrderBook::MAX_ORDER_VOLUMN = 0.05;

OrderBook::OrderBook(std::string symbol, std::shared_ptr<sql::Statement> stmt) : symbol_(symbol), stmt_(stmt) {
    APIUtil::securityQuery(stmt_, symbol_);
}

OrderBook::~OrderBook() {
    if (has_lock_) {
        unlock();
    }
}

void OrderBook::put(const Quote &quote) {
    APIUtil::orderbookPut(stmt_, quote);
}

void OrderBook::lock() {
    bool locked = false;
    do {
        APIUtil::securityTryLock(stmt_, symbol_, locked);
    } while (!locked);
    has_lock_ = true;
}

void OrderBook::unlock() {
    APIUtil::securityUnlock(stmt_, symbol_);
    has_lock_ = false;
}

std::vector<TradeRecord> OrderBook::execute() {
    bool is_running = false;
    APIUtil::systemStatusIsRunning(stmt_, is_running);
    if (!is_running) {
        return std::vector<TradeRecord>();
    }

    lock(); //TODO check whether locked
    std::queue<Quote> buy_quotes, sell_quotes;
    std::vector<TradeRecord> records;
    match(buy_quotes, sell_quotes);

    //buy/sell_quotes are sorted by price and time descending order
    while (!buy_quotes.empty() && !sell_quotes.empty()) {
        if (buy_quotes.front().price == sell_quotes.front().price) {

            auto &buy = buy_quotes.front();
            auto &sell = sell_quotes.front();
            TradeRecord record;

            int qty = std::min(buy.quantity, sell.quantity);
            buy.quantity -= qty;
            sell.quantity -= qty;

            //TODO what if failed
            updateQuote(buy);
            updateQuote(sell);
            security_status_.price = buy.price;
            updatePrice();

            record.price = buy.price;
            record.quantity = qty;
            record.order_buy = buy.id;
            record.order_sell = sell.id;
            records.push_back(record);

            if (buy.quantity == 0) {
                buy_quotes.pop();
            }
            if (sell.quantity == 0) {
                sell_quotes.pop();
            }

        } else if (buy_quotes.front().price < sell_quotes.front().price) {
            sell_quotes.pop();
        } else { /* buy_quotes.front().price > sell_quotes.front().price */
            buy_quotes.pop();
        }

    }
    unlock();

    for (auto &record:records) {
        APIUtil::tradeRecordPut(stmt_, record);
    }
    return records;
}

void OrderBook::match(std::queue<Quote> &buy, std::queue<Quote> &sell) {
    APIUtil::orderbookQueryMatch(stmt_, symbol_);

    std::shared_ptr<sql::ResultSet> res(stmt_->getResultSet());
    while (res->next()) {
        Quote quote(res);
        if (quote.side == kTradeSide::kBuy) {
            buy.push(quote);
        } else if (quote.side == kTradeSide::kSell) {
            sell.push(quote);
        }
    }
    stmt_->getMoreResults(); //free last result set
}

void OrderBook::updateQuote(const Quote &quote) {
    if (quote.quantity == 0) {
        APIUtil::orderbookDelete(stmt_, quote.id);
    } else {
        APIUtil::orderbookUpdate(stmt_, quote.id, quote.quantity);
    }
}

void OrderBook::updatePrice() {
    APIUtil::securityUpdatePrice(stmt_, security_status_);
}

bool OrderBook::isValid(const Quote &quote) const {
    int prev = security_status_.prev_close;
    int delta = prev * 0.1;
    if(quote.price < prev-delta || quote.price > prev+delta){
        return false;
    }
    if(quote.quantity * quote.price > security_status_.market_cap * 0.05){
        return false;
    }
    return true;
}