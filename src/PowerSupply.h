/**
 * @file PowerSupply.h
 * @brief Модель источника питания GW Instek PSW7-800.
 */

#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

/**
 * @brief Эмулирует поведение источника питания.
 */
class PowerSupply {
private:
    static float get_random_float_3dp(const float min, const float max);

    float voltage_limit = 12.000f;  ///< Текущий лимит напряжения в вольтах.
    float current_limit = 5.000f;   ///< Текущий лимит тока в амперах.
    float load_resistance = 100.0f; ///< Сопротивление нагрузки в Омах.

public:
    float get_voltage();
    float get_current();

    void set_voltage_limit(const float limit);
    float get_voltage_limit();

    void set_current_limit(const float limit);
    float get_current_limit();

    void set_load_resistance(float resistance);
    float get_load_resistance() const;
};

#endif // POWERSUPPLY_H