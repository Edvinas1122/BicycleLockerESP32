# BicycleLockerESP32

This repository contains a dispenser locker application designed for integration with the BicycleShare project.

## Getting Started

### Prerequisites

Ensure you have the following set up:

- An account on [Pusher](https://pusher.com/) with an associated app key.
- Access to a local network.
- A deployed instance of the [BicycleShare project](https://github.com/Edvinas1122/BicycleShare-FrontEnd) that's accessible from the internet.
- A locker device, an enstructions for it's build comming soon...

### Installation

1. **Clone the Repository**:
   
	Use your favorite method to clone or download the repository to your local machine.

2. **Open the Project in PlatformIO**:
   
	If you don't have it yet, install [PlatformIO](https://platformio.org/) and then open the project.

3. **Set Up Environment Variables**:

   Create a file named `env.txt` in the root of the project and populate it with the following:

   ```txt
   -DLOCAL_SSID="your hotspot name"
   -DLOCAL_PASSWORD="your hotspot password"
   -DLOCAL_USER="your local network username"
   -DPUSHER_KEY="your pusher key"
   -DBICYCLE_API_ENDPOINT="your network domain /pusher/auth"
   -DUPDATE_DB_ENDPOINT="your network domain /api/IoT/locker"
   ```

4. **Generate and Set Up Keys**

	Generate a public-private key pair. Place the public key in the keys/public_key.pem file within the project. Ensure you synchronize the private key with your server.

5. **Build and Upload the Project**

	Build and upload the project to your ESP32 device.
