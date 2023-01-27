# Bash script to setup the ClearPath SDK and drivers
# NOTE: requires g++ tar cmake make ldconfig

# Add the user to the correct group for USB permissions
sudo usermod -aG dialout "$(whoami)"

# Install Linux headers
sudo apt install linux-headers-"$(uname -r)" -y

# Extract the tar package
rm -rf clearpath_hub
mkdir clearpath_hub
tar -xf teknic_clearpath_sc_sdk.tar.gz --directory ./clearpath_hub/
cd clearpath_hub || exit
tar -xvf sFoundation.tar
cd sFoundation || exit
echo "Done inflating archives, building project"

# Build the source code and install it system wide
make
sudo cp {MNuserDriver20.xml,libsFoundation20.so} /usr/local/lib
sudo ldconfig
echo "Done installing libraries, starting driver install script"

# Inflate set up the Clearpath drivers
cd ..
tar -xvf Teknic_SC4Hub_USB_Driver.tar
cd ExarKernelDriver || exit
sudo chmod +777 Install_DRV_SCRIPT.sh
echo "Done setting up driver directory. Note that hub must be plugged in to continue!"

# Run the provided setup script
sudo ./Install_DRV_SCRIPT.sh

exit 0

