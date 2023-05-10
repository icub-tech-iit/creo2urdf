# creo2urdf
Generate URDF model from CREO mechanisms.


⚠️ LIBRARY UNDER DEVELOPMENT ⚠️

**Since it is under development, we cannot guarantee that the user interface of `creo2urdf` will not implement breaking changes. Be aware of this if you start using the code contained in this repository, sorry for the unconvenience.**


**Right now this plugin support only the simple case of an assembly with two links and one joints**

 <img src="https://github.com/icub-tech-iit/creo2urdf/assets/19152494/19866e6c-8f52-4010-84ef-db458189b753" width=600 align=center >
 <img src="https://github.com/icub-tech-iit/creo2urdf/assets/19152494/968a544f-d991-423f-9b4f-752d0db9dd45" width=600 align=center >

## Installation

### Dependencies
Right now `creo2urdf` plugin needs that the dependencies are compiled and linked **statically**:
- Download [vcpkg](https://github.com/microsoft/vcpkg): `git clone https://github.com/microsoft/vcpkg`
- Bootstrap vcpkg: `.\vcpkg\bootstrap-vcpkg.bat`
- Run `[path to vcpkg]/vcpkg install --triplet x64-windows-static-md eigen3 libxml2 assimp idyntree[core,assimp]`

### Build `creo2urdf`

- Pass `-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static-md` to the creo2urdf compilation.

For who uses CMake integrate in Visual Studio `-DCMAKE_TOOLCHAIN_FILE` option has not to be passed to `CMake command arguments` but in the `vcpkg.cmake` file path has to be set in `CMake toolchain file`


### Maintainers
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/Nicogene.png" width="40">](https://github.com/Nicogene) | [@Nicogene](https://github.com/Nicogene) |
| [<img src="https://github.com/mfussi66.png" width="40">](https://github.com/mfussi66) | [@mfussi66](https://github.com/mfussi66) |
