#include "zpl-tools.h"
#include "pipe.h"

int main(int arg_c, char** arg_v) {
    hide_console();

    PNG_ENCODER png_mode = PE_LODEPNG; // smaller PNG size
    bool test_reuse = false;
    bool silent = false;
    bool loud = false;
    bool print_memory = false;
    bool streamBase64 = false;
    std::string zpl_full_path = "";
    int num_of_tests = 15;
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
        if (arg == "-m") {
            print_memory = true; // print memory usage (for debugging)
            continue;
        }
        if (arg == "-b") {
            streamBase64 = true; // stream PNG data as base64
            continue;
        }
        if (arg == "silent") {
            silent = true; // no stdout output
            continue;
        }
        if (arg == "loud") {
            loud = true; // enable pop-up notifications
            continue;
        }
        if (arg == "-t") {
            test_reuse = true; // run multiple times for testing
            // Parse number
            if (arg_i + 1 < arg_c) {
                num_of_tests = atoi(arg_v[arg_i + 1]);
                if (num_of_tests < 1) num_of_tests = 15;
                else arg_i++;
            }
            continue;
        }
        // Handle "<file>" for input without '-' prefix
        if (zpl_full_path.empty() && arg[0] != '-') {
            zpl_full_path = arg;
            continue;
        }
    }

    bool got_file = !zpl_full_path.empty();

    notifications_enabled = loud;

    std::string pipe_data = getPipe();
    bool got_pipe = !pipe_data.empty();

    if (got_pipe || streamBase64) {
        silent = true;
        streamBase64 = true;
    }

    // if (got_pipe) {
    //     printf("Pipe data: %s\n", pipe_data.c_str());
    // } else {
    //     printf("No data found in the pipe.\n");
    // }


    if (!got_pipe && !got_file) {
        if (!silent) {
            notify("No ZPL file specified\n");
            printf("Usage: zpl2png <file> [-f] [-s] [-b] [silent] [loud]\n");
            printf("  <file>  ZPL file to convert to PNG\n");
            printf("  -f         Use fast PNG encoding (default is small PNG size)\n");
            printf("  -s         Use small PNG size (default is fast PNG encoding)\n");
            printf("  -b         Stream PNG data as base64\n");
            printf("  silent     No stdout output\n");
            printf("  loud       Enable pop-up notifications\n");
        }
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
    std::string zpl_input;
    size_t size = 0;
    // timer.start("Loaded");
    if (got_file && !got_pipe) {
        const char* zpl_raw = loadFile(zpl_file.c_str());
        if (!zpl_raw) {
            notifyf("Failed to load ZPL file: %s\n", zpl_file.c_str());
            return 1;
        }
        zpl_input = zpl_raw;
        // timer.log("Loaded");
    } else if (got_pipe) {
        zpl_input = pipe_data.c_str();
    }


    const int width = 1800;
    const int height = 1200;

    std::vector<uint8_t> png_output;
    if (print_memory) printHeapUsage();

    for (int i = 0; i < num_of_tests; i++) {
        if (!test_reuse && i > 0) break;

        timer.start("Total_2");

        int error = zpl2png(zpl_input, png_output, width, height, 0, png_mode, !print_memory && !silent); // Faster but less compression

        if (error) {
            notifyf("Error converting ZPL to PNG: %d\n", error);
            printf("ZPL data: %d bytes\n%s\n", zpl_input.size(), zpl_input.c_str());
            return 2;
        }

        // Save to a file with the same name as the ZPL file but with a PNG extension
        if (got_file && !streamBase64) {
            saveFile(png_full_path.c_str(), (const char*) png_output.data(), png_output.size());
        }
        if (streamBase64) {
            // printf("PNG data: %d bytes\n", png_output.size());
            std::string png_base64 = b64encode(png_output.data(), png_output.size());
            printf("%s\n", png_base64.c_str());
        }
        size = png_output.size();
        double saved = timer.time("Total_2");
        if (!print_memory && test_reuse && !silent) printf("Total time: %.1f ms for %d bytes\n", saved * 1000.0, size);
        if (print_memory) printHeapUsage();
    }
    double total = timer.time("Total");
    if (!test_reuse && !silent) printf("Total time: %.1f ms for %d bytes\n", total * 1000.0, size);
    return 0;
}