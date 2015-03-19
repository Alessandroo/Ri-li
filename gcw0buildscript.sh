#! /bin/sh

#./configure --host=mipsel-linux --prefix=/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/ CXXFLAGS=-DGCWZERO;
#make;
mkdir gcw0-release;
cp src/Ri_li gcw0-release;
cp data/* gcw0-release;
mkdir gcw0-release/Sounds;
cp Sounds/* gcw0-release/Sounds;
cp src/default.gcw0.desktop gcw0-release;

mksquashfs gcw0-release/ Ri-li.opk -all-root -noappend -no-exports -no-xattrs;
echo "And as if by magic, an opk appears :)";

