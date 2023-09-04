#!/bin/sh
# this script is called by the "make release" target in the Makefile
#
RELEASE_APPLICATIONS=$*
if [ "$RELEASE_APPLICATIONS" = "" ] ; then
  echo "this script should be called by the "make release" target in the Makefile"
  exit 1
fi

RELEASE=`grep '#define LIBPYNQ_RELEASE' library/version.c | sed s'!^[^"]*"\([^"][^"]*\)"$!\1!g'`
MAJOR=`grep '#define LIBPYNQ_VERSION_MAJOR' library/version.c | sed s'![^0-9]!!g'`
MINOR=`grep '#define LIBPYNQ_VERSION_MINOR' library/version.c | sed s'![^0-9]!!g'`
PATCH=`grep '#define LIBPYNQ_VERSION_PATCH' library/version.c | sed s'![^0-9]!!g'`
DATE=`date "+%Y-%m-%d %H:%M"`
RELEASE_BASENAME="libpynq-${RELEASE}-v${MAJOR}.${MINOR}.${PATCH}"
LABEL="release ${RELEASE} version ${MAJOR}.${MINOR}.${PATCH} of ${DATE}"

echo ===
echo === making release $RELEASE_BASENAME
echo ===
if [ -d ../$RELEASE_BASENAME ] || [ -f ../$RELEASE_BASENAME.zip ] ; then
  echo "won't overwrite $RELEASE_BASENAME"
  exit 1
fi
if [ ! -d .git ] || [ ! -d documentation ] ; then
  echo "already made this directory into a release?"
  exit 1
fi
if [ ! -f external/scpi-parser/README.md ] ; then
  echo "scpi-parser is missing; add it with: git submodule update --init"
  exit 1
fi
read -p "do you want to clean non-git files (yes for real release; no while testing) [yn]? " cleanit
[ "$cleanit" = "y" ] && git clean -dxf
make clean
# copy everything over
cp -r `pwd` ../$RELEASE_BASENAME
grep 'url =' .git/config | sed 's/.*url = //' > ../$RELEASE_BASENAME/tmp
(
  cd ../$RELEASE_BASENAME
  echo $LABEL > RELEASE
  git show --raw | head -1 >> RELEASE
  cat tmp >> RELEASE
  rm -r tmp
  echo making documentation - this takes some time, do not interrupt
  make doc >/dev/null 2>&1 
  mv build/latex/refman.pdf build
  rm -r build/latex
  mv build doc
  [ "$cleanit" = "y" ] && rm -rf applications/* .git* documentation README.md
)
if [ "$cleanit" = "y" ] ; then
  for a in $RELEASE_APPLICATIONS ; do
    cp -r applications/$a ../$RELEASE_BASENAME/applications
  done
fi
(
  cd ..
  zip -rq $RELEASE_BASENAME.zip $RELEASE_BASENAME
  rm -rf $RELEASE_BASENAME
)
echo ===
echo === made release $RELEASE_BASENAME in file ../$RELEASE_BASENAME.zip
echo === and you can now test it
echo ===
echo === IMPORTANT: unzip on PYNQ with: unzip -DD -q $RELEASE_BASENAME.zip
# this avoid problems with creation/modification dates in the future if the PYNQ clock is wrong
[ "$cleanit" = "n" ] && exit 0
echo ===
echo === to complete the release it must be tagged and distributed
echo === can the master branch be checked out to create a tag [yn]? 
echo === if this release is for testing only, type ^C or answer n
echo === if this release is for distribution then
echo ===    there should be no outstanding changes to be committed to the master branch
echo '===    (i.e. wait until the merge request has been commmitted on the master branch)'
echo ===
read -p "can the master branch be checked out to create a tag [yn]? " ok
[ "$ok" = "y" ] || exit 0

if git checkout master ; then
  echo checked out master branch
  # get up to date before tagging
  git pull
else
  echo FAIL: cannot checkout master branch
  exit 1
fi
if git tag -a $RELEASE_BASENAME -m "$LABEL" ; then
  echo succesfully created tag $LABEL
  git tag -l libpynq-*
else
  echo FAIL: tag $RELEASE_BASENAME already exists
  git tag -l libpynq-*
  exit 1
fi
echo ===
echo === the tag has not been pushed yet, so if something is wrong it can still be fixed locally
echo === otherwise: git push origin $RELEASE_BASENAME
echo ===
