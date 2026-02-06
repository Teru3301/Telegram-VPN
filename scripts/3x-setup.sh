#!/bin/sh

# --- Получаем параметры ---
OLD_ADDR="${1}"
OLD_USER="${2}"
OLD_PASS="${3}"
NEW_PORT="${4}"
NEW_PATH="${5}"
NEW_USER="${6}"
NEW_PASS="${7}"

# --- Функция для интерактивного ввода с дефолтами ---
prompt() {
    VAR_NAME="$1"
    DEFAULT="$2"
    read -p "Enter $VAR_NAME [$DEFAULT]: " INPUT
    echo "${INPUT:-$DEFAULT}"
}

# --- Если какие-то параметры не переданы, спрашиваем у пользователя ---
[ -z "$OLD_ADDR" ] && OLD_ADDR=$(prompt "OLD_ADDR" "http://172.17.0.1:2053")
[ -z "$OLD_USER" ] && OLD_USER=$(prompt "OLD_USER" "admin")
[ -z "$OLD_PASS" ] && OLD_PASS=$(prompt "OLD_PASS" "admin")
[ -z "$NEW_PORT" ] && NEW_PORT=$(prompt "NEW_PORT" "2053")
[ -z "$NEW_PATH" ] && NEW_PATH=$(prompt "NEW_PATH" "/")
[ -z "$NEW_USER" ] && NEW_USER=$(prompt "NEW_USER" "$OLD_USER")
[ -z "$NEW_PASS" ] && NEW_PASS=$(prompt "NEW_PASS" "$OLD_PASS")

# --- parse OLD_ADDR ---
OLD_ADDR_NO_SCHEME=$(echo "$OLD_ADDR" | sed -E 's#^https?://##')
HOSTPORT=$(echo "$OLD_ADDR_NO_SCHEME" | cut -d/ -f1)
HOST=$(echo "$HOSTPORT" | cut -d: -f1)
PORT=$(echo "$HOSTPORT" | cut -d: -f2)

# Формируем путь (без завершающего /)
URL_PATH="/$(echo "$OLD_ADDR_NO_SCHEME" | cut -s -d/ -f2-)"
[ "$URL_PATH" = "/" ] && URL_PATH=""   # если путь пустой, оставляем пустым

OLD_ADDR="http://$HOST:$PORT$URL_PATH"
NEW_ADDR="http://$HOST:$NEW_PORT$NEW_PATH"

# Убираем завершающий / у OLD_BASE и NEW_BASE, если он есть
OLD_ADDR="${OLD_ADDR%/}"
NEW_ADDR="${NEW_ADDR%/}"

echo "OLD_ADDR = $OLD_ADDR"
echo "NEW_ADDR = $NEW_ADDR"

COOKIE="/tmp/3xui_cookie.txt"


# --- Ожидание запуска 3x-ui контейнера ---
echo "[*] Waiting for 3x-ui container to be ready..."

MAX_RETRIES=30   # максимум попыток
SLEEP_TIME=2     # пауза между попытками в секундах
RETRY=0

while ! curl -s --connect-timeout 1 "$OLD_ADDR" >/dev/null 2>&1; do
    RETRY=$((RETRY+1))
    if [ "$RETRY" -ge "$MAX_RETRIES" ]; then
        echo "[!] Timeout waiting for 3x-ui container"
        exit 1
    fi
    echo "[*] 3x-ui not ready yet, retrying in $SLEEP_TIME seconds..."
    sleep $SLEEP_TIME
done

echo "[✓] 3x-ui is responding!"


# --- Login на старый адрес ---
echo "[*] Login to OLD_ADDR..."
RESP=$(curl -s -c "$COOKIE" \
    -H "Content-Type: application/json" \
    -X POST "$OLD_ADDR/login" \
    -d "{\"username\":\"$OLD_USER\",\"password\":\"$OLD_PASS\"}")

echo "$RESP" | grep -q '"success":true' || {
    echo "[!] Login failed $RESP"
    exit 1
}
echo "[✓] Login success"

# --- Update login/password ---
echo "[*] Update login/password..."
curl -s -b "$COOKIE" \
    -H "Content-Type: application/x-www-form-urlencoded; charset=UTF-8" \
    -H "X-Requested-With: XMLHttpRequest" \
    -X POST "$OLD_ADDR/panel/setting/updateUser" \
    --data-urlencode "id=1" \
    --data-urlencode "username=$OLD_USER" \
    --data-urlencode "password=$OLD_PASS" \
    --data-urlencode "loginSecret=" \
    --data-urlencode "oldUsername=$OLD_USER" \
    --data-urlencode "oldPassword=$OLD_PASS" \
    --data-urlencode "newUsername=$NEW_USER" \
    --data-urlencode "newPassword=$NEW_PASS" \
    | grep -q success || {
        echo "[!] User update failed"
        exit 1
    }

echo "[✓] Login/password saved"

# --- Restart panel ---
echo "[*] Restart panel..."
curl -s -b "$COOKIE" -X POST "$NEW_ADDR/panel/setting/restartPanel" >/dev/null 2>&1
echo "[✓] Done"

# --- Login на старый адрес ---
echo "[*] Login to OLD_ADDR..."
RESP=$(curl -s -c "$COOKIE" \
    -H "Content-Type: application/json" \
    -X POST "$OLD_ADDR/login" \
    -d "{\"username\":\"$NEW_USER\",\"password\":\"$NEW_PASS\"}")

echo "$RESP" | grep -q '"success":true' || {
    echo "[!] Login failed $RESP"
    exit 1
}
echo "[✓] Login success"

# --- Update settings ---
echo "[*] Update settings..."
RESP=$(curl -s -b "$COOKIE" \
    -H "Content-Type: application/x-www-form-urlencoded; charset=UTF-8" \
    -H "X-Requested-With: XMLHttpRequest" \
    -X POST "$OLD_ADDR/panel/setting/update" \
    --data-urlencode "webPort=$NEW_PORT" \
    --data-urlencode "webBasePath=$NEW_PATH" \
    --data-urlencode "sessionMaxAge=60" \
    --data-urlencode "pageSize=50" \
    --data-urlencode "tgBotEnable=false" \
    --data-urlencode "tgBotLoginNotify=true" \
    --data-urlencode "tgLang=en-US" \
    --data-urlencode "subEnable=false" \
    --data-urlencode "subPort=2096" \
    --data-urlencode "subPath=/sub/" \
    --data-urlencode "subJsonPath=/json/" \
    --data-urlencode "subEncrypt=true" \
    --data-urlencode "subShowInfo=true" \
    --data-urlencode "timeLocation=Europe/Moscow")

echo "$RESP" | grep -q '"success":true' || {
    echo "[!] Update settings failed $RESP"
    exit 1
}

echo "[✓] Settings saved"

# --- Restart panel ---
echo "[*] Restart panel..."
curl -s -b "$COOKIE" -X POST "$OLD_ADDR/panel/setting/restartPanel" >/dev/null 2>&1
echo "[✓] Done"

docker container restart 3x-ui
docker container restart vpn-bot

