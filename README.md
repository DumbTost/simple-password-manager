# Description
An simple password manager written in C++ as a side project which aims to provide users with an simple command line interface for managing their passwords easier.

# Libaries used
- Sqlite3 for database management.
- Openssl for encryption
- ClipboardXX to put passwords in the clipboard

# Features that are being developed 
- More commands
- Customability


# LINUX INSTRUCTIONS:
To run this program you need to have these libraries installed
- sqlite3
- openssl
- xcb

Command to install them all at once:
### Debian/Ubuntu based systems
`sudo apt-get install libsqlite3-dev libssl-dev libxcb1-dev -y`
### Red hat/Fedora based system
`sudo dnf install sqlite-devel openssl-devel libxcb-devel -y`
