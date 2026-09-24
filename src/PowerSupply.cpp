#include "PowerSupply.h"
#include <QRandomGenerator>

float PowerSupply::get_random_float_3dp(const float min, const float max) {
    double random = QRandomGenerator::global()->generateDouble();
    float value = min + static_cast<float>(random) * (max - min);

    return std::round(value * 1000.0f) / 1000.0f;
}

float PowerSupply::get_voltage()  {
    return (voltage_limit - 1) + get_random_float_3dp(0, 1);
}

void PowerSupply::set_voltage_limit(const float limit) {
    voltage_limit = limit;
}

float PowerSupply::get_voltage_limit() {
    return voltage_limit;
}