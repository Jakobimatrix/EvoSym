clangpath=
if [ -L "/usr/bin/clang" ] 
then
    export CC=/usr/bin/clang
    export CXX=/usr/bin/clang++
else
    echo "Did not find a clang installation. Using default compiler."
fi
cmake -DCMAKE_BUILD_TYPE=Release ..
make
