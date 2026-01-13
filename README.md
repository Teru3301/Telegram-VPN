# Telegram VPN Service

Безопасный VPN-сервис с доступом через Telegram-бота.

<img src="https://img.shields.io/badge/bash-4EAA25?style=for-the-badge&logo=gnubash&logoColor=ffffff"/> <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=ffffff"/> <img src="https://img.shields.io/badge/cmake-064F8C?style=for-the-badge&logo=cmake&logoColor=ffffff"/> <img src="https://img.shields.io/badge/mongodb-47A248?style=for-the-badge&logo=mongodb&logoColor=ffffff"/> <img src="https://img.shields.io/badge/telegram-26A5E4?style=for-the-badge&logo=Telegram&logoColor=ffffff"/> <img src="https://img.shields.io/badge/docker-2496ED?style=for-the-badge&logo=docker&logoColor=ffffff"/> <img src="https://img.shields.io/badge/git-F05032?style=for-the-badge&logo=git&logoColor=ffffff"/>

---

## Особенности

- Быстрая, разовая установка и настройка
- Пользователи сами управляют подпиской на сервис
- Автоматическая базовая харденизация сервера

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
- IP-адрес сервера
- Имя нового пользователя
- Кастомный SSH-порт
- Токен телеграм бота

> **Важно:** Скрипт предназначен для однократной настройки **чистого** сервера.

---

## Использование

После установки скрипт сгенерирует:
- **SSH-команду** для подключения к серверу
- **Ссылку** для быстрого доступа к 3x-ui панели
- **Ссылку** для управления сервисом

---

## TODO

- [ ] Реализация Telegram-бота
- [x] Система промокодов
- [ ] Интергация с 3x-ui
- [ ] Интеграция с банковой системой
- [ ] Создание docker образа

---

> [!WARNING]
> Проект в активной разработке. API и функционал могут изменяться.

