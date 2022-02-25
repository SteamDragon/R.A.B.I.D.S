# R.A.B.I.D.S
Discord Bot for Your Cyberpunk Red Open Table Campain in Foundry VTT

## Current Version : 0.2.3
NOTE: For proper working of all functions needed to grab number of users from FVTT (changes to source needed)
# Dependencies:
* [Boost](https://www.boost.org/) (Needed to be installed)
* [OpenSSL](https://www.openssl.org/) (Needed to be installed)
* [nlohmann::json](https://github.com/nlohmann/json) (Downloading on build)
* [SleepyDiscord](https://github.com/yourWaifu/sleepy-discord) (Downloading on build)
* [nlohmann_json::nlohmann_json](https://github.com/nlohmann/json) (Downloading on build)
* [fmt](https://github.com/fmtlib/fmt) (Downloading on build)
* [sqlite3](https://www.sqlite.org/) (Already included)

# Build Linux
```sh
git clone https://github.com/SteamDragon/R.A.B.I.D.S
mkdir out
cd out/
cmake ..
make
```

# Changelog:
## 0.2.3

1. When server is stopped no restart will be performed
   
## 0.2.2

1. Changed Registration function to Slash command one
2. Small Cleanup
   
## 0.2.1

1. Moved all string to the config file
2. Added auto generating date for the game

## 0.2.0

1. Added Boost::Log logging
2. Now, if an error occurs during the execution of adding downtime or healing operations, this will not lead to a crash of the entire application.

## 0.1.9

1. Added a library for interacting with SQLite
2. Downtime table display is now beautiful
3. Downtime processing
4. Healing processing
5. Cryotank processing

## 0.1.8

1. Fixed a bug due to which the restart was incorrect
2. Removed the last lines that are shown to the user in the config file (Including the token)
3. Added FMT library, now strings can be beautifully formatted in the console (mostly for myself)
   
## 0.1.7

1. Added configuration files and the ability to operate on them
2. Updated the saving of the technical restart time (Now it is saved in the config when the application is closed)

## 0.1.6

1. Fixed registration errors
   
## 0.1.5

1. First test version
2. Fixed server not stopping issues
3. Improved the functionality of displaying the number of players
4. Registration now creates an Actor
5. When registering users, a temporary file is created with a list of actors / users to be created
6. Technical restart of the server by time (Default time is 12 hours)

## 0.1.4

1. Fixed issue with writing to database
2. Added download support

## 0.1.3

1. Added support for registration and password change
2. Added support for starting/stopping and restarting the server
3. Added Help Message
4. Optimized status update. Only the unique status is sent
