LOCALSOURCEPATH="/home/ubuntu/rpi5_yocto/poky/meta-graphicapps-sani/recipes-graphics/mesa"

FILESEXTRAPATHS:prepend = "${LOCALSOURCEPATH}:"

LICENSE = "MIT"

INHERIT += "externalsrc"

SRC_URI:remove = "https://mesa.freedesktop.org/archive/mesa-${PV}.tar.xz "

EXTERNALSRC = "${LOCALSOURCEPATH}/files"

S = "${EXTERNALSRC}"

EXTERNALSRC_BUILD = "${WORKDIR}/build"

LIC_FILES_CHKSUM = "file://docs/license.rst;md5=63779ec98d78d823a9dc533a0735ef10"

do_patch[noexec] = "1"
do_unpack[noexec] = "1"

DEPENDS:append = " meson"
