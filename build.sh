mkdir -p build/ && 
cmake . -B build/ &&
cd build && make || echo "BUILDING FAILED"
read -p "$*"