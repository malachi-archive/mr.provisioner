#!/bin/bash

DOCKER_IMAGE=malachib/esp-open-rtos
DOCKER_TAG=latest-mal-lwip

ext/useful-scripts/embedded/esp-open-rtos/docker-env.sh $DOCKER_IMAGE:$DOCKER_TAG
