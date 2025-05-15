FILESEXTRAPATHS:prepend = " ${THISDIR}/files:"
SRC_URI += " file://weston_window_setup.env"

RDEPENDS:${PN}:append = " bash"

FILES:${PN}:append = " /home/root/weston_window_setup.env"

do_install:append() {
    install -d ${D}/home/root/
    install -m 0775 ${WORKDIR}/weston_window_setup.env ${D}/home/root/
}
