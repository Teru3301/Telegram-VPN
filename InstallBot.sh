#!/bin/bash

# Файл .env
ENV_FILE=".env"
MONGO_INIT_FILE="mongo-init.js"

echo "=== Telegram-VPN Setup Script ==="

# Функция для запроса значения с дефолтом
ask_var() {
    local var_name=$1
    local default_value=$2
    read -p "$var_name [$default_value]: " value
    if [ -z "$value" ]; then
        value=$default_value
    fi
    echo $value
}

# Запрашиваем значения
TG_BOT_TOKEN=$(ask_var "TG_BOT_TOKEN" "your_telegram_bot_token")
MONGO_USERNAME=$(ask_var "MONGO_USERNAME" "vpnbot")
MONGO_PASSWORD=$(ask_var "MONGO_PASSWORD" "botpass")
MONGO_DB=$(ask_var "MONGO_DB" "tg-vpn-db")
IP=$(ask_var "IP" "127.0.0.1")
XUI_URL=$(ask_var "XUI_URL" "172.17.0.1:2053")
XUI_LOGIN=$(ask_var "XUI_LOGIN" "admin")
XUI_PASSWORD=$(ask_var "XUI_PASSWORD" "admin")

# Создаем .env
cat > $ENV_FILE <<EOL
TG_BOT_TOKEN=$TG_BOT_TOKEN
MONGO_USERNAME=$MONGO_USERNAME
MONGO_PASSWORD=$MONGO_PASSWORD
MONGO_DB=$MONGO_DB
MONGO_URI=mongodb://${MONGO_USERNAME}:${MONGO_PASSWORD}@mongo:27017/${MONGO_DB}?authSource=admin
IP=$IP
XUI_URL=$XUI_URL
XUI_LOGIN=$XUI_LOGIN
XUI_PASSWORD=$XUI_PASSWORD
EOL

echo ".env file created."

# Создаем mongo-init.js с жесткими значениями
cat > $MONGO_INIT_FILE <<EOL
db = db.getSiblingDB('$MONGO_DB');
db.createUser({
  user: "$MONGO_USERNAME",
  pwd: "$MONGO_PASSWORD",
  roles: [{ role: "readWrite", db: "$MONGO_DB" }]
});
EOL

echo "mongo-init.js file created."

# Запуск docker compose
echo "Starting Docker containers..."
docker compose up -d

echo "Setup complete!"

