SRCREV = "${AUTOREV}"

CFLAGS:append = " -fno-asynchronous-unwind-tables"
ASFLAGS:append = " -Wa,--noexecstack"

python do_unpack:append() {
    bb.note("Injecting line-ending fix function")
    bb.build.exec_func('fix_crlf_line_endings', d)
}

fix_crlf_line_endings() {
for f in $(find ${S} -type f); do
sed -i 's/\r$//' "$f" || true
done
}