#!/bin/bash

# Get the process owner passed as argumenter to the script
process_owner=$1

# Get all PIDs of Fremen processes executed by the process owner
echo `ps -u ${process_owner} | awk '$4=="fremen" {print $1}'`