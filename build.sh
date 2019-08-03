#!/bin/bash

./generate.sh && cd build; xcodebuild -project muzik.xcodeproj

exit $?
