[![Build Status](https://travis-ci.org/arcana261/uvchan.svg?branch=master)](https://travis-ci.org/arcana261/uvchan)
[![Coverage Status](https://coveralls.io/repos/github/arcana261/uvchan/badge.svg?branch=master)](https://coveralls.io/github/arcana261/uvchan?branch=master)
[![Build status](https://ci.appveyor.com/api/projects/status/bxbuw5928rcuqjd3/branch/master?svg=true)](https://ci.appveyor.com/project/arcana261/uvchan/branch/master)

# shootersteind

backend for a multipler shooter game we decided to build on hackethon!


## setup development environment

```bash
apt-get update
apt-get install -y git autoconf build-essential libboost-all-dev
apt-get install -y python-pip clang-format lcov doxygen
pip install cpplint
git clone https://github.com/arcana261/shootersteind-backend.git
cd shootersteind-backend
./autogen.sh
./configure
make check
make sanity
```
