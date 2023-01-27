# Script to setup a device for building

# Install apt packages
echo "Installing required packages"
sudo apt update -y
sudo apt upgrade -y
sudo apt dist-upgrade -y
sudo apt install g++ tar cmake make ldconfig net-tools can-utils htop -y
sudo apt update
echo "Done!"

# Run the setup script for the ClearPath SDK
sudo ./install_clearpath_drivers.sh

echo "Environment setup complete!"

exit 0;