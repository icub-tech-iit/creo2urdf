# creo2urdf
Generate URDF model from CREO mechanisms.

> **Warning**
>
> ⚠️ LIBRARY UNDER DEVELOPMENT ⚠️

**Since it is under development, we cannot guarantee that the user interface of `creo2urdf` will not implement breaking changes. Be aware of this if you start using the code contained in this repository, sorry for the unconvenience.**


**Presently, this plugin supports only the creation of the urdf of the ergocub head**

 <img src="https://github.com/icub-tech-iit/creo2urdf/assets/19152494/19866e6c-8f52-4010-84ef-db458189b753" width=600 align=center >


## Assumptions & limitations

- The assembly has to be the entire ergoCub 1.1 simulation model.
- The joints are in 0.

## Installation

### Dependencies
Right now the `creo2urdf` plugin needs its dependencies to be compiled and linked **statically**:
- Download [vcpkg](https://github.com/microsoft/vcpkg): `git clone https://github.com/microsoft/vcpkg`
- Bootstrap vcpkg: `.\vcpkg\bootstrap-vcpkg.bat`
- Run `[path to vcpkg]/vcpkg install --triplet x64-windows-static-md eigen3 libxml2 assimp idyntree[core,assimp]`

### Build `creo2urdf`

- Pass `-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static-md` to the creo2urdf compilation.


For those who use the CMake integration in Visual Studio, the `-DCMAKE_TOOLCHAIN_FILE` option should not be passed to `CMake command arguments`. Instead, the `vcpkg.cmake` file path must be passed in `CMake toolchain file`.


## Usage

Put in your CREO working directory the `protk.dat` that is automatically generated by CMake in `${PROJECT_BINARY_DIR}` (e.g. `C:\Users\ngenesio\icub-tech-iit\creo2urdf\build\x64-Release`).
Or create it by hand as follow:

```
name creo2urdf
Startup dll
Allow_stop True
Delay_start False
exec_file /path/to/creo2urdf.dll
text_dir /path/to/creo2urdf/src/creo2urdf/text
END
```


### Maintainers
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/Nicogene.png" width="40">](https://github.com/Nicogene) | [@Nicogene](https://github.com/Nicogene) |
| [<img src="https://github.com/mfussi66.png" width="40">](https://github.com/mfussi66) | [@mfussi66](https://github.com/mfussi66) |
