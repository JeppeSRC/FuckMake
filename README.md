# FuckMake

FuckMake is a much simpler thing than make and it wont just suddenly take a dump in your face.

## Example

Example compiling [PowerSupply](https://github.com/JeppeSRC/PowerSupply)

```
!FuckMake

CC = arm-none-eabi-gcc
LD =%(CC)
AS = arm-none-eabi-as

CFILES = GetFiles(src/,*.c,)
ASMFILES = GetFiles(src/,*.asm,)

INC = -Isrc

OutDir = bin/
ObjDir = %(OutDir)obj/

AFLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS = %(AFLAGS) -O3 -Wno-unknown-pragmas -Wno-nonnull-compare -Wall
LFLAGS = -nostartfiles -nodefaultlibs -nostdlib -Wl,--gc-sections --specs=nosys.specs -T"LinkerScript.ld"


Compile {
    Msg(Compiling %Input)
    !%(CC) %(CFLAGS) %(INC) -c -o %Output %Input 
}

Assemble {
    Msg(Assembling %Input)
    !%(AS) %(AFLAGS) %(INC) -c -o %Output %Input
}

Link {
    Msg(Linking %Output)
    !%(LD) %(LFLAGS) -o %Output %Input
}

build:
    ExecuteList(Compile, %(CFILES), %(ObjDir))
    ExecuteList(Assemble, %(ASMFILES), %(ObjDir))
    Execute(Link, GetFiles(%(ObjDir), *.obj,), %(OutDir)MicroCode.elf)

clean:
    DeleteFiles(GetFiles(%(ObjDir),*.obj,))

```

## Some form of explanation

### Beginning of file
`!FuckMake` Marks the start of the FuckFile (it's just there to say FUCK YOU MAKE one more time).

### Variables
`Name = <something>` Defines a variable.

`%(Name)` Is used to get the value of a variable.

#### Builtin Variables

-   `ROOT`

`ROOT`: ROOT specifies the root directory. For example if FuckMake is executed in dev/folder/ but your sources are in dev/, you can set ROOT to ../ which will make the root directory dev/. This is useful because FuckMake really doesn't like /../ in it's paths GGWP. It may be set to whatever you want early in the script, if not it will contain the normal root directory ie dev/folder/.

### Actions

```
Name {
    ....
}
```

Defines an action which can be executed with [Execute](#execute). Actions define how to compile, assemble, link etc.

`%Input` and `%Output` are built in variables that will be different depending on how the action is executed.

`!` Tells FuckMake that it's a command line action.

### Execute

There are two types of execute functions, [ExecuteList](#executelist) and [Execute](#execute)

### Targets

`Name:` Defines a target. All targets must be at the end of the file. 

`__default__` is a reserved target.

### Functions

-   [`GetFiles`](#getfiles)
-   [`DeleteFiles`](#deletefiles)
-   [`Msg`](#msg)
-   [`ExecuteList`](#executelist)
-   [`Execute`](#execute)

### GetFiles

`GetFiles(Directory, Wildcards, Exclusions)` 

-   `Directory` The root directory to start grabbing files in.
-   `Wildcards` You can use this to filter the files included. Example `*.c` will only include files that end with a `.c`. Multiple wildcards can be specified if seperated by a space
-   `Exclusions` Used to exclute files and subdirectories that gets included. Example `print.c` will exclude all files named `print.c` even if it gets included by the wildcard parameter.

All parameters are optional, all files in the current directory will be included if all are left blank.

### DeleteFiles

`DeleteFiles(Files)`

-   `Files` A list of files to be deleted.

### Msg

`Msg(Message)`

-   `Message` A message that will be printed when executed.

### ExecuteList

`ExecuteList(Action, Files, OutDir)`

-   `Action` Specifies the actions to be executed.
-   `Files` A List of files separeted by commas to be used as input files. This will be the contents of `%Input`.
-   `OutDir` Is a path to where the files shall be written. This is the directory that will be in `%Output`.

Executes the action once for every file in the `Files` list. Except if the resulting file was modified after the input file, basically only files that needs to be for instance compiled will.

### Execute

`Execute(Action, Files, OutDir)`

-   `Action` Same as [ExecuteList](#executelist)
-   `Files` Same as [ExecuteList](#executelist)
-   `OutDir` Same as [ExecuteList](#executelist)

Same as [ExecuteList](#executelist) except that it's only executed once and `%Input` will be a list of `Files` separated by spaces.
