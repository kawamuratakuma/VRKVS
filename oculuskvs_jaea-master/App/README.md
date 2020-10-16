# OculusKVS_JAEA/App

## How to append new App program.

###  Make directory

```
> mkdir NewApp
> cd NewApp
```

### Copy conf files for KVSMake and nmake

```
> copy ../HUDTest01/kvsmake*.conf .

```

### make source & header files

### run KVSMake

```
> kvsmake -G 
```

### build using build_app.bat

```
> cd ..
> build_app.bat NewApp
```