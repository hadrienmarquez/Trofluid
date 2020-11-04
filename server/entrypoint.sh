#!/bin/bash

# Generate ssh keys
ssh-keygen -q -t rsa -N '' <<< ""$'\n'"y"

# Share public key to server through docker volume
cat /root/.ssh/id_rsa.pub >> /usr/src/app/ssh_public/id_rsa.pub