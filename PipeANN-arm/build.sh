export ADDITIONAL_DEFINITIONS="-DDYN_PIPE_WIDTH"

rm -rf build
mkdir build
cd build
cmake ..
make -j
