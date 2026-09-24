#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

class PowerSupply {
private:
    static float get_random_float_3dp(const float min, const float max);

    float voltage_limit = 12.000f;

public:

    float get_voltage();

    void set_voltage_limit(const float limit);
    float get_voltage_limit();
};

#endif // POWERSUPPLY_H
