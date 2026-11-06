/**
 * @file PowerSupply.h
 * @brief Модель источника питания GW Instek PSW7-800.
 */

#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

/**
 * @brief Эмулирует физическое поведение источника питания.
 * 
 * Рассчитывает выходные параметры с учетом закона Ома, 
 * режимов стабилизации напряжения (CV) и тока (CC).
 */
class PowerSupply {
private:
    static float get_random_float_3dp(const float min, const float max);

    float voltage_limit = 12.000f;  ///< Текущий лимит напряжения в вольтах.
    float current_limit = 5.000f;   ///< Текущий лимит тока в амперах.
    float load_resistance = 100.0f; ///< Сопротивление нагрузки в Омах.

public:

    /**
     * @brief Проверяет, превышает ли теоретический ток установленный лимит.
     * @return true, если нагрузка требует больше тока, чем позволяет лимит.
     */
    bool is_overcurrent();

    /**
     * @brief Возвращает измеренное напряжение.
     * @return Напряжение с учетом лимитов и сопротивления нагрузки.
     */
    float get_voltage();

    /**
     * @brief Возвращает измеренный ток.
     * @return Ток, рассчитанный по закону Ома.
     */
    float get_current();

    /**
     * @brief Устанавливает лимит напряжения.
     * @param limit Новое значение. Отрицательное значение приводится к 0.
     */
    void set_voltage_limit(const float limit);

    /// @return Текущий лимит напряжения.
    float get_voltage_limit();

    /**
     * @brief Устанавливает лимит тока.
     * @param limit Новое значение. Отрицательное значение приводится к 0.
     */
    void set_current_limit(const float limit);

    /// @return Текущий лимит тока.
    float get_current_limit();

    /**
     * @brief Устанавливает сопротивление нагрузки.
     * @param resistance Сопротивление в Омах. Минимум 0.1 Ом.
     */
    void set_load_resistance(float resistance);

    /// @return Текущее сопротивление нагрузки.
    float get_load_resistance() const;
};

#endif // POWERSUPPLY_H