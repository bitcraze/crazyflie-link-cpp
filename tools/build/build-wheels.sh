#!/bin/bash
set -ex

PYVERSION=${1?"Error: expected the python version"}

# Prefer running with podman
DOCKER=docker
if which podman
then
    DOCKER=podman
fi

# Test if we are currently in the builder image
if [ ! -d /opt/python ]
then
    # We are not in the image, start the image with the build script
    $DOCKER run --rm -v $(realpath $(dirname $0)/../..):/io quay.io/pypa/manylinux2014_x86_64 /io/tools/build/build-wheels.sh $PYVERSION
else
    # We are in the image, building!
    PYVERCL=${PYVERSION//./}
    # Bins located in paths similar to /opt/python/cp38-cp38/bin
    PYBIN="/opt/python/cp${PYVERCL}-cp${PYVERCL}/bin"
    echo "Using python path $PYBIN"

    yum install -y libusbx-devel

    cd /io

    PATH=${PYBIN}:$PATH
    "${PYBIN}/pip" install -U "setuptools>=42" wheel ninja "cmake>=3.12"
    "${PYBIN}/python" setup.py bdist_wheel

    WHEELS=$(echo dist/*.whl)

    for whl in $WHEELS; do
        auditwheel repair "$whl" -w dist/
    done

    rm $WHEELS
fi
