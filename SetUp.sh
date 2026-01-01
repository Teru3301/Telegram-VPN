#!/bin/bash


read -p "VDS server ip              : " SERVER_IP
read -p "Write username for new user: " NEW_USER_NAME
read -p "Write custom SSH port      : " CUSTOM_PORT
echo ""


echo "Check SSH keys..."
if [ ! -f ~/.ssh/id_rsa ] || [ ! -f ~/.ssh/id_rsa.pub ]; then
    echo "Generating SSH keys..." &&
    ssh-keygen -t rsa -b 4096 -f ~/.ssh/id_rsa -N "" -q
else
    echo "SSH keys exist"
fi
PUBLIC_KEY=$(cat ~/.ssh/id_rsa.pub)

echo ""
echo "------------------------------------------------"
echo ""
echo "Try to connect $SERVER_IP..."
echo ""

ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null root@$SERVER_IP "
    export DEBIAN_FRONTEND=noninteractive
    export PUBLIC_KEY='$PUBLIC_KEY'
    export NEW_USER_NAME='$NEW_USER_NAME'
    export CUSTOM_PORT='$CUSTOM_PORT'

    echo 'Connection successful!'
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Setting up...'
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Update system...'
    echo ''
    echo 'Update package lists...'
    apt update -y > /dev/null 2>&1
    echo 'Upgrade packages...'
    apt upgrade -y -o Dpkg::Options::=\"--force-confdef\" -o Dpkg::Options::=\"--force-confold\" > /dev/null 2>&1
    echo 'Cleaning...'
    apt autoremove -y > /dev/null 2>&1
    echo ''
    echo 'Done!'
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Create new user...'
    echo ''
    useradd -m -s /bin/bash \"\$NEW_USER_NAME\"
    usermod -aG sudo \"\$NEW_USER_NAME\"
    echo 'User '\"\$NEW_USER_NAME\"' created!'
    echo ''
    echo 'Configure sudo without password...'
    echo \"\$NEW_USER_NAME ALL=(ALL:ALL) NOPASSWD: ALL\" > /etc/sudoers.d/\$NEW_USER_NAME
    chmod 440 /etc/sudoers.d/\$NEW_USER_NAME
    echo 'Sudo NOPASSWD configured!'
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Setting up SSH for new user...'
    echo ''
    mkdir -p /home/\$NEW_USER_NAME/.ssh
    echo \"\$PUBLIC_KEY\" > /home/\$NEW_USER_NAME/.ssh/authorized_keys
    chown -R \$NEW_USER_NAME:\$NEW_USER_NAME /home/\$NEW_USER_NAME/.ssh
    chmod 700 /home/\$NEW_USER_NAME/.ssh
    chmod 600 /home/\$NEW_USER_NAME/.ssh/authorized_keys
    echo 'User SSH setup complete!'
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Configuring SSH daemon...'
    echo ''
    systemctl stop ssh.socket 2>/dev/null
    systemctl disable ssh.socket 2>/dev/null
    # Используем CUSTOM_PORT вместо фиксированного 3301
    sed -i '/^#Port\|^Port/d' /etc/ssh/sshd_config
    echo \"Port \$CUSTOM_PORT\" >> /etc/ssh/sshd_config
    sed -i '/^#*PasswordAuthentication\|^PasswordAuthentication/d' /etc/ssh/sshd_config
    sed -i '/^#*PermitRootLogin\|^PermitRootLogin/d' /etc/ssh/sshd_config
    sed -i '/^#*PubkeyAuthentication\|^PubkeyAuthentication/d' /etc/ssh/sshd_config
    echo 'PasswordAuthentication no' >> /etc/ssh/sshd_config
    echo 'PermitRootLogin no' >> /etc/ssh/sshd_config
    echo 'PubkeyAuthentication yes' >> /etc/ssh/sshd_config
    systemctl restart ssh
    echo 'SSH configuration complete!'
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Install packages...'
    apt install -y git btop tree lsd neovim hyfetch duf
    echo 'Packages installed successfully!'
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Install docker...'
    wget https://raw.githubusercontent.com/docker/docker-install/master/install.sh -O /tmp/install.sh &&
    chmod +x /tmp/install.sh &&
    /tmp/install.sh > /dev/null 2>&1 &&
    rm -f /tmp/install.sh
    echo 'Docker installed successfully!'
    echo ''
    echo 'Add user to docker group...'
    usermod -aG docker \"\$NEW_USER_NAME\"
    echo 'User added to docker group!'
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Installing 3x-ui...'
    echo ''
    echo 'Create docker-compose file...'
    mkdir -p /opt/3xui/
    cat > /opt/3xui/docker-compose.yml << 'EOF'
services:
  3x-ui:
    image: metaligh/3x-ui:latest
    container_name: 3x-ui
    hostname: 3x-ui-panel
    volumes:
      - ./db/:/etc/x-ui/
      - /etc/letsencrypt/:/etc/letsencrypt/:rw
    environment:
      XRAY_VMESS_AEAD_FORCED: \"false\"
    tty: true
    network_mode: host
    restart: unless-stopped
EOF
    echo 'Deploy 3x-ui...'
    cd /opt/3xui &&
    docker compose up -d &&
    sleep 3 &&
    docker ps | grep 3x-ui
    echo ''
    echo '3x-ui deployed successfully!'
"


if [ $? -eq 0 ]; then
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Setup complete!'
    echo ''
    echo '**IMPORTANT**: For docker permissions to take effect,'
    echo 'you need to either:'
    echo '1. Re-login to the server:'
    echo "   ssh -p $CUSTOM_PORT $NEW_USER_NAME@$SERVER_IP"
    echo '2. Or run this command on the server:'
    echo "   sudo newgrp docker"
    echo ''
    echo '------------------------------------------------'
    echo ''
else
    echo 'Connection error!'
    exit 1
fi


echo ''
echo '------------------------------------------------'
echo ''
echo 'Connect to server:'
echo "ssh -p $CUSTOM_PORT $NEW_USER_NAME@$SERVER_IP"
echo ''
echo '3x-ui panel:'
echo "http://$SERVER_IP:2053"
echo ''
echo 'Docker commands (after re-login or newgrp):'
echo "ssh -p $CUSTOM_PORT $NEW_USER_NAME@$SERVER_IP 'docker ps'"
echo "ssh -p $CUSTOM_PORT $NEW_USER_NAME@$SERVER_IP 'docker logs 3x-ui'"
echo ''

