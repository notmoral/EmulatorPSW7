/**
 * @file tst_HttpEmulator.cpp
 * @brief Интеграционные тесты HTTP-транспорта эмулятора.
 *
 * Требует, чтобы `EmulatorPSW7.exe` был запущен заранее.
 */

#include <QtTest/QTest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QUrlQuery>

class TestHttpEmulator : public QObject
{
    Q_OBJECT

private:
    QJsonObject httpGet(const QString& query);

private slots:
    void http_idn_psw7();
    void http_idn_e36312a();
    void http_measure_psw7();
    void http_unknown_path();
    void http_missing_cmd();
};

QJsonObject TestHttpEmulator::httpGet(const QString& query)
{
    QNetworkAccessManager mgr;

    QUrl url("http://127.0.0.1:8080/scpi");
    QUrlQuery q;
    // query — это уже готовая строка вида "cmd=...&device=..."
    q.setQuery(query);
    url.setQuery(q);

    QNetworkRequest req(url);
    QNetworkReply* reply = mgr.get(req);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    const QByteArray data = reply->readAll();
    reply->deleteLater();

    return QJsonDocument::fromJson(data).object();
}

void TestHttpEmulator::http_idn_psw7()
{
    const QJsonObject obj = httpGet("cmd=*IDN?&device=PSW7");

    QVERIFY(!obj.isEmpty());
    QCOMPARE(obj.value("device").toString(), QString("PSW7"));
    QVERIFY(obj.value("response").toString().contains("GW-INSTEK"));
}

void TestHttpEmulator::http_idn_e36312a()
{
    const QJsonObject obj = httpGet("cmd=*IDN?&device=E36312A");

    QVERIFY(!obj.isEmpty());
    QCOMPARE(obj.value("device").toString(), QString("E36312A"));
    QVERIFY(obj.value("response").toString().contains("Keysight"));
}

void TestHttpEmulator::http_measure_psw7()
{
    httpGet("cmd=SOURce:VOLTage:LIMit%2015&device=PSW7");

    const QJsonObject obj = httpGet("cmd=MEASure:VOLTage:DC?&device=PSW7");

    bool ok = false;
    const float voltage = obj.value("response").toString().toFloat(&ok);
    QVERIFY2(ok, "Invalid voltage value over HTTP");
    QVERIFY2(voltage >= 14.0f && voltage <= 15.0f,
             "Voltage outside [14.0, 15.0] range over HTTP");
}

void TestHttpEmulator::http_unknown_path()
{
    QNetworkAccessManager mgr;
    QNetworkRequest req(QUrl("http://127.0.0.1:8080/unknown"));
    QNetworkReply* reply = mgr.get(req);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();

    QCOMPARE(status, 404);
}

void TestHttpEmulator::http_missing_cmd()
{
    QNetworkAccessManager mgr;
    QNetworkRequest req(QUrl("http://127.0.0.1:8080/scpi?device=PSW7"));
    QNetworkReply* reply = mgr.get(req);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();

    QCOMPARE(status, 400);
}

QTEST_MAIN(TestHttpEmulator)

#include "tst_HttpEmulator.moc"