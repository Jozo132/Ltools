#include "zpl-tools.h"


int main(int arg_c, char** arg_v) {
    hide_console();

    PNG_ENCODER png_mode = PE_LODEPNG; // smaller PNG size
    bool once = true;
    std::string zpl_full_path = "";

    for (int arg_i = 1; arg_i < arg_c; arg_i++) {
        std::string arg = arg_v[arg_i];
        // Handle '-f' for fast PNG encoding and '-s' for small PNG size
        if (arg == "-f") {
            png_mode = PE_FPNG; // faster PNG encoding
            continue;
        }
        if (arg == "-s") {
            png_mode = PE_LODEPNG; // smaller PNG size
            continue;
        }
        if (arg == "-t") {
            once = false; // run multiple times
            continue;
        }
        // Handle '-i <file>' for input file
        if (arg == "-i" && arg_i + 1 < arg_c) {
            zpl_full_path = arg_v[arg_i + 1];
            continue;
        }
        // Handle "<file>" for input without '-' prefix
        if (zpl_full_path.empty() && arg[0] != '-') {
            zpl_full_path = arg;
            continue;
        }
    }

    if (zpl_full_path.empty()) {
        notify("No ZPL file specified\n");
        printf("Usage: zpl2png -i <file> [-f] [-s] [-t]\n");
        printf("  -i <file>  ZPL file to convert to PNG\n");
        printf("  -f         Use fast PNG encoding (default is small PNG size)\n");
        printf("  -s         Use small PNG size (default is fast PNG encoding)\n");
        printf("  -t         Run multiple times (default is once)\n");
        return 1;
    }

    // Get file path and file name
    std::string zpl_file = zpl_full_path;
    std::string directory = "";
    size_t last_slash = zpl_full_path.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        directory = zpl_full_path.substr(0, last_slash);
        zpl_file = zpl_full_path.substr(last_slash + 1);
    }
    std::string png_file = zpl_file.substr(0, zpl_file.find_last_of('.')) + ".png";
    std::string png_full_path = "";
    if (directory.empty()) {
        png_full_path = png_file;
    } else {
        png_full_path = directory + "/" + png_file;
    }


    timer.start("Total");
    size_t size = 0;
    timer.start("Loaded");
    std::string zpl_input = loadFile(zpl_file.c_str());
    timer.log("Loaded");


    const int width = 1800;
    const int height = 1200;


    for (int i = 0; i < 5; i++) {
        if (once && i > 0) break;
        if (!once) {
            printf("#################################\n");
            printf("###### RENDERING CYCLE  %d #######\n", i);
            printf("#################################\n");
        }

        timer.start("Total_2");

        std::vector<uint8_t> png_output;
        int error = zpl2png(zpl_input, png_output, width, height, 0, png_mode, true); // Faster but less compression

        if (error) {
            notifyf("Error converting ZPL to PNG: %d\n", error);
            return 2;
        }

        // Save to a file with the same name as the ZPL file but with a PNG extension
        saveFile(png_full_path.c_str(), (const char*) png_output.data(), png_output.size());
        size = png_output.size();
        double saved = timer.time("Total_2");
        if (!once) printf("Total PNG: %.1f ms for %d bytes\n", saved * 1000.0, size);
    }
    double total = timer.time("Total");
    if (once) printf("Total time: %.1f ms for %d bytes\n", total * 1000.0, size);
    return 0;
}