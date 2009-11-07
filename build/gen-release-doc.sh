#!/bin/bash

# Versions.
VER="0.4"
REL="r89"

# Where to find the files.
WIKIDOCROOT=http://bdremote-ng.googlecode.com/svn-history/$REL/wiki

# Wget stuff.
WGET=`type -p wget`
WGET_OPTIONS="-q"
D=`pwd`

# Utility used to convert google wiki files from SVN into html.
# Slightly broken, needs patch in doc/tools.
CONVERT="ruby -I ~/remote-svn/google-wiki-syntax ~/remote-svn/google-wiki-syntax/wiki_convertor.rb"

# Delete/create directory.
rm -fr bdremoteng-$VER-doc
mkdir -p bdremoteng-$VER-doc

GetDoc()
{
    echo "Getting document $1.wiki .. "
    $WGET $WGET_OPTIONS $WIKIDOCROOT/$1.wiki -O $1

    return 0
}

ConvertDoc()
{
    echo "Converting document $1 to html .. " && \
    $CONVERT $1 .

    return 0
}

# echo "Getting $WIKIDOCROOT/README.wiki .." 
 
DOCS="README Releases initscripts relproc"

cd bdremoteng-$VER-doc && \
for DOC in `echo $DOCS`
do
    GetDoc $DOC
done 

echo "Downloading done .."

for DOC in `echo $DOCS`
do
    ConvertDoc $DOC
done 

echo "Conversion done .."

for DOC in `echo $DOCS`
do
    rm -f $DOC
done 

echo "Generating doxygen .. "

cd ../../doc/ && \
rm -fr html && \
doxygen && \
cd ../build/bdremoteng-$VER-doc && \
cp -r ../../doc/html doxygen && \
echo "Generated doxygen .."

cd .. && tar -cvjf bdremoteng-$VER-doc.tar.bz2 bdremoteng-$VER-doc

exit 0


