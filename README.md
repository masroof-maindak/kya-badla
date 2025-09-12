# Kya Badla?

Zero-point-five dependency (OpenCV is only used for container classes and to load frames) computer vision project that takes a 'video' as output and outputs a video highlighting 'change' in the original video.

Just about everything in terms of actual classical CV algorithms has been done from scratch, with (to the best of my ability) best modern C++ practices.

## Usage

```bash
git clone https://github.com/masroof-maindak/kya-badla.git
cd kya-badla
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. # Or 'Debug' if you want to hack away.
cmake --build .
./kybdl -i <input-dir> -o <output-dir>
```

## Assumptions

- \[\*\] The 'video' lies as image frames inside the input directory.
- Each image has 3 8-bit channels

## Dependencies

- OpenCV
- QT6
- VTK
- HDF5

#### Arch Linux

```bash
sudo pacman -S opencv qt6-base vtk hdf5
```

## Acknowledgements

- [Convert RGB to Grayscale Algorithms](https://fiveko.com/blog/convert-rgb-to-grayscale-algorithms/)
