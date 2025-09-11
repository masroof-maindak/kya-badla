# Change Detector

## Usage

```bash
git clone https://github.com/masroof-maindak/change-detector.git
cd change-detector
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. # Or 'Debug' if you want to hack away.
cmake --build .
./demo -i <input-dir> -o <output-dir>
```

## Dependencies

- OpenCV
- QT6
- VTK
- HDF5

#### Arch Linux

```bash
sudo pacman -S opencv qt6-base vtk hdf5
```

<!-- ## Acknowledgements -->
<!---->
<!-- -  -->
