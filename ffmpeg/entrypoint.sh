#!/bin/bash

# Adding ssh to service list on 'default' runlevel
rc-update add sshd default

# As we did not boot with openrc, necessary steps to launch sshd service
rc-status
mkdir -p /run/openrc
touch /run/openrc/softlevel

# Start the ssh server
/etc/init.d/sshd start

# Copy ssh public key from other container
mkdir /home/ffmpeg/.ssh
cat /usr/local/ssh_public/id_rsa.pub >> /home/ffmpeg/.ssh/authorized_keys
rm  /usr/local/ssh_public/id_rsa.pub 

# Change ownership to ffmpeg user
chown -R ffmpeg /home/ffmpeg

# Start bash to keep it running
bash