#!/usr/bin/env bash

echo "Welcome to TU/e Pynq platform"
echo -e "\a"
echo "Board Info:"
echo -e "\x0e"

echo "IPs:"
echo -e "\x0e"
hostname -I | while read A
do
	echo "* ${A}"
done
echo -e "\x0f"

echo "HW "$(../read-version/main)
echo "SW version: "$(git rev-parse --short HEAD)
echo -e "\x0f"
echo -e "\a"
echo "Distro:"
echo -e "\x0e"
echo "id:      "$(lsb_release  -i -s)
echo "version: "$(lsb_release  -r -s)
echo -e "\x0f"
echo -e "\a"
