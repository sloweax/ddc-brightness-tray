# ddc-brightness-tray
System tray application to manage display brightness using DDC, with support for multiple displays

# Dependencies
Make sure your user is added to the i2c group and i2c kernel module is loaded

- make
- pkg-config
- gcc
- qt6-base-devel
- ddcutil-devel

# Build
```sh
git clone https://github.com/sloweax/ddc-brightness-tray
cd ddc-brightness-tray
make
```
