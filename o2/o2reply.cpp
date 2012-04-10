#include <QTimer>
#include <QNetworkReply>

#include "o2reply.h"

O2Reply::O2Reply(QNetworkReply *r, int timeOut, QObject *parent): QTimer(parent), reply(r) {
    setSingleShot(true);
    connect(this, SIGNAL(error(QNetworkReply::NetworkError)), reply, SIGNAL(error(QNetworkReply::NetworkError)));
    connect(this, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    start(timeOut);
}

void O2Reply::onTimeOut() {
    emit error(QNetworkReply::TimeoutError);
}

O2ReplyList::~O2ReplyList() {
    foreach (O2Reply *timedReply, replies_) {
        delete timedReply;
    }
}

void O2ReplyList::add(QNetworkReply *reply) {
    add(new O2Reply(reply));
}

void O2ReplyList::add(O2Reply *reply) {
    replies_.append(reply);
}

void O2ReplyList::remove(QNetworkReply *reply) {
    (void)replies_.removeOne(find(reply));
}

O2Reply *O2ReplyList::find(QNetworkReply *reply) {
    foreach (O2Reply *timedReply, replies_) {
        if (timedReply->reply == reply) {
            return timedReply;
        }
    }
    return 0;
}
