echo "###########################################"
rm -rf build
mkdir build
cd build

cmake ..

make

cd ..
rm -rf dist
mkdir dist
cp build/ask_server* dist
cp -r certs dist
cp /usr/local/bin/cygmicrohttpd-10.dll dist
