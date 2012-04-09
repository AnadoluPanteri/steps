/*
O2: OAuth 2.0 authenticator for Qt.

Author: Akos Polster (akos@pipacs.com). Inspired by KQOAuth, the OAuth library made by Johan Paul (johan.paul@d-pointer.com).

O2 is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

O2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with O2. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QList>
#include <QPair>
#include <QDebug>
#include <QTcpServer>
#include <QMap>
#include <QSettings>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QScriptEngine>
#include <QDateTime>
#include <QCryptographicHash>
#include <QTimer>

#include "o2.h"
#include "o2replyserver.h"
#include "simplecrypt.h"

O2::O2(const QString &clientId, const QString &clientSecret, const QString &scope, const QUrl &requestUrl, const QUrl &tokenUrl, const QUrl &refreshTokenUrl, QObject *parent): QObject(parent), clientId_(clientId), clientSecret_(clientSecret), scope_(scope), requestUrl_(requestUrl), tokenUrl_(tokenUrl), refreshTokenUrl_(refreshTokenUrl) {
    QByteArray hash = QCryptographicHash::hash(clientSecret.toUtf8() + "12345678", QCryptographicHash::Sha1);
    crypt_ = new SimpleCrypt(*((quint64 *)(void *)hash.data()));
    manager_ = new QNetworkAccessManager(this);
    replyServer_ = new O2ReplyServer(this);
    connect(replyServer_, SIGNAL(verificationReceived(QMap<QString,QString>)), this, SLOT(onVerificationReceived(QMap<QString,QString>)));
}

O2::~O2() {
    delete crypt_;
}

void O2::link() {
    if (linked()) {
        return;
    }

    // Start listening to authentication replies
    replyServer_->listen();

    // Save redirect URI, as we have to reuse it when requesting the access token
    redirectUri_ = QString("http://localhost:%1").arg(replyServer_->serverPort());

    // Assemble intial authentication URL
    QList<QPair<QString, QString> > parameters;
    parameters.append(qMakePair(QString("response_type"), QString("code")));
    parameters.append(qMakePair(QString("client_id"), clientId_));
    parameters.append(qMakePair(QString("redirect_uri"), redirectUri_));
    parameters.append(qMakePair(QString("scope"), scope_));

    // Show authentication URL with a web browser
    QUrl url(requestUrl_);
    url.setQueryItems(parameters);
    emit openBrowser(url);
}

void O2::unlink() {
    if (!linked()) {
        return;
    }
    setToken(QString());
    setRefreshToken(QString());
    setExpires(0);
    emit linkedChanged();
}

bool O2::linked() {
    return token().length();
}

void O2::onVerificationReceived(const QMap<QString, QString> response) {
    emit closeBrowser();
    if (response.contains("error")) {
        qDebug() << "O2::onVerificationReceived: Verification failed";
        emit linkingFailed();
        return;
    }

    // Save access code
    setCode(response.value(QString("code")));

    // Exchange access code for access/refresh tokens
    QNetworkRequest tokenRequest(tokenUrl_);
    tokenRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QMap<QString, QString> parameters;
    parameters.insert("code", code());
    parameters.insert("client_id", clientId_);
    parameters.insert("client_secret", clientSecret_);
    parameters.insert("redirect_uri", redirectUri_);
    parameters.insert("grant_type", "authorization_code");
    QByteArray data = buildRequestBody(parameters);
    tokenReply_ = manager_->post(tokenRequest, data);
    timedReplies_.addReply(tokenReply_);
    connect(tokenReply_, SIGNAL(finished()), this, SLOT(onTokenReplyFinished()));
    connect(tokenReply_, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onTokenReplyError(QNetworkReply::NetworkError)));
}

QString O2::code() {
    QString key = QString("code.%1").arg(clientId_);
    return crypt_->decryptToString(QSettings().value(key).toString());
}

void O2::setCode(const QString &c) {
    QString key = QString("code.%1").arg(clientId_);
    QSettings().setValue(key, crypt_->encryptToString(c));
}

void O2::onTokenReplyFinished() {
    if (tokenReply_->error() == QNetworkReply::NoError) {
        QByteArray reply = tokenReply_->readAll();
        QScriptValue value;
        QScriptEngine engine;
        value = engine.evaluate("(" + QString(reply) + ")");
        setToken(value.property("access_token").toString());
        setExpires(QDateTime::currentMSecsSinceEpoch() / 1000 + value.property("expires_in").toInteger());
        setRefreshToken(value.property("refresh_token").toString());
        qDebug() << "O2::onTokenReplyFinished: Token expires in" << value.property("expires_in").toInteger() << "seconds";
        timedReplies_.removeReply(tokenReply_);
        emit linkingSucceeded();
        emit tokenChanged();
        emit linkedChanged();
    }
    tokenReply_->deleteLater();
}

void O2::onTokenReplyError(QNetworkReply::NetworkError error) {
    qDebug() << "O2::onTokenReplyError" << error << tokenReply_->errorString();
    setToken(QString());
    setRefreshToken(QString());
    timedReplies_.removeReply(tokenReply_);
    emit tokenChanged();
    emit linkingFailed();
    emit linkedChanged();
}

QByteArray O2::buildRequestBody(const QMap<QString, QString> &parameters) {
    QByteArray body;
    bool first = true;
    foreach (QString key, parameters.keys()) {
        if (first) {
            first = false;
        } else {
            body.append("&");
        }
        QString value = parameters.value(key);
        body.append(QUrl::toPercentEncoding(key) + QString("=").toUtf8() + QUrl::toPercentEncoding(value));
    }
    return body;
}

QString O2::token() {
    QString key = QString("token.%1").arg(clientId_);
    return crypt_->decryptToString(QSettings().value(key).toString());
}

void O2::setToken(const QString &v) {
    QString key = QString("token.%1").arg(clientId_);
    QSettings().setValue(key, crypt_->encryptToString(v));
}

int O2::expires() {
    QString key = QString("expires.%1").arg(clientId_);
    return QSettings().value(key).toInt();
}

void O2::setExpires(int v) {
    QString key = QString("expires.%1").arg(clientId_);
    QSettings().setValue(key, v);
}

QString O2::refreshToken() {
    QString key = QString("refreshtoken.%1").arg(clientId_);
    QString ret = crypt_->decryptToString(QSettings().value(key).toString());
    return ret;
}

void O2::setRefreshToken(const QString &v) {
    QString key = QString("refreshtoken.%1").arg(clientId_);
    QSettings().setValue(key, crypt_->encryptToString(v));
}

void O2::refresh() {
    qDebug() << "O2::refresh: Token: ..." << refreshToken().right(7);

    if (!refreshToken().length()) {
        qWarning() << "O2::refresh: No refresh token";
        onRefreshError(QNetworkReply::AuthenticationRequiredError);
        return;
    }

    QNetworkRequest refreshRequest(refreshTokenUrl_);
    refreshRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QMap<QString, QString> parameters;
    parameters.insert("client_id", clientId_);
    parameters.insert("client_secret", clientSecret_);
    parameters.insert("refresh_token", refreshToken());
    parameters.insert("grant_type", "refresh_token");
    QByteArray data = buildRequestBody(parameters);
    refreshReply_ = manager_->post(refreshRequest, data);
    timedReplies_.addReply(refreshReply_);
    connect(refreshReply_, SIGNAL(finished()), this, SLOT(onRefreshFinished()));
    connect(refreshReply_, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onRefreshError(QNetworkReply::NetworkError)));
}

void O2::onRefreshFinished() {
    qDebug() << "O2::onRefreshFinished";
    if (refreshReply_->error() == QNetworkReply::NoError) {
        QByteArray reply = refreshReply_->readAll();
        QScriptValue value;
        QScriptEngine engine;
        value = engine.evaluate("(" + QString(reply) + ")");
        setToken(value.property("access_token").toString());
        setExpires(QDateTime::currentMSecsSinceEpoch() / 1000 + value.property("expires_in").toInteger());
        setRefreshToken(value.property("refresh_token").toString());
        timedReplies_.removeReply(refreshReply_);
        emit linkingSucceeded();
        emit tokenChanged();
        emit linkedChanged();
        emit refreshFinished(QNetworkReply::NoError);
        qDebug() << "O2::refreshToken: New token expires in" << expires() << "seconds";
    }
    refreshReply_->deleteLater();
}

void O2::onRefreshError(QNetworkReply::NetworkError error) {
    qDebug() << "O2::onRefreshFailed:" << error;
    setToken(QString());
    setRefreshToken(QString());
    timedReplies_.removeReply(refreshReply_);
    emit tokenChanged();
    emit linkingFailed();
    emit linkedChanged();
    emit refreshFinished(error);
}
