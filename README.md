
# Qt-Solitaire

Solitaire in C++ using Qt.


## Building with qmake (CMake is recommended)

```
mkdir build
cd build
qmake ../qtsolitaire.pro
make
```

## Examples on how to build with cmake


### General approach

```
cmake -B build -S . -GNinja
cd build; ninja
```


### Notes for specific machines

my mac:

```
cmake -B build -S . -DCMAKE_PREFIX_PATH=${HOME}}$/Qt/5.15.2/macos
```


on linux with ninja:

```
cmake -S . -B build -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=${HOME}/Qt/5.15.2/gcc_64
```




