#!/usr/bin/env bash

set -eu
set -o pipefail

MASON_VERSION="master"

function setup_mason() {
    mkdir -p ./.mason
    curl -sSfL https://github.com/mapbox/mason/archive/v0.10.0.tar.gz | tar --gunzip --extract --strip-components=1 --exclude="*md" --exclude="test*" --directory=./.mason
    export PATH=$(pwd)/.mason:$PATH
}

function init_binary() {
    mason install ${1} ${2}
    export PATH=$(mason prefix ${1} ${2})/bin:${PATH}

}

function main() {
    setup_mason
    if [[ $(uname -s) == 'Linux' ]]; then
        init_binary clang 3.9.1
        export CXX=clang++
        export CC=clang
    fi
    if [[ ${NODE_ASAN:-} == 'true' ]]; then
        init_binary node_asan 4.4.5
    fi
}

main

# set back to non-strict bash mode to avoid breaking travis itself
set +eu
set +o pipefail

