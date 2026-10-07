/**
 * @file PowerSupply.h
 * @brief Модель источника питания GW Instek PSW7-800.
 */

#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

/**
 * @brief Эмулирует поведение источника питания.
 *
 * Хранит текущий лимит напряжения и генерирует псевдослучайные
 * значения измеряемого напряжения в диапазоне [limit-1, limit].
 */
class PowerSupply {
private:
    /**
     * @brief Генерирует случайное число с 3 знаками после запятой.
     * @param min Нижняя граница диапазона.
     * @param max Верхняя граница диапазона.
     * @return Случайное значение в диапазоне [min, max].
     */
    static float get_random_float_3dp(const float min, const float max);

    /// Текущий лимит напряжения в вольтах. По умолчанию 12.0.
    float voltage_limit = 12.000f;

public:
    /**
     * @brief Возвращает измеренное напряжение.
     * @return Значение в диапазоне [voltage_limit - 1, voltage_limit].
     */
    float get_voltage();

    /**
     * @brief Устанавливает лимит напряжения.
     * @param limit Новое значение. Отрицательное значение приводится к 0.
     */
    void set_voltage_limit(const float limit);

    /**
     * @brief Возвращает текущий лимит напряжения.
     * @return Лимит в вольтах.
     */
    float get_voltage_limit();
};

#endif // POWERSUPPLY_H
