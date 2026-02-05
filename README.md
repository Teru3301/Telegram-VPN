# Telegram VPN Service

Безопасный VPN-сервис с доступом через Telegram-бота.

<img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=ffffff"/> <img src="https://img.shields.io/badge/cmake-064F8C?style=for-the-badge&logo=cmake&logoColor=ffffff"/> <img src="https://img.shields.io/badge/mongodb-47A248?style=for-the-badge&logo=mongodb&logoColor=ffffff"/> <img src="https://img.shields.io/badge/bash-4EAA25?style=for-the-badge&logo=gnubash&logoColor=ffffff"/> <img src="https://img.shields.io/badge/telegram-26A5E4?style=for-the-badge&logo=Telegram&logoColor=ffffff"/> <img src="https://img.shields.io/badge/docker-2496ED?style=for-the-badge&logo=docker&logoColor=ffffff"/> <img src="https://img.shields.io/badge/git-F05032?style=for-the-badge&logo=git&logoColor=ffffff"/>

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
- **Telegram Bot Token** (получить у [@BotFather](https://t.me/botfather))

---

## Установка

Выполните следующие команды на удалённом сервере
```bash
curl -fsSL https://raw.githubusercontent.com/Teru3301/Telegram-VPN/main/install.sh -o install.sh
chmod +x install.sh
./install.sh
```

---

## Переменные окружения

`TZ` - часовой пояс сервера (по умолчанию Europe/Moscow)

`TG_BOT_TOKEN` - токен вашего Telegram-бота

`MONGO_USERNAME` - имя пользователя для подключения к MongoDB (по умолчанию vpnbot)

`MONGO_PASSWORD` - пароль пользователя MongoDB (по умолчанию botpass)

`MONGO_DB` - название базы данных для бота (по умолчанию tg-vpn-db)

`MONGO_URI` - строка подключения к MongoDB (формируется автоматически)

`IP` - IP-адрес вашего сервера (по умолчанию 127.0.0.1)

`XUI_URL` - адрес веб-панели 3X-UI (по умолчанию 172.17.0.1:2053)

`XUI_LOGIN` - логин для входа в панель 3X-UI (по умолчанию admin)

`XUI_PASSWORD` - пароль для входа в панель 3X-UI (по умолчанию admin)

---

Для добавления администратора Telegram-бота:
1. Отредактируйте файл `vpn-bot/bot/admins.json`
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

## Поддержка проекта

**Если этот проект полезен для вас, вы можете поставить ему** :star:

---

## Баги и предложения

Если вы нашли баг или у вас есть предложение по улучшению проекта, создавайте **[Issue](https://github.com/Teru3301/Telegram-VPN/issues)** на GitHub.

---

## TODO

### Core
- [x] Реализация Telegram-бота
- [x] Интеграция с 3x-ui API
- [x] Создание docker образа

### Payments
- [ ] Интеграция с платёжной системой
- [x] Система промокодов

### Others
- [ ] Мануалы по использованию бота на разных платформах
- [ ] Отдельны скрипт для настройки сервера
- [x] Отдельны скрипт для установики контейнетов 

---

> [!WARNING]
> Проект в активной разработке. API и функционал могут изменяться.

