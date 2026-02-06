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
    echo 'Upgrade packages... (it will take some time)'
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
"


if [ $? -eq 0 ]; then
    echo 'Setup complete!'
    echo ''
    echo '------------------------------------------------'
    echo ''
    echo 'Connect to server:'
    echo "ssh -p $CUSTOM_PORT $NEW_USER_NAME@$SERVER_IP"
else
    echo 'Connection error!'
    exit 1
fi

