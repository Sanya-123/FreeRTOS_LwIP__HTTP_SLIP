import qbs

Project {
    minimumQbsVersion: "1.7.1"

    CppApplication {
        consoleApplication: true
        files: [ "*.c", "*.h" ]

        Group {
            fileTagsFilter: product.type
            qbs.installDir: "/tmp" // for remote debugger
            qbs.install: true
        }

            cpp.defines: ["SPI_RW"]
            //cpp.defines: ["EBI_RW"]
    }
}
