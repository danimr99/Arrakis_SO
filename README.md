
# Arrakis System

Final project of the subject "Operative Systems" that consists on the development of:

<details>
  <summary>Fremen</summary>
  <p>
    It is a program that takes on the role of client.
    It offers a terminal where users can run Linux and other special commands 
    that will allow them to connect and disconnect to the Atreides server, 
    search for users by zip code and transfer and download images. 
    In addition, the images that have been downloaded from Atreides are removed from 
    time to time.
  </p>
</details>

<details>
  <summary>Atreides</summary>
  <p>
    It is a program that takes on the role of server.
    It allows the connection of several Fremen processes. Atreides is responsible 
    for maintaining a historical record of users who have logged in as well as 
    storing images sent by Fremen processes.
  </p>
</details>

<details>
  <summary>Harkonen</summary>
  <p>
    It is a program that takes on the role of attacker.
    Its goal is to scan processes for Fremen processes in order to terminate them. 
    This malicious action is executed once every certain period of time and the 
    terminated Fremen process is random among all the Fremen processes that it has
    been able to find.
  </p>
</details>

## Installation

Clone this repository

```bash
  git clone https://github.com/danimr99/Arrakis_SO
  cd Arrakis_SO
```

Compile all using the makefile

```bash
  make
```

To start Atreides:

| Type |Option| Description | Example |
|------|------|-------------|---------|
| String | configuration_file | Path of the Atreides configuration file | atreides_config.txt |

```bash
  ./atreides configuration_file
```

To start Fremen:

| Type | Option| Description | Example |
|------|-------|-------------|---------|
| String | configuration_file | Path of the Fremen configuration file | fremen_config.dat |

```bash
  ./fremen configuration_file
```

To start Harkonen:

| Type |Option| Description | Example |
|------|------|-------------|---------|
| Integer | time_period | Time period between each Fremen process terminated | 60

```bash
  ./harkonen time_period
```
**NOTE: Images to be transferred from Fremen to Atreides must be placed 
on the specified directory from the Fremen configuration file.**

## Authors

- [@danimr99](https://www.github.com/danimr99)
- [@Luelmo14](https://www.github.com/Luelmo14)

