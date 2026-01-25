# Telegram VPN Service

Безопасный VPN-сервис с доступом через Telegram-бота.

<img src="https://img.shields.io/badge/bash-4EAA25?style=for-the-badge&logo=gnubash&logoColor=ffffff"/> <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=ffffff"/> <img src="https://img.shields.io/badge/cmake-064F8C?style=for-the-badge&logo=cmake&logoColor=ffffff"/> <img src="https://img.shields.io/badge/mongodb-47A248?style=for-the-badge&logo=mongodb&logoColor=ffffff"/> <img src="https://img.shields.io/badge/telegram-26A5E4?style=for-the-badge&logo=Telegram&logoColor=ffffff"/> <img src="https://img.shields.io/badge/docker-2496ED?style=for-the-badge&logo=docker&logoColor=ffffff"/> <img src="https://img.shields.io/badge/git-F05032?style=for-the-badge&logo=git&logoColor=ffffff"/>

---

## Особенности

- Быстрая, разовая установка и настройка
- Управление VPN-доступом через Telegram-бота
- Самостоятельное управление подпиской пользователями
- Базовая автоматическая харденизация сервера:
    - отключение root-доступа
    - доступ по SSH-ключам
    - кастомный SSH-порт
- Контейнеризация сервисов (Docker)

---

## Требования

- Сервер с **Ubuntu 24.04 LTS**
- Локальная машина на Linux с ssh и доступом в интернет
- **Telegram Bot Token** (получить у [@BotFather](https://t.me/botfather))

---

## Установка

Выполните следующие команды на локальной машине
```bash
git clone https://github.com/Teru3301/Telegram-VPN.git
cd Telegram-VPN
chmod +x SetUp.sh
./SetUp.sh
```

**В процессе установки потребуется ввести:**
- IP-адрес удалённого сервера
- Имя нового Linux-пользователя
- Кастомный SSH-порт
- Токен Telegram-бота

> **Важно:** Скрипт предназначен для однократной настройки **чистого** сервера.

---

## Использование

После установки всех компонентов, скрипт сгенерирует:
- **SSH-команду** для подключения к серверу
- **Ссылку** для быстрого доступа к 3x-ui панели

Для добавления администратора Telegram-бота:
1. Отредактируйте файл `data/admins.json`
2. Укажите правило определиения является ли пользователь администратором: 
- `or` - у пользователя совпадает username или id с указанными в admins.json (полезно если неизвестен один из параметров) 
- `and` - у пользователя совпадают все параметры
3. Укажите имя пользователя и/или его id.
4. Перезапустите контейнер для применения изменений:
```bash
docker compose restart bot
```

На данный момент администратор может только создавать новые промокоды.

---

## Переменные окружения

`TG_BOT_TOKEN` — токен Telegram-бота

`MONGO_URI` — строка подключения к MongoDB

`XUI_URL` - адрес 3X-UI панели

`IP` - ip-адрес вашего сервера

---

## Ручная сборка и запуск

### Запуск с помошью Docker

Убедитесь что увас установлены и активны:
- docker
- docker-compose

Убедитесь, что файл .env содержит переменные окружения с корректными значениями:
```
TG_BOT_TOKEN=your_telegram_bot_token
MONGO_URI=mongodb://mongo:27017
XUI_URL=127.0.0.1:2053
IP=127.0.0.1
```

В корне репозитория выполните:
```bash
docker compose up -d
```
Контейнеры будут автоматически созданы и запущены в фоновом режиме

### Сборка бинарного файла

Убедитесь что увас установлены:
- cmake
- ninja
- Компилятор C++ (gcc/clang)
- mongo-c-driver
- mongo-cxx-driver 4.0.0
- libtgbot-cpp
- MongoDB
- 3X-UI

```bash
mkdir -p Bot/buld
cd Bot/build
cmake ..
cmake --build .
```

запуск бинарника

```bash
# экспортируйте переменные окружения с вашими значениями
export TG_BOT_TOKEN="12345:your_telegram_bot_token"
export MONGO_URI="mongodb://127.0.0.1:27017"
export XUI_URL="127.0.0.1:2053"
export IP="127.0.0.1"
# запустите бота
./bot
```

---

## Поддержка проекта

**Если этот проект полезен для вас, вы можете поставить ему** :star:

---

## Баги и предложения

Если вы нашли баг или у вас есть предложение по улучшению проекта, создавайте **[Issue](https://github.com/Teru3301/Telegram-VPN/issues)** на GitHub.

---

## TODO

### Core
- [ ] Реализация Telegram-бота
- [x] Интеграция с 3x-ui API
- [x] Создание docker образа

### Payments
- [ ] Интеграция с платёжной системой
- [x] Система промокодов

---

> [!WARNING]
> Проект в активной разработке. API и функционал могут изменяться.

