import qbs
import qbs.FileInfo

Product {
    type: ["application", "bin", "hex", "size"]
    Depends { name:"cpp" }
    cpp.positionIndependentCode: false
    cpp.enableExceptions: false
    cpp.executableSuffix: ".elf"

    cpp.defines: [
        "STM32F407xx",
        "PRIO_LWIP=3",  //приоритеты тдля lwip
        "LWIP_DEBUG",

//        "STM32F407xx",
//    //        "HW_V2",
//        "HW_V1",

//    //        "SOCKETS_MT",  //its_my for sockets_mt

//        "LIB_ALLOC_FUNC=pvPortMalloc",
//        "LIB_FREE_FUNC=vPortFree",

    ]

    files: [
        "Drivers/**",
        "src/*",
        "src/console_mod/*",
                //OS
        "FreeRTOS/include/*",
        "FreeRTOS/include/private/*",
        "FreeRTOS/FreeRTOS/*",
        "FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F/*",
        "FreeRTOS/FreeRTOS/portable/MemMang/heap_2.c",
        "FreeRTOS/Trace/Streaming/*",
            //lwiP  =  lwip     lwip_povorotnoe    lwip_old
        "lwip/api/*",
        "lwip/arch/*",
        "lwip/core/**",
        "lwip/httpd/*",
        "lwip/httpd/ssi/*",
        "lwip/httpd/post/*",
        "lwip/netif/*",
        "lwip/include/**",
            //apps programm thast include from telnet / console
        "src/apps/ebi_rw-master/*.c",
        "src/apps/ebi_rw-master/*.h",
    ]

    cpp.includePaths: [
        "Drivers/",
        "src/",
        "src/console_mod/",
                //OS
        "FreeRTOS/include/",
        "FreeRTOS/include/private/",
//        "FreeRTOS/FreeRTOS/",
        "FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F/",
        "FreeRTOS/Trace/Streaming/",
            //lwiP
        "lwip/",
        "lwip/include/",
        "lwip/httpd/ssi/",
            //apps programm thast include from telnet / console
        "src/apps/ebi_rw-master/",
        "src/apps/ebi_rw-master/",
    ]

    cpp.driverFlags: [
        "-mthumb",
        "-mcpu=cortex-m4",
        "-mfloat-abi=hard",
        //"-msoft-float",
        "-mfpu=fpv4-sp-d16",
        "-fno-strict-aliasing",
        "-fdata-sections",
        "-ffunction-sections",
        "-fshort-enums",
        "-flto",
        "-finput-charset=UTF-8",
        "-fexec-charset=cp1251",
        "-std=gnu11",
        "-Wall",
        "-Wpedantic",
        "-Wno-main",
        "-Wno-unused-function",
        "-Wno-old-style-declaration",
        "-specs=nano.specs",
    ]

    cpp.commonCompilerFlags: [

    ]

    cpp.linkerFlags :  [
        "-print-memory-usage",
        "-start-group",
        "-gc-sections",
        "-lnosys",
        "-nostdlib",
        "-T" + path + "/STM32F407VGTx_FLASH.ld",
    ]

//    cpp.linkerScripts: [
//        "STM32F407VGTx_FLASH.ld"//NOTE flase lenght RAM
//    ]

    Properties {
        condition: qbs.buildVariant === "debug"
        cpp.driverFlags: outer.concat(["-Og", "-ggdb", "-DDEBUG=1"])
    }

    Properties {
        condition: qbs.buildVariant === "release"
        cpp.driverFlags: outer.concat(["-Og", "-g0", "-s", "-DDEBUG=0"])
    }

    Rule {
        id: bin
        inputs: "application"
        Artifact {
            fileTags: ["bin"]
           filePath: FileInfo.baseName(input.filePath) + ".bin"
        }
        prepare: {
                var args = ["-O", "binary", input.filePath, output.filePath];
                var cmd = new Command("arm-none-eabi-objcopy", args);
                cmd.description = "converting to bin: "+FileInfo.fileName(input.filePath);
                cmd.highlight = "linker";
                return cmd;
        }
    }

    Rule {
        id: hex
        inputs: "application"
        Artifact {
            fileTags: ["hex"]
            filePath: FileInfo.baseName(input.filePath) + ".hex"
        }
        prepare: {
            var args = ["-O", "ihex", input.filePath, output.filePath];
            var cmd = new Command("arm-none-eabi-objcopy", args);
            cmd.description = "converting to hex: "+FileInfo.fileName(input.filePath);
            cmd.highlight = "linker";
            return cmd;
        }
    }

    Rule {
        id: size
        inputs: ["application"]
        Artifact {
            fileTags: ["size"]
            filePath: "-"
        }
        prepare: {
            var args = [input.filePath];
            var cmd = new Command("arm-none-eabi-size", args);
            cmd.description = "File size: " + FileInfo.fileName(input.filePath);
            cmd.highlight = "linker";
            return cmd;
        }
    }
}


//import qbs
//import qbs.FileInfo

//// Сборка gdb:  ./configure --with-python --target=arm-elf
//// openocd -f interface/stlink-v1.cfg   -f target/stm32f1x_stlink.cfg
//// openocd -f interface/vsllink_swd.cfg -f target/stm32f1x.cfg  -f stm32_debug.cfg
//// vsprog -V"tvcc.set 3300" -sstm32f1 -ms -I firmware.hex -oe -owf -ovf
//// openocd -f interface/vsllink.cfg -f target/stm32f1x.cfg
//// st-util -s 1 -m
//// -V"tvcc.set 3300" -sstm32f1 -ms -I %{DebuggedExecutable:NativePath}/firmware.hex -oe -owf -ovf
////(My)openocd -f interface\jlink.cfg

//Product {
//    name: "firmware.elf"             // название выходного файла (без суффикса, он зависит от цели)
//    // cpp.executableSuffix: ".elf"  // но почему-то работает не всегда

//    property string hwVersion: "1"
//    property string fwVersion: "1"

//    type: ["application", "bin", "hex", "size"] // Что мы хотим получить на выходе
//    Depends { name:"cpp" }              // этот продукт зависит от компилятора C++


//    cpp.defines: [
//        "STM32F407xx",
////        "HW_V2",
//        "HW_V1",

////        "SOCKETS_MT",  //its_my for sockets_mt

//        "LIB_ALLOC_FUNC=pvPortMalloc",
//        "LIB_FREE_FUNC=vPortFree",
//        "PRIO_LWIP=3",  //приоритеты тдля lwip
//    ]

//    files: [
//        "Drivers/**",
//        "src/**",
//                //OS
////        "FreeRTOS/**",
////        "FreeRTOS/portable/**",
////        "FreeRTOS/FreeRTOS-Plus-TCP/**",
////         "FreeRTOS/FreeRTOS-Plus-TCP/include/**",
////        "FreeRTOS/FreeRTOS-Plus-TCP/source/**",
////        "FreeRTOS/FreeRTOS-Plus-TCP/source/portable/**",
//                //lwiP
////        "lwip/**",
////        "lwip/api/**",
////        "lwip/core/**",
////        "lwip/nrtif/**",
////        "lwip/include/**",
////        "lwip/httpd/**",
////        "lwip/arch/**",
//    ]

//    cpp.includePaths: [
//        "Drivers/",
//        "src/",
//                //OS
////        "FreeRTOS/",
////        "FreeRTOS/portable/",
////        "FreeRTOS/FreeRTOS-Plus-TCP/",
////        "FreeRTOS/FreeRTOS-Plus-TCP/include/",
////        "FreeRTOS/FreeRTOS-Plus-TCP/source/",
////        "FreeRTOS/FreeRTOS-Plus-TCP/source/portable/",
//                //lwiP
////        "lwip/",
////        "lwip/api/",
////        "lwip/core/",
////        "lwip/nrtif/",
////        "lwip/include/",
////        "lwip/httpd/",
////        "lwip/arch/",
//    ]

//    cpp.linkerScripts: [
//        "STM32F407VGTx_FLASH.ld"//NOTE flase lenght RAM
////        "STM32.ld"
//    ]

//    cpp.commonCompilerFlags: [
//        "-mthumb",
//        "-mcpu=cortex-m4",
//        "-mfpu=fpv4-sp-d16",
//        "-mfloat-abi=hard",
//        "-Wall",
//        "-fdata-sections",
//        "-ffunction-sections",
// //       "-g",
// //       "-gdwarf-2",
//// //       "-msoft-float",
//////        "-mfpu=vfp",
//////        "-Os",
////        "-fdata-sections",
////        "-ffunction-sections",
////        "-fno-lto",
////        "-finput-charset=UTF-8",
////        "-fexec-charset=cp1251",
//////        "-fno-inline",
//           "-std=gnu11",
//// //       "-flto",
////        "-Wall",
//        "-J",
////     // "-O3",
//    ]

//    cpp.linkerFlags :  [
//        "-lc",
//        "-lm",
//        "-lnosys",
//        "-specs=nano.specs",
//        "-Wl,--gc-sections",

//        "-mthumb",
//        "-mcpu=cortex-m4",
//        "-mfpu=fpv4-sp-d16",
//        "-mfloat-abi=hard",
////        "-fno-lto",
////  //      "-flto",
////        "-mthumb",
////        "-mcpu=cortex-m3",
////  //      "-msoft-float",
////  //      "-mfpu=vfp",
////   //    "-specs=nano.specs",

////        "-specs=nosys.specs",
////        "-Wl,--start-group",
////        "-Wl,--gc-sections",
////   //             "-nodefaultlibs",
////        "-lnosys",//I
////        "-lgcc",//I
////             "-lc",//I
//////        "-lstdc++",
//////        "-lm",
////        "-Wl,-section-start=.ram=0x20000000"//Thats for memory
//////        "-Wl,-section-start=.rtt=0x20000000"//Thats for real time
//    ]

//    Properties {
//        condition: qbs.buildVariant === "debug"
//        cpp.driverFlags: outer.concat(["-Og", "-ggdb", "-DDEBUG=1"])
//    }

//    Properties {
//        condition: qbs.buildVariant === "release"
//        cpp.driverFlags: outer.concat(["-Og", "-g0", "-s", "-DDEBUG=0"])
//    }

//    Rule {
//        id: bin
//        inputs: "application"
//        Artifact {
//            fileTags: ["bin"]
//           filePath: FileInfo.baseName(input.filePath) + ".bin"
//        }
//        prepare: {
//                var args = ["-O", "binary", input.filePath, output.filePath];
//                var cmd = new Command("arm-none-eabi-objcopy", args);
//                cmd.description = "converting to bin: "+FileInfo.fileName(input.filePath);
//                cmd.highlight = "linker";
//                return cmd;
//        }
//    }

//    Rule {
//        id: hex
//        inputs: "application"
//        Artifact {
//            fileTags: ["hex"]
//            filePath: FileInfo.baseName(input.filePath) + ".hex"
//        }
//        prepare: {
//            var args = ["-O", "ihex", input.filePath, output.filePath];
//            var cmd = new Command("arm-none-eabi-objcopy", args);
//            cmd.description = "converting to hex: "+FileInfo.fileName(input.filePath);
//            cmd.highlight = "linker";
//            return cmd;
//        }
//    }

//    Rule {
//        id: size
//        inputs: ["application"]
//        Artifact {
//            fileTags: ["size"]
//            filePath: "-"
//        }
//        prepare: {
//            var args = [input.filePath];
//            var cmd = new Command("arm-none-eabi-size", args);
//            cmd.description = "File size: " + FileInfo.fileName(input.filePath);
//            cmd.highlight = "linker";
//            return cmd;
//        }
//    }
//}
