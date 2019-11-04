
# Compile

## Prerequisites

- Qt 5.11 with following modules
    - core
    - qml
    - quick
    - widgets
    - bluetooth

### Linux

Easy install using `apt-get` (works at least in [Ubuntu](https://ubuntu.com/#download) 19.04 and [Rasbian](https://www.raspberrypi.org/downloads/raspbian/) Buster):
```sh
apt-get install qt5-default qtdeclarative5-dev qtconnectivity5-dev \
    qml-module-qtquick2 qml-module-qtquick-controls2 \
    qml-module-qtquick-controls qml-module-qt-labs-settings qml-module-qtgraphicaleffects
```

### Windows and macOS

Unfortunately I have no access to either of these since switching fully to Linux.

But as Qt is cross-platform framework, there should be no problem compiling the app by using [Qt Creator](https://www.qt.io/download-qt-installer) free version.

### Android

This app will be available in [Play Store](https://play.google.com/store) later. Can be compiled using [Qt Creator](https://www.qt.io/download-qt-installer) free version.

# API

Multimeter API is designed to be *"telnet-compatible"*. What this means, is that if you have a TCP connectable multimeter device
in your network, you can simply connect using `telnet <host> <port>` to it and control the device by typing.

## Data exchange basics

1. All data **must** be encoded in [UTF-8](https://en.wikipedia.org/wiki/UTF-8)
1. Every *"packet"* is simply a line
    * A line **must** end with line feed (`\n`)
    * That line feed **may** be prefixed with carriage return (`\r`) which **must** be ignored
1. Line starting with **a capital letter** belongs to a channel which that letter is assigned to
1. Line starting with hash (`#`) **must** be ignored
1. Line parsed as invalid or not recognised must be ignored without errors

### Channel

##### ID

Channel ID is a single character upper case ASCII letter from **A** to **Z**. This means that every device can have 26 channels.

#### Configuration

Every channel configuration line is as follows:
```
<ID>:<key[,options]>:<value>[\r]\n
```

##### Keys

- name
- type
- mode = **r** / **w** / **rw**
- method = **push** or **pull**


### Device

##### Ask configuration
```
get config[\r]\n
```

Configuration is requested simply using this format.

##### Receive configuration
```
<id>:<key[,options]>:<value>[\r]\n
```

Every configuration line is split into sections by first two colons.
All other colons after first two are considered to belong in the value.

First section is **id**, which can be one of these:

- `device`: device specific configuration
- `<channel>`: Channel specific configuration for this device

##### Set value
```
<channel>=<value>[\r]\n
```

##### Ask value
```
<channel>[\r]\n
```
