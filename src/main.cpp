#include "zpl-tools.h"


uint16_t calculate_checksum(const char* data, int len) {
    uint16_t sum = 0;
    for (int i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

struct StopWatch {
    double t;
    double duration;
    void start() {
        t = GetTime();
    }
    void stop() {
        duration = GetTime() - t;
    }
    void log(const char* message) {
        stop();
        printf("%s in %.3f ms\n", message, duration * 1000.0);
    }
} timer;

int main() {

    for (int i = 0; i < 5; i++) {
        printf("#################################");
        printf("###### RENDERING CYCLE  %d #######", i);
        printf("#################################");

        timer.start();
        const char* zpl_sample = loadFile("zpl_sample.zpl");
        int zpl_sample_len = strlen(zpl_sample);
        timer.log("Loaded");

        timer.start();
        ZPL_label label = parse_zpl(zpl_sample, zpl_sample_len);
        // label.print();
        timer.log("Parsed");

        if (label.error) {
            printf("Error: %s\n", label.message);
            int offset = 0;
            int row = 0;
            const char* line = lineAt(zpl_sample, zpl_sample_len, label.idx, &offset, &row);
            printf("  Line %d\n", row);
            printf("   %s\n", line);
            printf("   %*s\n", offset, "^");
            return 1;
        }

        const int img_width = 1800;
        const int img_height = 1200;

        Image image = Image(img_width, img_height, WHITE);
        timer.start();
        label.draw(image);
        timer.log("Drawn");

        timer.start();
        std::vector<unsigned char>* png = image.toPNG(PE_LODEPNG); // Slower but better compression
        // std::vector<unsigned char>* png = image.toPNG(PE_FPNG); // Faster but less compression
        if (!png) {
            printf("Error encoding PNG\n");
            return 1;
        }
        if (png->size() == 0) {
            printf("Error encoding PNG: Output file is empty\n");
            return 1;
        }
        timer.log("Encoded to PNG");
        printf("PNG size: %d\n", png->size());

        timer.start();
        saveFile("output.png", (const char*) png->data(), png->size());
        timer.log("Saved");

    }
    return 0;
}