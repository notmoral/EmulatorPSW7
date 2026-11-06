/**
 * @file PowerSupply.cpp
 * @brief Реализация физической модели источника питания.
 */

#include "PowerSupply.h"
#include <QRandomGenerator>
#include <cmath>
#include <algorithm>

float PowerSupply::get_random_float_3dp(const float min, const float max) {
    double random = QRandomGenerator::global()->generateDouble();
    float value = min + static_cast<float>(random) * (max - min);
    return std::round(value * 1000.0f) / 1000.0f;
}

float PowerSupply::get_voltage() {
    if (load_resistance <= 0.0f) return 0.0f;

    // Закон Ома: I = U / R
    float theoretical_current = voltage_limit / load_resistance;
    float actual_voltage = 0.0f;

    if (theoretical_current <= current_limit) {
        // Режим стабилизации напряжения (CV)
        actual_voltage = voltage_limit;
    } else {
        // Режим стабилизации тока (CC)
        actual_voltage = current_limit * load_resistance;
    }

    // Добавляем небольшую погрешность измерения (шум)
    float noise = get_random_float_3dp(-0.05f, 0.05f);
    actual_voltage += noise;

    // Не даем шуму вывести напряжение за установленный лимит
    if (actual_voltage > voltage_limit) {
        actual_voltage = voltage_limit;
    }

    return (actual_voltage < 0.0f) ? 0.0f : std::round(actual_voltage * 1000.0f) / 1000.0f;
}

float PowerSupply::get_current() {
    if (load_resistance <= 0.0f) return 0.0f;

    float theoretical_current = voltage_limit / load_resistance;
    float actual_current = std::min(theoretical_current, current_limit);

    float noise = get_random_float_3dp(-0.01f, 0.01f);
    actual_current += noise;

    return (actual_current < 0.0f) ? 0.0f : std::round(actual_current * 1000.0f) / 1000.0f;
}

void PowerSupply::set_voltage_limit(const float limit) {
    if (limit < 0) {
        voltage_limit = 0;
        return;
    }
    voltage_limit = limit;
}

float PowerSupply::get_voltage_limit() {
    return voltage_limit;
}

void PowerSupply::set_current_limit(const float limit) {
    if (limit < 0) {
        current_limit = 0;
        return;
    }
    current_limit = limit;
}

float PowerSupply::get_current_limit() {
    return current_limit;
}

void PowerSupply::set_load_resistance(float resistance) {
    if (resistance < 0.1f) {
        load_resistance = 0.1f;
        return;
    }
    load_resistance = resistance;
}

float PowerSupply::get_load_resistance() const {
    return load_resistance;
}

bool PowerSupply::is_overcurrent() {
    if (load_resistance <= 0.0f) return true;
    float theoretical_current = voltage_limit / load_resistance;
    return theoretical_current > current_limit;
}