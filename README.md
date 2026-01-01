
my mac:

cmake -B build -S . -DCMAKE_PREFIX_PATH=${HOME}}$/Qt/5.15.2/macos


on linux with ninja:

cmake -S . -B build -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=${HOME}/Qt/5.15.2/gcc_64



