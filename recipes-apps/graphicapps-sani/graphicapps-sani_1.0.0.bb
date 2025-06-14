SUMMARY = "${PN} file recipe"
LICENSE = "CLOSED"

SRC_URI = "file://source"

CORE_IMAGE_EXTRA_INSTALL:append = " wayland weston xwayland     \
                                    glfw-dev libegl-mesa-dev    \
                                    "

EXTRA_IMAGE_FEATURES:append = " tools-sdk dev-pkgs x11 vulkan"

# list of runtime dependencies
RDEPENDS:${PN}:append = "vulkan-loader      \
                        vulkan-tools        \
                        vulkan-headers      \
                        spirv-tools         \
                        mesa                \
                        glfw                \
                        wayland             \
                        weston              \
                        xrandr              \
                        libxi"

# list of buildtime dependencies
DEPENDS:append = "vulkan-loader             \
                  vulkan-tools              \
                  vulkan-headers            \
                  spirv-tools               \
                  glfw                      \
                  glm                       \
                  xrandr                    \
                  wayland                   \
                  weston                    \
                  xrandr                    \
                  libxi                     \
                  "


TARGET_CC_ARCH:append = " -L${STAGING_LIBDIR} -lvulkan -lglfw -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lm"

SOURCE_DIR = "${WORKDIR}/source"

do_compile[depends] += " glfw:do_populate_sysroot vulkan-loader:do_populate_sysroot"
do_compile () {
    cd ${SOURCE_DIR}
    ln -sf ${RECIPE_SYSROOT}/usr/lib/libglfw.so.3 ${RECIPE_SYSROOT}/usr/lib/libglfw.so || true
    ln -sf ${RECIPE_SYSROOT}/usr/lib/libvulkan.so.1 ${RECIPE_SYSROOT}/usr/lib/libvulkan.so || true

    make clean

    make \
    CFLAGS=" ${CFLAGS} -I${STAGING_INCDIR} -I${SOURCE_DIR}/include" \
    LDFLAGS=" ${LDFLAGS}"
}
addtask compile

FILES:${PN}:append = " /home/root/graphicapps-source"
INSANE_SKIP:${PN}:append = " already-stripped installed-vs-shipped"

do_install () {
    install -d ${D}${bindir}
    install -m 0755 ${SOURCE_DIR}/graphicapps-sani ${D}${bindir}/

    install -d ${D}/home/root/graphicapps-source
    cp -r ${SOURCE_DIR}/* ${D}/home/root/graphicapps-source/
}
addtask install after do_compile
