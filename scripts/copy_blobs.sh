FILES="
vendor/bin/hw/android.hardware.gatekeeper@1.0-service-qti:recovery/root/sbin/android.hardware.gatekeeper@1.0-service-qti
vendor/bin/hw/android.hardware.keymaster@3.0-service-qti:recovery/root/sbin/android.hardware.keymaster@3.0-service-qti
vendor/bin/qseecomd:recovery/root/sbin/qseecomd
vendor/lib64/libtime_genoff.so:recovery/root/vendor/lib64/libtime_genoff.so
vendor/lib64/libStDrvInt.so:recovery/root/vendor/lib64/libStDrvInt.so
vendor/lib64/libsecureui.so:recovery/root/vendor/lib64/libsecureui.so
vendor/lib64/libGPreqcancel_svc.so:recovery/root/vendor/lib64/libGPreqcancel_svc.so
vendor/lib64/libkeymasterprovision.so:recovery/root/vendor/lib64/libkeymasterprovision.so
vendor/lib64/libGPreqcancel.so:recovery/root/vendor/lib64/libGPreqcancel.so
vendor/lib64/libkeymasterdeviceutils.so:recovery/root/vendor/lib64/libkeymasterdeviceutils.so
vendor/lib64/libQSEEComAPI.so:recovery/root/vendor/lib64/libQSEEComAPI.so
vendor/lib64/libsecureui_svcsock.so:recovery/root/vendor/lib64/libsecureui_svcsock.so
vendor/lib64/libdrmtime.so:recovery/root/vendor/lib64/libdrmtime.so
vendor/lib64/librpmb.so:recovery/root/vendor/lib64/librpmb.so
vendor/lib64/libdrmfs.so:recovery/root/vendor/lib64/libdrmfs.so
vendor/lib64/hw/android.hardware.gatekeeper@1.0-impl-qti.so:recovery/root/vendor/lib64/hw/android.hardware.gatekeeper@1.0-impl-qti.so
vendor/lib64/hw/gatekeeper.sdm660.so:recovery/root/vendor/lib64/hw/gatekeeper.sdm660.so
vendor/lib64/hw/keystore.sdm660.so:recovery/root/vendor/lib64/hw/keystore.sdm660.so
vendor/lib64/hw/android.hardware.keymaster@3.0-impl-qti.so:recovery/root/vendor/lib64/hw/android.hardware.keymaster@3.0-impl-qti.so
vendor/lib64/libssd.so:recovery/root/vendor/lib64/libssd.so
vendor/lib64/libSecureUILib.so:recovery/root/vendor/lib64/libSecureUILib.so
vendor/lib64/libqisl.so:recovery/root/vendor/lib64/libqisl.so
vendor/lib64/libdiag.so:recovery/root/vendor/lib64/libdiag.so
vendor/lib64/libkeymasterutils.so:recovery/root/vendor/lib64/libkeymasterutils.so
";
work=/sdcard/tmp/device_tree;
rm -rf $work;
mkdir -p $work;
cd /;
for i in $FILES; do
rootpath=${i%:*}
devicepath=${i#*:}
if [ -f "/$rootpath" ]; then
mkdir -p "$work/`dirname $devicepath`";
echo "/$rootpath"
cp -f "/$rootpath" "$work/$devicepath";
fi;
done;
