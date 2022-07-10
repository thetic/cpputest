#!/usr/bin/env bash

set -o nounset
set -o errexit
set -o pipefail

# Find the config file next to this script.
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
CONFIG=$SCRIPT_DIR/dosbox-0.74.conf

WORKINGDIR=$(mktemp -d)

# Put the executable into the working directory with a valid DOS name.
PROGRAM=$1
ln -s "$PROGRAM" "$WORKINGDIR/PROG.EXE"

# Create a dummy file for DOSBox to run.
# All the real work happens in the -c commands.
DUMMY=$WORKINGDIR/file
printf "" >"$DUMMY"

# Capture the program's output
LOG=$WORKINGDIR/LOG.TXT
# shellcheck disable=2064
trap "cat $LOG; rm -rf $WORKINGDIR" exit

DRIVE=e

env SDL_VIDEODRIVER=dummy \
    dosbox \
    -conf "$CONFIG" \
    -noautoexec \
    "$DUMMY" \
    -c "mount $DRIVE $WORKINGDIR" \
    -c "$DRIVE:" \
    -c "PROG.EXE ${*:2} >> LOG.TXT" \
    -noconsole \
    -exit \
    1>&2

# We *should* be reflecting PROG.EXE's return code, but that's essentially
# impossible with DOSBox. This is a hack that fails if the output looks like a
# test failure. This will not catch errors from other calls (eg test
# discovery).
if grep -q '^Errors (' "$LOG"; then
    exit 1
fi
