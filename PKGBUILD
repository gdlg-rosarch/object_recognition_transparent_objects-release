# Script generated with Bloom
pkgdesc="ROS - A technique to recognize and estimate poses of transparent objects"
url='http://wg-perception.github.io/transparent_objects'

pkgname='ros-kinetic-object-recognition-transparent-objects'
pkgver='0.4.3_1'
pkgrel=1
arch=('any')
license=('BSD'
)

makedepends=('proj'
'ros-kinetic-catkin'
'ros-kinetic-ecto'
'ros-kinetic-object-recognition-core'
'ros-kinetic-pcl-ros'
)

depends=('proj'
'ros-kinetic-ecto'
'ros-kinetic-ecto-opencv'
'ros-kinetic-object-recognition-core'
'ros-kinetic-pcl-ros'
)

conflicts=()
replaces=()

_dir=object_recognition_transparent_objects
source=()
md5sums=()

prepare() {
    cp -R $startdir/object_recognition_transparent_objects $srcdir/object_recognition_transparent_objects
}

build() {
  # Use ROS environment variables
  source /usr/share/ros-build-tools/clear-ros-env.sh
  [ -f /opt/ros/kinetic/setup.bash ] && source /opt/ros/kinetic/setup.bash

  # Create build directory
  [ -d ${srcdir}/build ] || mkdir ${srcdir}/build
  cd ${srcdir}/build

  # Fix Python2/Python3 conflicts
  /usr/share/ros-build-tools/fix-python-scripts.sh -v 2 ${srcdir}/${_dir}

  # Build project
  cmake ${srcdir}/${_dir} \
        -DCMAKE_BUILD_TYPE=Release \
        -DCATKIN_BUILD_BINARY_PACKAGE=ON \
        -DCMAKE_INSTALL_PREFIX=/opt/ros/kinetic \
        -DPYTHON_EXECUTABLE=/usr/bin/python2 \
        -DPYTHON_INCLUDE_DIR=/usr/include/python2.7 \
        -DPYTHON_LIBRARY=/usr/lib/libpython2.7.so \
        -DPYTHON_BASENAME=-python2.7 \
        -DSETUPTOOLS_DEB_LAYOUT=OFF
  make
}

package() {
  cd "${srcdir}/build"
  make DESTDIR="${pkgdir}/" install
}

