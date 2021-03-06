#ifndef EXCHANGESIMULATOR_SERVER_ORDERHANDLER_H
#define EXCHANGESIMULATOR_SERVER_ORDERHANDLER_H


#include "Fix42.h"
#include "MessageHandler.h"
#include "Quote.h"
#include "APIUtil.h"

#include <vector>
#include <memory>

class OrderHandler : public MessageHandler {
public:

    OrderHandler(APIUtil::ConnPtr conn) : MessageHandler(conn) { }

    virtual std::vector<Fix42::MessagePtr> accept(const Fix42::MessagePtr message) override;

    void handleSingleOrder(const Quote &quote, std::vector<Fix42::MessagePtr> &results);

    std::vector<Fix42::MessagePtr> handleSingleRequest(const Fix42::MessagePtr message);

private:

    Fix42::MessagePtr createMsgAccept(const Quote &quote);

    Fix42::MessagePtr createMsgReject(const Quote &quote);

    void addBasicFields(const Quote &quote, std::shared_ptr<Fix42::Message> &message) const;


};


#endif //EXCHANGESIMULATOR_SERVER_ORDERHANDLER_H
