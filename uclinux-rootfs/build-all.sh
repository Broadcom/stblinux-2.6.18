echo "============================================="
echo "time make distclean"
echo "============================================="
time make distclean

echo "======================================================================="
echo "time make -f build.mk TFTPDIR=/tftpboot/2618-1.0_4 &> 2618-1.0_4-LE.log"
echo "======================================================================="
time make -f build.mk TFTPDIR=/tftpboot/2618-1.0_4 &> 2618-1.0_4-LE.log

echo "============================================="
echo "time make distclean"
echo "============================================="
time make distclean

echo "============================================================================="
echo "time make -f build-be.mk TFTPDIR=/tftpboot/2618-1.0_4_be &> 2618-1.0_4-BE.log"
echo "============================================================================="
time make -f build-be.mk TFTPDIR=/tftpboot/2618-1.0_4_be  &> 2618-1.0_4-BE.log

