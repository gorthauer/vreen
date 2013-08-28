import qbs.base 1.0
import qbs.fileinfo as FileInfo

Module {
    property string binDestination: 'bin'
    property string libDestination: {
        if (qbs.targetOS === 'windows')
            return "bin";
        else
            return "lib";
    }
    property string qmlDestination: "bin"

    property string versionMajor:  0
    property string versionMinor: 9
    property string versionRelease: 5
    property string version: versionMajor + '.' + versionMinor + '.' + versionRelease

    Depends { name: "cpp" }
    Depends { name: "Qt.core"}

    Properties {
        condition: qbs.toolchain != 'msvc'
        cpp.cxxFlags: base.concat([ "-std=c++11" ])
    }
    Properties {
        condition: qbs.targetOS == "mac"
        cpp.cxxFlags: base.concat([ "-stdlib=libc++" ])
    }

    setupRunEnvironment: {
        putenv('QML2_IMPORT_PATH', product.buildDirectory + "/" + qmlDestination);
    }

    Rule {
        inputs: [ "devheader" ]
        Artifact {
            fileTags: [ "hpp",  "installed_content" ]
            fileName: "GeneratedFiles/" + input.moduleProperty("qbs", "installDir") + "/" + input.fileName
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                var inputFile = new TextFile(input.fileName, TextFile.ReadOnly);
                var file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write("#include \"" + input.fileName + "\"\n");
                file.close();
            }
            cmd.description = "generating " + FileInfo.fileName(output.fileName);
            cmd.highlight = "filegen";
            return cmd;
        }
    }

    Rule {
        inputs: [ "qml" ]
        Artifact {
            fileTags: [ "installed_content" ]
            fileName: input.moduleProperty("qbs", "installDir") + "/" + input.fileName
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                File.remove(output.fileName);
                if (!File.copy(input.fileName, output.fileName))
                    throw "Cannot install '" + input.fileName + "' as '" + output.fileName + "'";
            }
            cmd.description = "copying " + FileInfo.fileName(output.fileName);
            cmd.highlight = "linker";
            return cmd;
        }
    }
}
