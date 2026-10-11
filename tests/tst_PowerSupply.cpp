#include <QtTest/QTest>
#include "../src/PowerSupply.h"

class TestPowerSupply : public QObject {
    Q_OBJECT
private slots:
    void default_voltage_limit();
    void set_voltage_limit();
    void voltage_is_within_limit();
    void voltage_is_within_limit_data();
    void negative_voltage_limit();
};

void TestPowerSupply::negative_voltage_limit() {
    PowerSupply ps;
    ps.set_voltage_limit(-1.0f);
    QVERIFY(ps.get_voltage_limit() >= 0);
}

void TestPowerSupply::default_voltage_limit()
{
    PowerSupply ps;
    QCOMPARE(ps.get_voltage_limit(), 12.0f);
}

void TestPowerSupply::set_voltage_limit() {
    PowerSupply ps;
    ps.set_voltage_limit(3.0f);
    QCOMPARE(ps.get_voltage_limit(), 3.0f);
}

void TestPowerSupply::voltage_is_within_limit_data()
{
    QTest::addColumn<float>("limit");

    QTest::newRow("zero") << 0.0f;
    QTest::newRow("one") << 1.0f;
    QTest::newRow("ten") << 10.0f;
    QTest::newRow("default") << 12.0f;
    QTest::newRow("hundred") << 100.0f;
}

void TestPowerSupply::voltage_is_within_limit()
{
    QFETCH(float, limit);

    PowerSupply ps;
    ps.set_voltage_limit(limit);

    float v = ps.get_voltage();

    QVERIFY(v >= 0.0f);
    QVERIFY(v <= limit);
}

QTEST_MAIN(TestPowerSupply)

#include "tst_PowerSupply.moc"