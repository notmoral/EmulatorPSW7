<div align="center">

# ⚡ EmulatorPSW7

**Продвинутый эмулятор лабораторных источников питания с поддержкой SCPI-команд, физической моделью и сетевыми протоколами.**

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/notmoral/EmulatorPSW7/actions)
[![Qt](https://img.shields.io/badge/Qt-6.10.2-blue)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C++-17-blue)](https://isocpp.org/)
[![Docker](https://img.shields.io/badge/Docker-Supported-blue)](https://www.docker.com/)
[![Documentation](https://img.shields.io/badge/docs-Doxygen-informational)](https://notmoral.github.io/EmulatorPSW7/)

</div>

---

## 📖 О проекте

**EmulatorPSW7** — это кроссплатформенный программный комплекс, предназначенный для эмуляции работы лабораторных источников питания. Проект создан для автоматизации тестирования программного обеспечения и отказа от использования дорогостоящего физического оборудования.

Эмулятор не просто выдает случайные числа. В него встроена **реалистичная физическая модель** (закон Ома), режимы стабилизации тока/напряжения (CV/CC), а также аппаратные защиты (OVP, OCP), что позволяет использовать его для безопасного стресс-тестирования промышленного ПО.

---

## ✨ Ключевые возможности

### 🔌 Сетевое взаимодействие
*   **TCP (порт 5025)** — классический SCPI-сервер.
*   **UDP (порт 5025)** — быстрая передача датаграмм.
*   **HTTP (порт 8080)** — REST API с поддержкой `GET` и `POST` запросов, защищенное базовой аутентификацией (Basic Auth).
*   **WebSocket (порт 8081)** — используется для мгновенных уведомлений графического интерфейса об изменениях.

### 🔬 Физическая модель и Защиты
*   **Закон Ома:** Расчет тока и напряжения в зависимости от сопротивления нагрузки.
*   **Режим CV (Constant Voltage):** Поддержание заданного напряжения при высоком сопротивлении нагрузки.
*   **Режим CC (Constant Current):** Снижение напряжения при коротком замыкании для удержания тока в рамках лимита.
*   **OVP (Over-Voltage Protection):** Аппаратная защита от перенапряжения с автоматическим отключением выхода.
*   **OCP (Over-Current Protection):** Защита от перегрузки по току.
*   **Очередь ошибок:** Стандартная SCPI-команда `SYSTem:ERRor?` для чтения логов срабатывания защит.

### 💻 Интерфейс и Данные
*   Темная тема интерфейса (Qt Quick / QML).
*   Встроенная база данных **SQLite** с автоматической миграцией схемы.
*   История команд с возможностью фильтрации по прибору или тексту команды.
*   Экспорт истории в **CSV** и **JSON**.
*   Импорт списка команд из текстового файла.

---

## 🛠 Поддерживаемые приборы

| Прибор | Каналы | Макс. напряжение | Макс. ток | Особенности |
| :--- | :--- | :--- | :--- | :--- |
| **GW Instek PSW7-800** | 1 | 12.0 V | 5.0 A | Базовая SCPI-модель |
| **Keysight E36312A** | 3 | 6V / 25V / 25V | 5A / 1A / 1A | Выбор канала (`INSTrument:NSELect`) |
| **Rigol DP832** | 3 | 30V / 30V / 5V | 3A / 3A / 3A | Управление каналом (`OUTPut1:STATe`) |

---

## 🚀 Начало работы

### Системные требования
*   **ОС:** Windows 10/11 (64-bit) или Linux (для Docker).
*   **Компилятор:** Visual Studio 2022 (MSVC) или GCC.
*   **CMake:** версия 3.16 или выше.
*   **Qt:** версия 6.10.2 (msvc2022_64) с модулями Network, SQL, WebSockets.

### Сборка под Windows (GUI)

Откройте `Developer Command Prompt for VS 2022` и выполните:

```cmd
# 1. Перейдите в папку проекта
cd C:\путь\к\проекту\EmulatorPSW7

# 2. Сгенерируйте файлы сборки
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.10.2\msvc2022_64" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release

# 3. Соберите проект
cmake --build build
Запуск
cmd
# Добавьте библиотеки Qt в PATH (чтобы приложение нашло DLL)
set PATH=C:\Qt\6.10.2\msvc2022_64\bin;%PATH%

# Запустите приложение
build\EmulatorPSW7.exe
```
### Запуск в Docker (Headless-режим)

Для запуска на серверах без графического интерфейса:

```bash
# Сборка образа
docker build -t emulatorpsw7 .

# Запуск контейнера (порты 5025, 8080, 8081)
docker run -d --name emulator -p 5025:5025 -p 5025:5025/udp -p 8080:8080 -p 8081:8081 emulatorpsw7
```
### 🧪 Быстрый тест (Примеры команд)

Вы можете отправить команды через любой TCP-клиент (например, nc или PowerShell):

```powershell
$c = New-Object Net.Sockets.TcpClient("127.0.0.1", 5025)
$s = $c.GetStream()
$w = New-Object IO.StreamWriter($s); $w.AutoFlush = $true
$r = New-Object IO.StreamReader($s)

# Проверка связи
$w.WriteLine("[DEVICE=PSW7]*IDN?")
$r.ReadLine()

# Установка лимитов (10V, 2A)
$w.WriteLine("[DEVICE=PSW7]SOURce:VOLTage:LIMit 10")
$w.WriteLine("[DEVICE=PSW7]SOURce:CURRent:LIMit 2")

# Включение защиты от перенапряжения
$w.WriteLine("[DEVICE=PSW7]VOLTage:PROTection:STATe ON")

# Чтение ошибок
$w.WriteLine("[DEVICE=PSW7]SYSTem:ERRor?")
$r.ReadLine()

$c.Close()
```
### 🧪 Тестирование

Проект использует Qt Test для юнит-тестирования и интеграционных проверок. CI/CD настроен через GitHub Actions (файл .github/workflows/build.yml).

```cmd
build\EmulatorPSW7Tests.exe          # Тесты физики и защит
build\EmulatorPSW7DatabaseTests.exe  # Тесты БД
build\EmulatorPSW7UdpTests.exe       # Интеграционные тесты UDP
build\EmulatorPSW7HttpTests.exe      # Интеграционные тесты HTTP
build\EmulatorPSW7ImporterTests.exe  # Тесты импорта команд
```
### 📚 Документация

📖 GitHub Wiki — подробное описание архитектуры и команд.

📑 Doxygen HTML — автоматически сгенерированная документация по API классов.

### 🗺 Архитектура

Проект построен на паттерне Стратегия.

Интерфейс IInstrument определяет контракт для всех приборов.

ScpiParser выступает в роли диспетчера, маршрутизируя команды на основе префикса [DEVICE=NAME].

Все сетевые серверы (TcpServer, UdpServer, HttpServer) независимы и делегируют парсинг команд ядру.

<div align="center"> <i>Разработано в рамках технологической практики. 2026 год.</i> </div> ```
