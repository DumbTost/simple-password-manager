# Description
An simple password manager written in C++ as a side project which aims to provide users with an simple command line interface for managing their passwords easier.

# Libaries used
- Sqlite3 for database management.
- Openssl for encryption
- cpp-httplib to check for updates

# Features that are being developed 
Working on a Qt port

# Known issues
- Spaces being allowed, but filtered
- Size issues with openssl encryption

# LINUX INSTRUCTIONS:
To run this program you need to have these libraries installed
- sqlite3
- openssl
- xcb
  
Command to install them all at once:
### Debian/Ubuntu based systems
`sudo apt-get install libsqlite3-dev libssl-dev xclip -y`
### Red hat/Fedora based system
`sudo dnf install sqlite-devel openssl-devel xclip -y`


# WINDOWS INSTALLATION INSTRUCTIONS:
To run this program, OpenSSL must be installed on your system. Follow these steps for a seamless installation using the Windows Package Manager (winget).

If you don't have winget installed, you can install it from the Microsoft Store by clicking [here](https://www.microsoft.com/store/productId/9NBLGGH4NNS1?ocid=pdpshare).

Alternatively, open your PowerShell and run the following command:

```powershell
winget install firedaemon.openssl
```
