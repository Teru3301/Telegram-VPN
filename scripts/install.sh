#!/bin/bash

curl -fsSL https://raw.githubusercontent.com/Teru3301/Telegram-VPN/main/docker-compose.yml -o docker-compose.yml
curl -fsSL https://raw.githubusercontent.com/Teru3301/Telegram-VPN/main/scripts/3x-setup.sh -o 3x-setup.sh
chmod +x 3x-setup.sh

ENV_FILE=".env"
MONGO_INIT_FILE="vpn-bot/mongo/mongo-init.js"

# --- function ---
ask_var() {
    local var_name=$1
    local default_value=$2
    read -p "$var_name [$default_value]: " value
    echo "${value:-$default_value}"
}

# --- input ---
TG_BOT_TOKEN=$(ask_var "TG_BOT_TOKEN" "your_telegram_bot_token")
MONGO_USERNAME=$(ask_var "MONGO_USERNAME" "vpnbot")
MONGO_PASSWORD=$(ask_var "MONGO_PASSWORD" "botpass")
MONGO_DB=$(ask_var "MONGO_DB" "tg-vpn-db")
IP=$(ask_var "IP" "127.0.0.1")
XUI_PORT=$(ask_var "XUI_PORT" "2053")
XUI_PATH=$(ask_var "XUI_PATH" "/3x")
XUI_LOGIN=$(ask_var "XUI_LOGIN" "xui_admin")
XUI_PASSWORD=$(ask_var "XUI_PASSWORD" "admin_password")
XUI_URL="172.17.0.1:$XUI_PORT$XUI_PATH"
XUI_URL="${XUI_URL%/}"

# --- .env ---
cat > "$ENV_FILE" <<EOL
TZ=Europe/Moscow
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

# --- mongo init ---
mkdir -p vpn-bot/mongo
mkdir -p vpn-bot/bot
cat > "$MONGO_INIT_FILE" <<EOL
db = db.getSiblingDB('$MONGO_DB');
db.createUser({
  user: "$MONGO_USERNAME",
  pwd: "$MONGO_PASSWORD",
  roles: [{ role: "readWrite", db: "$MONGO_DB" }]
});
EOL

echo "mongo-init.js file created."

# --- run ---
echo "Starting Docker containers..."
docker compose up -d

echo "Configure 3x-ui..."
sleep 2
./3x-setup.sh "172.17.0.1:2053" "admin" "admin" "$XUI_PORT" "$XUI_PATH" "$XUI_LOGIN" "$XUI_PASSWORD"

echo "Setup complete!"
echo "Edit \"./vpn-bot/bot/config.json\" and restart bot container \"docker compose restart vpn-bot\""

