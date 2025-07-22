FILESEXTRAPATHS:prepend = " ${THISDIR}/files:"
SRC_URI += " file://weston_window_setup.env"
SRC_URI += " file://shaders"
SRC_URI += " file://scripts"
SRC_URI += " file://data"

RDEPENDS:${PN}:append = " bash"

FILES:${PN}:append = " /home/root/weston_window_setup.env"

do_install:append() {
    install -d ${D}/home/root/
    install -m 0775 ${WORKDIR}/weston_window_setup.env ${D}/home/root/
}

DEPENDS:append = " glslang-native"

GLSLC = "${WORKDIR}/recipe-sysroot-native${bindir}/glslang"
DATA_DIR = "${WORKDIR}/data"
SHADER_DIR = "${WORKDIR}/shaders"
SCRIPT_DIR = "${WORKDIR}/scripts"

do_install[depends] += " glslang:do_populate_sysroot"

do_install:append() {
    VERTEX_TRGT=$(find ${SHADER_DIR} -name *.vert)
    if [ -n ${VERTEX_TRGT} ]; then
        ${GLSLC} -G ${VERTEX_TRGT} -o $(echo "${VERTEX_TRGT}" | sed 's/.vert/_vert.spv/')
    fi

    FRAGMENT_TRGT=$(find ${SHADER_DIR} -name *.frag)
    if [ -n ${FRAGMENT_TRGT} ]; then
        ${GLSLC} -G ${FRAGMENT_TRGT} -o $(echo "${FRAGMENT_TRGT}" | sed 's/\.frag/_frag\.spv/')
    fi

    install -d ${D}/home/root/graphicapps-source
    install -d ${D}/home/root/graphicapps-source/data
    install -d ${D}/home/root/graphicapps-source/shaders
    install -d ${D}/home/root/graphicapps-source/scripts

    cp -r ${DATA_DIR} ${D}/home/root/graphicapps-source
    cp -r ${SHADER_DIR} ${D}/home/root/graphicapps-source
    cp -r ${SCRIPT_DIR} ${D}/home/root/graphicapps-source
}
