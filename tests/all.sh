#!/bin/bash
#
# Execute all tests, in sequence.

set -e

BASEDIR=`dirname $0`

source ${BASEDIR}/helpers.sh

${BASEDIR}/lex.sh
echo

${BASEDIR}/ast.sh
echo

${BASEDIR}/suite
