#include "gamestate.h"
#include <QDebug>
#include <QJsonArray>

using namespace std;

void GameState::dump() const
{

    qDebug() << "stock:" << stock.toString();
    qDebug() << "waste:" << waste.toString();
    for (const auto &stack : stacks) {
        qDebug() << "stack:" << stack.toString();
    }
    for (const auto &table : tables) {
        qDebug() << "table:" << table.toString();
    }
}

QJsonObject GameState::toJson() const
{
    QJsonObject obj;

    auto pileToJson = [](const Pile &pile) {
        QJsonObject pileObj;
        pileObj["type"] = static_cast<int>(pile.type);
        pileObj["index"] = pile.index;
        QJsonArray cardsArray;
        for (const auto &card : pile.cards) {
            QJsonObject cardObj;
            cardObj["suit"] = static_cast<int>(card.suit);
            cardObj["rank"] = static_cast<int>(card.rank);
            cardObj["side"] = static_cast<int>(card.side);
            cardsArray.append(cardObj);
        }
        pileObj["cards"] = cardsArray;
        return pileObj;
    };

    obj["stock"] = pileToJson(stock);
    obj["waste"] = pileToJson(waste);

    QJsonArray stacksArray;
    for (const auto &stack : stacks) {
        stacksArray.append(pileToJson(stack));
    }
    obj["stacks"] = stacksArray;

    QJsonArray tablesArray;
    for (const auto &table : tables) {
        tablesArray.append(pileToJson(table));
    }
    obj["tables"] = tablesArray;

    return obj;
}

GameState GameState::fromJson(const QJsonObject &obj)
{
    GameState state;

    auto jsonToPile = [](const QJsonObject &pileObj) {
        Pile pile;
        pile.type = static_cast<PileType>(pileObj["type"].toInt());
        pile.index = pileObj["index"].toInt();
        QJsonArray cardsArray = pileObj["cards"].toArray();
        for (const auto &cardValue : cardsArray) {
            QJsonObject cardObj = cardValue.toObject();
            Card card;
            card.suit = static_cast<Suit>(cardObj["suit"].toInt());
            card.rank = static_cast<Rank>(cardObj["rank"].toInt());
            card.side = static_cast<Side>(cardObj["side"].toInt());
            pile.cards.push_back(card);
        }
        return pile;
    };

    state.stock = jsonToPile(obj["stock"].toObject());
    state.waste = jsonToPile(obj["waste"].toObject());

    QJsonArray stacksArray = obj["stacks"].toArray();
    for (const auto &stackValue : stacksArray) {
        state.stacks.push_back(jsonToPile(stackValue.toObject()));
    }

    QJsonArray tablesArray = obj["tables"].toArray();
    for (const auto &tableValue : tablesArray) {
        state.tables.push_back(jsonToPile(tableValue.toObject()));
    }
    return state;
}
