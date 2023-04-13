# creo2urdf
Generate URDF model from CREO mechanisms.

## Installation

### Dependencies
Right now `creo2urdf` plugin needs that the dependencies are compiled and linked **statically**:
- Download [vcpkg](https://github.com/microsoft/vcpkg): `git clone https://github.com/microsoft/vcpkg`
- Bootstrap vcpkg: `.\vcpkg\bootstrap-vcpkg.bat`
- Run `[path to vcpkg]/vcpkg install --triplet x64-windows-static-md eigen3 libxml2 assimp`
- Compile idyntree from source following the instructions in https://github.com/robotology/idyntree/blob/master/doc/build-from-source.md, remembering to pass `-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static-md` and `-DBUILD_SHARED_LIBS:BOOL=OFF` to ensure that iDynTree is compiled as static.
- Install iDynTree

### Build `creo2urdf`

- Pass `-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static-md` to the creo2urdf compilation, remembering also to pass the iDynTree's installaton location via `CMAKE_PREFIX_PATH`.

For who uses CMake integrate in Visual Studio `-DCMAKE_TOOLCHAIN_FILE` option has not to be passed to `CMake command arguments` but in the `vcpkg.cmake` file path has to be set in `CMake toolchain file`


### Maintainers
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/Nicogene.png" width="40">](https://github.com/Nicogene) | [@Nicogene](https://github.com/Nicogene) |
| [<img src="https://github.com/mfussi66.png" width="40">](https://github.com/mfussi66) | [@mfussi66](https://github.com/mfussi66) |
