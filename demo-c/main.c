#include <stdio.h>

#include <acknext.h>

#include <math.h>

int main(int argc, char *argv[])
{
    if(engine_open(argc, argv) == false)
    {
        printf("Failed to initialize engine: %s\n", engine_lasterror(NULL));
        return 1;
    }

    float time = 0.0;
    while(engine_frame())
    {
        // Now: Render!

        screen_color.red = 128 + 127 * sin(time);
        screen_color.green = 128 + 127 * sin(time + 1.0);

        engine_log("time_step: %f", time_step);

        time += time_step;
    }
    engine_close();
    return 0;
}