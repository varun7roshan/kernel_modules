1. create a dts file - which is a overlay dts : "testoverlay.dts"
-----------------------------------------
/dts-v1/;
/plugin/;
/ {
  compatible = "brcm,bcm2835";
  fragment@0{
    target-path = "/";
    __overlay__{
       my_device: my_device{
         compatible = "overlay_Test_module";
         status = "okay";
         label = "Re-test OVERLAY";
         my_value = <100>;
      };
    };
  };
};

-----------------------------------------
2. If the file "testoverlay.dts" is created else where, copy it to kernel source path:
arch/arm/boot/dts/overlays/

3. Compile the dts overlay to dtbo :
/usr/local/bin/dtc -@ -Hepapr -I dts -O dtb -o arch/arm/boot/dts/overlays/testoverlay.dtbo arch/arm/boot/dts/overlays/testoverlay.dts

4. copy the "testoverlay.dtbo" to target : home folder
scp arch/arm/boot/dts/overlays/testoverlay.dtbo pi@192.168.0.110:~/

5. Insert the overlay in target :
sudo dtoverlay testoverlay.dtbo

6. Check if the overlay is inserted into the device tree:
/proc/device-tree/my_device

7. Insert the module
sudo insmod 02_dt_probe_module.ko

----------------------------------------------------------------------------------
- If ran into dtc compilation isssue - for overlays - install the dtc from :
dtc-3.8.x.sh

#git pull git://github.com/RobertCNelson/dtc.git dtc-fixup-65cc4d2
git pull https://github.com/RobertCNelson/dtc.git dtc-fixup-65cc4d2

--------------------------
cat dtc-3.8.x.sh
#!/bin/bash -e

#git_sha="origin/master"
#git_sha="27cdc1b16f86f970c3c049795d4e71ad531cca3d"
#git_sha="fdc7387845420168ee5dd479fbe4391ff93bddab"
git_sha="65cc4d2748a2c2e6f27f1cf39e07a5dbabd80ebf"
project="dtc"
server="git://git.kernel.org/pub/scm/linux/kernel/git/jdl"

if [ ! -f /tmp/git/ ] ; then
        mkdir -p /tmp/git/ || true
fi

git clone ${server}/${project}.git /tmp/git/${project}/

cd /tmp/git/${project}/
make clean
git checkout master -f
git pull || true

git checkout ${git_sha} -b ${git_sha}-build
#git pull git://github.com/RobertCNelson/dtc.git dtc-fixup-65cc4d2
git pull https://github.com/RobertCNelson/dtc.git dtc-fixup-65cc4d2

make clean
make PREFIX=/usr/local/ CC=gcc CROSS_COMPILE= all
echo "Installing into: /usr/local/bin/"
make PREFIX=/usr/local/ install
--------------------------


