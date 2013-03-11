#!/bin/sh

mkdir deploy
make DESTDIR=deploy install
scp -r -P 2223 -i $HOME/.ssh/mer-qt-creator-rsa deploy/* root@localhost:/
rm -rf deploy
ssh -p 2223 -i $HOME/.ssh/mer-qt-creator-rsa root@localhost "DISPLAY=:0 /usr/bin/invoker --type=j -s /opt/sdk/bin/documents"
