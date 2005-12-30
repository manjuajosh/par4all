#! /bin/sh
#
# $Id$
#
# Setup a basic pips installation from scratch
#

# where to get pips
SVN_CRI='http://svn.cri.ensmp.fr/svn'
PIPS_SVN=$SVN_CRI/pips

#POLYLIB_SITE='http://icps.u-strasbg.fr/polylib/polylib_src'
POLYLIB_SITE='http://www.cri.ensmp.fr/pips'
POLYLIB='polylib-5.22.1'

# help
command=${0/*\//}
usage="usage: $command [destination-directory [developer]]"

# arguments
destination=${1:-`pwd`/MYPIPS}
developer=${2:-$USERNAME} 

error()
{
    echo "$@" >&2
    exit 1
}

test -d $destination && error "directory $destination already exists"
mkdir $destination || error "cannot mkdir $destination"

prod=$destination/prod

echo "### checking needed softwares"
for exe in svn wget tar gunzip make cproto flex bison gcc perl sed tr
do
  type $exe || error "no such executable, please install: $exe"
done

echo "### downloading pips"
svn checkout $PIPS_SVN/bundles/trunks $prod || error "cannot checkout pips"
PIPS_ARCH=`$prod/pips/makes/arch.sh`
export PIPS_ARCH

# this should fail if not a developer
echo "### getting user development branches"
svn checkout $PIPS_SVN/branches/$developer $destination/pips_dev
#svn checkout $LINEAR_SVN/branches/$developer $destination/linear_dev
#svn checkout $NEWGEN_SVN/branches/$developer $destination/newgen_dev

echo "### downloading $POLYLIB"
cd /tmp
test -f $POLYLIB.tar.gz && error "some $POLYLIB.tar.gz file already there"
wget $POLYLIB_SITE/$POLYLIB.tar.gz || error "cannot wget polylib"

echo "### building $POLYLIB"
gunzip $POLYLIB.tar.gz || error "cannot decompress polylib"
tar xf $POLYLIB.tar || error "cannot untar polylib"
cd $POLYLIB || error "cannot cd into polylib"
./configure --prefix=$prod/extern || error "cannot configure polylib"
make || error "cannot make polylib"

mkdir $prod/extern || error "cannot mkdir $prod/extern"
make install || error "cannot install polylib"
cd .. || error "cannot cd .."
rm -rf $POLYLIB || error "cannot remove polylib"
rm -f $POLYLIB.tar || error "cannot remove polylib tar"

echo "### fixing $POLYLIB"
mkdir $prod/extern/lib/$PIPS_ARCH || error "cannot mkdir"
cd $prod/extern/lib/$PIPS_ARCH || error "cannot cd"
ln -s ../libpolylib*.a libpolylib.a || error "cannot create links"

echo "### testing special commands for config.mk"
config=$prod/pips/makes/config.mk
type javac && echo '_HAS_JDK_ = 1' >> $config
type latex && echo '_HAS_LATEX_ = 1' >> $config
type emacs && echo '_HAS_EMACS_ = 1' >> $config
# others? copy config to newgen and linear?

echo "### building newgen"
cd $prod/newgen
make compile

echo "### building linear"
cd $prod/linear
make compile

echo "### building pips"
cd $prod/pips
make compile

echo "### creating pipsrc.sh"
cat <<EOF > $destination/pipsrc.sh
# minimum rc file for sh-compatible shells

# default architecture
export PIPS_ARCH=$PIPS_ARCH

# subversion repositories
export NEWGEN_SVN=$SVN_CRI/newgen
export LINEAR_SVN=$SVN_CRI/linear
export PIPS_SVN=$SVN_CRI/pips

# software roots
export EXTERN_ROOT=$prod/extern
export NEWGEN_ROOT=$prod/newgen
export LINEAR_ROOT=$prod/linear
export PIPS_ROOT=$prod/pips

# path
PATH=\$PIPS_ROOT/bin:\$PIPS_ROOT/utils:\$PATH
EOF

echo "### generating csh environment"
$prod/pips/utils/sh2csh.pl < $destination/pipsrc.sh > $destination/pipsrc.csh

echo "### checking useful softwares"
for exe in bash m4 wish
do
  type $exe || echo "no such executable, consider installing: $exe"
done
