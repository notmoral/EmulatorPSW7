# EmulatorPSW7

Эмулятор источника питания GW Instek PSW7-800.

Проект написан на C++ с использованием Qt.
Эмулятор работает через TCP и принимает SCPI команды.

## Требования

- C++17 или выше
- Qt 5.15 / Qt 6.x
- CMake 3.16+
- Qt Network
- Qt Test

## Сборка

Клонировать репозиторий:

git clone <repository-url>
cd EmulatorPSW7

Создать build директорию:

cmake -S . -B build

Собрать проект:

cmake --build build

После сборки доступны:

- EmulatorPSW7 — основной эмулятор
- EmulatorPSW7Tests — unit tests
- EmulatorPSW7IntegrationTests — integration tests

## Запуск

Запустить эмулятор:

./build/EmulatorPSW7

После запуска сервер работает на:

127.0.0.1:5025

В консоли должно появиться:

Server started!

Для подключения можно использовать nc:

nc 127.0.0.1 5025

## SCPI команды

### Получение информации об устройстве

*IDN?

Ответ:

GW-INSTEK,PSW800-4.32,TW123456,01.00.20110101

### Проверка выполнения команды

*OPC?

Ответ:

1

### Установка ограничения напряжения

SOURce:VOLTage:LIMit 10

После этого ограничение напряжения устанавливается на 10 V.

### Получение ограничения напряжения

SOURce:VOLTage:LIMit?

Ответ:

10

### Получение измерения напряжения

MEASure:VOLTage:DC?

Пример ответа:

9.426

При установленном ограничении 10 V значение находится в диапазоне:

9.0 - 10.0 V

## Тесты

В проекте используются Qt Test.

### Unit tests

Unit tests проверяют работу класса PowerSupply.

Сборка:

cmake --build build --target EmulatorPSW7Tests

Запуск:

./build/EmulatorPSW7Tests

Проверяется:

- значение ограничения по умолчанию;
- установка ограничения;
- получение напряжения;
- нахождение напряжения в допустимом диапазоне;
- обработка отрицательного ограничения.

### Integration tests

Integration tests работают с запущенным эмулятором через TCP.

Сначала необходимо запустить эмулятор:

./build/EmulatorPSW7

В другом терминале:

cmake --build build --target EmulatorPSW7IntegrationTests

Запустить тест:

./build/Desktop_arm_darwin_generic_mach_o_64bit_Debug/EmulatorPSW7IntegrationTests

Тест проверяет:

- подключение к 127.0.0.1:5025;
- получение *IDN?;
- установку ограничения 10 V;
- получение 100 измерений;
- корректность каждого измерения;
- 100 последовательных переподключений.

При успешном прохождении:

Totals: 4 passed, 0 failed, 0 skipped, 0 blacklisted

## Git

Для разработки используются отдельные ветки:

main
dev
issue/<number>-<description>

Пример создания ветки:

git checkout dev
git pull
git checkout -b issue/8-cpp-tests

После выполнения задачи:

git add .
git commit -m "Add integration tests for emulator"
git push -u origin issue/8-cpp-tests

После этого создаётся Pull Request:

issue/8-cpp-tests -> dev

## Статус проекта

Сейчас реализованы:

- TCP сервер;
- SCPI parser;
- эмуляция напряжения;
- основные SCPI команды;
- unit tests;
- integration tests;
- тестирование 100 измерений;
- тестирование 100 переподключений.
