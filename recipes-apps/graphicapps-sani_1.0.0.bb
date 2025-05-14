SUMMARY = "${PN} file recipe"
LICENSE = "CLOSED"

SRC_URI = "file://source \
           "

# list of runtime dependencies
RDEPENDS:${PN}:append = " "

# list of buildtime dependencies
DEPENDS:append = " "

S = "${WORKDIR}/source"

TARGET_CXX_ARCH += "${LDFLAGS}"

do_compile () {
    make clean
    make all
}
addtask compile

do_install () {
    install -d ${D}${bindir}
    install -m 0755 ${B}/graphicapps-sani ${D}${bindir}/
}
addtask install after do_compile
