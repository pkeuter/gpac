#!/bin/sh
#set -e

source_path=.

function rewrite_deps {
dylib=$1
# echo rewriting deps for $1
for ref in `otool -L $1 | grep '/local' | awk '{print $1'}`
do
# echo changing $ref to @executable_path/lib/`basename $ref` $1
  install_name_tool -change $ref @executable_path/lib/`basename $ref` $1
  copy_lib $ref
done

}

function copy_lib {
# echo testing $1 for bundle copy
basefile=`basename $1`
if [ ! $basefile == 'libgpac.dylib' ] &&  [ ! -e lib/$basefile ];
then
#  echo copying $1 to bundle
  cp $1 lib/
  rewrite_deps lib/$basefile
fi
}

#copy all libs
echo Copying binaries
mkdir tmpdmg
mkdir tmpdmg/Osmo4.app
rsync -r --exclude=.svn $source_path/build/osxdmg/Osmo4.app/ ./tmpdmg/Osmo4.app/
ln -s /Applications ./tmpdmg/Applications
cp $source_path/README ./tmpdmg
cp $source_path/COPYING ./tmpdmg

cp bin/gcc/gm* tmpdmg/Osmo4.app/Contents/MacOS/modules
cp bin/gcc/libgpac.dylib tmpdmg/Osmo4.app/Contents/MacOS/lib
cp bin/gcc/MP4Client tmpdmg/Osmo4.app/Contents/MacOS/Osmo4
cp bin/gcc/MP4Box tmpdmg/Osmo4.app/Contents/MacOS/MP4Box

cd tmpdmg/Osmo4.app/Contents/MacOS/

#check all external deps, and copy them
echo rewriting DYLIB dependencies
for dylib in lib/*.dylib modules/*.dylib
do
  rewrite_deps $dylib
done

echo rewriting APPS dependencies
install_name_tool -change /usr/local/lib/libgpac.dylib @executable_path/lib/libgpac.dylib Osmo4
install_name_tool -change /usr/local/lib/libgpac.dylib @executable_path/lib/libgpac.dylib MP4Box
install_name_tool -change ../bin/gcc/libgpac.dylib @executable_path/lib/libgpac.dylib Osmo4
install_name_tool -change ../bin/gcc/libgpac.dylib @executable_path/lib/libgpac.dylib MP4Box


cd ../../../..

echo Copying GUI
rsync -r --exclude=.svn $source_path/gui ./tmpdmg/Osmo4.app/Contents/MacOS/

echo Building DMG
version=`grep '#define GPAC_VERSION ' $source_path/include/gpac/tools.h | cut -d '"' -f 2`

cur_dir=`pwd`
cd $source_path
rev=`svn info | grep Revision | tr -d 'Revison: '`
cd $cur_dir


#create dmg
hdiutil create ./gpac.dmg -volname "GPAC for OSX"  -srcfolder tmpdmg -ov
rm -rf ./tmpdmg

#add SLA
echo "Adding licence"
hdiutil convert -format UDCO -o gpac_sla.dmg gpac.dmg
rm gpac.dmg
hdiutil unflatten gpac_sla.dmg
/Developer/Tools/Rez /Developer/Headers/FlatCarbon/*.r $source_path/build/osxdmg/SLA.r -a -o gpac_sla.dmg
hdiutil flatten gpac_sla.dmg
hdiutil internet-enable -yes gpac_sla.dmg

echo "GPAC-$version-r$rev.dmg ready"
mv gpac_sla.dmg GPAC-$version-r$rev.dmg
