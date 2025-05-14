SUMMARY = "${PN} file recipe"
LICENSE = "CLOSED"

SRC_URI = "file://source \
           "

EXTRA_IMAGE_FEATURES:append = "dev-pkgs"

# list of runtime dependencies
RDEPENDS:${PN}:append = "                   \
                        vulkan-loader       \
                        vulkan-tools        \
                        vulkan-headers      \
                        vulkan-loader-dev   \
                        vulkan-loader-dev   \
                        glfw                \
                        glfw-dev            \
                        glm                 \
                        libxi"

# list of buildtime dependencies
DEPENDS:append = " vulkan-loader vulkan-tools vulkan-headers make glfw glm libxi "

TARGET_CXX_ARCH:append = "${LDFLAGS}"

SOURCE_DIR = "${WORKDIR}/source"

do_compile () {
    cd ${SOURCE_DIR}

    ln -sf ${RECIPE_SYSROOT}/usr/lib/libglfw.so.3 ${RECIPE_SYSROOT}/usr/lib/libglfw.so || true
    ln -sf ${RECIPE_SYSROOT}/usr/lib/libvulkan.so.1 ${RECIPE_SYSROOT}/usr/lib/libvulkan.so || true

    make clean
    make \
    CPPFLAGS="${CPPFLAGS} -I${STAGING_INCDIR}" \
    LDFLAGS="${LDFLAGS} -L${STAGING_LIBDIR} \
     -lvulkan -lglfw -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lm"
}
addtask compile

FILES:${PN}:append = "/home/root/graphicapps-source"
INSANE_SKIP:${PN}:append = "build-deps already-stripped installed-vs-shipped"

do_install () {
    install -d ${D}${bindir}
    install -m 0755 ${SOURCE_DIR}/graphicapps-sani ${D}${bindir}/

    install -d ${D}/home/root/graphicapps-source
    cp -r ${SOURCE_DIR}/* ${D}/home/root/graphicapps-source/
}
addtask install after do_compile
