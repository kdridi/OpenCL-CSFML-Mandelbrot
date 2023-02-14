//
//  main.c
//  mandelbrot
//
//  Created by Karim DRIDI on 04/07/2019.
//  Copyright © 2019 Karim DRIDI. All rights reserved.
//

#include <assert.h>

#include <stdio.h>
#include <stdbool.h>

#include <SFML/Graphics.h>

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

typedef cl_double t_double;
typedef cl_uint t_uint;

char OCLEB[1024];
const char *OCLE(cl_int err)
{
#define CASE(__NAME) case __NAME: sprintf(OCLEB, "%s", #__NAME); break
    switch (err) {
            CASE(CL_DEVICE_NOT_FOUND);
            CASE(CL_DEVICE_NOT_AVAILABLE);
            CASE(CL_COMPILER_NOT_AVAILABLE);
            CASE(CL_MEM_OBJECT_ALLOCATION_FAILURE);
            CASE(CL_OUT_OF_RESOURCES);
            CASE(CL_OUT_OF_HOST_MEMORY);
            CASE(CL_PROFILING_INFO_NOT_AVAILABLE);
            CASE(CL_MEM_COPY_OVERLAP);
            CASE(CL_IMAGE_FORMAT_MISMATCH);
            CASE(CL_IMAGE_FORMAT_NOT_SUPPORTED);
            CASE(CL_BUILD_PROGRAM_FAILURE);
            CASE(CL_MAP_FAILURE);
            CASE(CL_MISALIGNED_SUB_BUFFER_OFFSET);
            CASE(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
            CASE(CL_COMPILE_PROGRAM_FAILURE);
            CASE(CL_LINKER_NOT_AVAILABLE);
            CASE(CL_LINK_PROGRAM_FAILURE);
            CASE(CL_DEVICE_PARTITION_FAILED);
            CASE(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
            CASE(CL_INVALID_VALUE);
            CASE(CL_INVALID_DEVICE_TYPE);
            CASE(CL_INVALID_PLATFORM);
            CASE(CL_INVALID_DEVICE);
            CASE(CL_INVALID_CONTEXT);
            CASE(CL_INVALID_QUEUE_PROPERTIES);
            CASE(CL_INVALID_COMMAND_QUEUE);
            CASE(CL_INVALID_HOST_PTR);
            CASE(CL_INVALID_MEM_OBJECT);
            CASE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
            CASE(CL_INVALID_IMAGE_SIZE);
            CASE(CL_INVALID_SAMPLER);
            CASE(CL_INVALID_BINARY);
            CASE(CL_INVALID_BUILD_OPTIONS);
            CASE(CL_INVALID_PROGRAM);
            CASE(CL_INVALID_PROGRAM_EXECUTABLE);
            CASE(CL_INVALID_KERNEL_NAME);
            CASE(CL_INVALID_KERNEL_DEFINITION);
            CASE(CL_INVALID_KERNEL);
            CASE(CL_INVALID_ARG_INDEX);
            CASE(CL_INVALID_ARG_VALUE);
            CASE(CL_INVALID_ARG_SIZE);
            CASE(CL_INVALID_KERNEL_ARGS);
            CASE(CL_INVALID_WORK_DIMENSION);
            CASE(CL_INVALID_WORK_GROUP_SIZE);
            CASE(CL_INVALID_WORK_ITEM_SIZE);
            CASE(CL_INVALID_GLOBAL_OFFSET);
            CASE(CL_INVALID_EVENT_WAIT_LIST);
            CASE(CL_INVALID_EVENT);
            CASE(CL_INVALID_OPERATION);
            CASE(CL_INVALID_GL_OBJECT);
            CASE(CL_INVALID_BUFFER_SIZE);
            CASE(CL_INVALID_MIP_LEVEL);
            CASE(CL_INVALID_GLOBAL_WORK_SIZE);
            CASE(CL_INVALID_PROPERTY);
            CASE(CL_INVALID_IMAGE_DESCRIPTOR);
            CASE(CL_INVALID_COMPILER_OPTIONS);
            CASE(CL_INVALID_LINKER_OPTIONS);
            CASE(CL_INVALID_DEVICE_PARTITION_COUNT);
        default:
            sprintf(OCLEB, "0x%08X", err);
            break;
    }
#undef CASE
    return OCLEB;
}

typedef struct s_camera
{
    t_double x;
    t_double y;
    t_double z;
    t_uint m;
    t_uint w;
    t_uint h;
    sfBool mouseReleased;
    sfBool resetReleased;
    sfBool gputkReleased;
    sfBool printReleased;
    sfBool debugReleased;
    sfBool keyA;
    sfBool keyS;
    sfBool keyZ;
    sfBool keyX;
    sfBool keyG;
    sfBool keyD;
} t_camera;

static t_camera camera;

void camera_reset() {
    camera.x = -0.5;
    camera.y = 0;
    camera.z = 5;
    camera.m = 100;
    
    camera.x = -0.766031;
    camera.y = -0.100861;
    camera.z = 0.172708;
    camera.m = 1000;

    camera.w = 800;
    camera.h = 800;
    camera.mouseReleased = sfTrue;
    camera.resetReleased = sfTrue;
    camera.gputkReleased = sfTrue;
    camera.printReleased = sfTrue;
    camera.debugReleased = sfTrue;
    camera.keyA = sfFalse;
    camera.keyS = sfFalse;
    camera.keyZ = sfFalse;
    camera.keyX = sfFalse;
    camera.keyG = sfTrue;
    camera.keyD = sfFalse;
}

void camera_print() {
    printf("==========================\n");
    printf("camera.x = %f;\n", camera.x);
    printf("camera.y = %f;\n", camera.y);
    printf("camera.z = %f;\n", camera.z);
    printf("camera.m = %u;\n", camera.m);
    printf("==========================\n");
}

typedef struct s_framebuffer
{
    t_uint width;
    t_uint height;
    sfColor pixels[1];
} t_framebuffer;

#define map(x, ymin, ymax, zmin, zmax) (((x) - (ymin)) * ((zmax) - (zmin)) / ((ymax) - (ymin)) + (zmin))

t_framebuffer *framebuffer_create(t_uint width, t_uint height);
void framebuffer_destroy(t_framebuffer *self);

void framebuffer_draw(t_framebuffer *self);

void ocl_initialize(void);
void ocl_cleanup(void);

int main(int argc, const char * argv[])
{
    ocl_initialize();
    camera_reset();
    
    sfVideoMode mode = { .width = camera.w, .height = camera.h, .bitsPerPixel = 32 };
    sfUint32 style = sfClose;
    sfContextSettings settings = { 24, 8, 0};;
    
    sfRenderWindow *window = sfRenderWindow_create(mode, "mandelbrot", style, &settings);
    assert(window != NULL);
    sfRenderWindow_setKeyRepeatEnabled(window, sfFalse);
    
    t_framebuffer *buffer = framebuffer_create(mode.width, mode.height);
    assert(buffer != NULL);
    
    sfTexture *texture = sfTexture_create(mode.width, mode.height);
    assert(texture != NULL);
    
    sfSprite *sprite = sfSprite_create();
    assert(sprite != NULL);
    
    sfSprite_setTexture(sprite, texture, sfTrue);
    
    while (sfRenderWindow_isOpen(window) == sfTrue)
    {
        sfEvent event;
        sfRenderWindow_pollEvent(window, &event);
        
        if (event.type == sfEvtClosed)
            sfRenderWindow_close(window);
        
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyEscape)
            sfRenderWindow_close(window);
        
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyA && camera.keyA == sfFalse) camera.keyA = sfTrue;
        if (event.type == sfEvtKeyReleased && event.key.code == sfKeyA && camera.keyA == sfTrue) camera.keyA = sfFalse;
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyS && camera.keyS == sfFalse) camera.keyS = sfTrue;
        if (event.type == sfEvtKeyReleased && event.key.code == sfKeyS && camera.keyS == sfTrue) camera.keyS = sfFalse;
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyZ && camera.keyZ == sfFalse) camera.keyZ = sfTrue;
        if (event.type == sfEvtKeyReleased && event.key.code == sfKeyZ && camera.keyZ == sfTrue) camera.keyZ = sfFalse;
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyX && camera.keyX == sfFalse) camera.keyX = sfTrue;
        if (event.type == sfEvtKeyReleased && event.key.code == sfKeyX && camera.keyX == sfTrue) camera.keyX = sfFalse;
        
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyP && camera.printReleased == sfTrue)
            camera.printReleased = sfFalse;
        
        if (event.type == sfEvtKeyReleased && event.key.code == sfKeyP && camera.printReleased == sfFalse)
        {
            camera.printReleased = sfTrue;
            camera_print();
        }
        
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyR && camera.resetReleased == sfTrue)
            camera.resetReleased = sfFalse;
        
        if (event.type == sfEvtKeyReleased && event.key.code == sfKeyR && camera.resetReleased == sfFalse)
        {
            camera.resetReleased = sfTrue;
            camera_reset();
        }
        
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyG && camera.gputkReleased == sfTrue)
            camera.gputkReleased = sfFalse;
        
        if (event.type == sfEvtKeyReleased && event.key.code == sfKeyG && camera.gputkReleased == sfFalse)
        {
            camera.gputkReleased = sfTrue;
            camera.keyG = !camera.keyG;
        }
        
        if (event.type == sfEvtKeyPressed && event.key.code == sfKeyD && camera.debugReleased == sfTrue)
            camera.debugReleased = sfFalse;
        
        if (event.type == sfEvtKeyReleased && event.key.code == sfKeyD && camera.debugReleased == sfFalse)
        {
            camera.debugReleased = sfTrue;
            camera.keyD = !camera.keyD;
        }
        
        if (event.type == sfEvtMouseButtonPressed && event.mouseButton.button == sfMouseLeft && camera.mouseReleased == sfTrue)
        {
            camera.x = map(event.mouseButton.x, 0, camera.w, camera.x - camera.z * 0.5, camera.x + camera.z * 0.5);
            camera.y = map(event.mouseButton.y, 0, camera.h, camera.y - camera.z * 0.5, camera.y + camera.z * 0.5);
            camera.mouseReleased = sfFalse;
        }
        
        if (event.type == sfEvtMouseButtonReleased && event.mouseButton.button == sfMouseLeft && camera.mouseReleased == sfFalse)
            camera.mouseReleased = sfTrue;
        
        if (camera.keyA == sfTrue) camera.m += 1;
        if (camera.keyS == sfTrue) camera.m -= 1;
        if (camera.keyZ == sfTrue) camera.z *= 0.9;
        if (camera.keyX == sfTrue) camera.z /= 0.9;
        
        sfRenderWindow_setActive(window, sfTrue);
        sfRenderWindow_clear(window, sfBlack);
        framebuffer_draw(buffer);
        sfTexture_updateFromPixels(texture, (sfUint8*) buffer->pixels, buffer->width, buffer->height, 0, 0);
        sfRenderWindow_drawSprite(window, sprite, NULL);
        sfRenderWindow_display(window);
    }
    
    sfSprite_destroy(sprite);
    sfTexture_destroy(texture);
    framebuffer_destroy(buffer);
    sfRenderWindow_destroy(window);
    ocl_cleanup();
    
    return 0;
}

#include <stdlib.h>

t_framebuffer *framebuffer_create(t_uint width, t_uint height)
{
    t_framebuffer *self;
    
    self = malloc(sizeof(*self) + (width * height - 1) * sizeof(*(self->pixels)));
    assert(self != NULL);
    
    self->width = width;
    self->height = height;
    
    return (self);
}

void framebuffer_destroy(t_framebuffer *self)
{
    assert(self != NULL);
    free(self);
}
cl_context context;
cl_program program;
cl_kernel kernel;

cl_int clGetPlatformAndDeviceIDs(cl_platform_id *pid, cl_device_id *did)
{
    cl_uint ps;
    cl_uint ds;
    cl_platform_id pids[1024];
    cl_device_id dids[1024];
    cl_bool first = true;
    cl_uint max = 0;
    cl_int err;
    
    err = clGetPlatformIDs(1024, pids, &ps);
    if (err != CL_SUCCESS)
        return (err);

    for (cl_uint i = 0; i < ps; i++)
    {
        err = clGetDeviceIDs(pids[i], CL_DEVICE_TYPE_GPU, 1024, dids, &ds);
        if (err != CL_SUCCESS)
            return (err);
        
        for (cl_uint j = 0; j < ds; j++)
        {
            cl_uint n;
            char name[1024];
            
            err = clGetDeviceInfo(dids[j], CL_DEVICE_EXTENSIONS, sizeof(name) / sizeof(*name), &name, NULL);
            assert(err == CL_SUCCESS);
            printf("CL_DEVICE_EXTENSIONS: %s\n", name);
            
            err = clGetDeviceInfo(dids[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(n), &n, NULL);
            if (err != CL_SUCCESS)
                return (err);
            
            if (first == true || n > max)
            {
                *pid = pids[i];
                *did = dids[j];
                first = false;
                max = n;
            }
        }
    }
    assert(max > 0);
    
    if (true) {
        char name[1024];
        
        err = clGetPlatformInfo(*pid, CL_PLATFORM_NAME, sizeof(name) / sizeof(*name), &name, NULL);
        assert(err == CL_SUCCESS);
        printf("CL_PLATFORM_NAME: %s\n", name);
        
        err = clGetDeviceInfo(*did, CL_DEVICE_NAME, sizeof(name) / sizeof(*name), &name, NULL);
        assert(err == CL_SUCCESS);
        printf("CL_DEVICE_NAME: %s\n", name);
    }
    
    return (err);
}

const char *source_str(void);

void ocl_initialize(void)
{
    cl_int clErr;
    cl_platform_id platform_id;
    cl_device_id device_id;

    const char *source = source_str();
    
    clErr = clGetPlatformAndDeviceIDs(&platform_id, &device_id);
    assert(clErr == CL_SUCCESS);

    cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id, 0 };
    
    context = clCreateContext(properties, 1, &device_id, NULL, NULL, &clErr);
    assert(clErr == CL_SUCCESS);
    
    program = clCreateProgramWithSource(context, 1, &source,  NULL, &clErr);
    assert(clErr == CL_SUCCESS);
    
    clErr = clBuildProgram(program, 1, &device_id, "", NULL, NULL);
    if (clErr != CL_SUCCESS)
    {
        size_t length;
        char buffer[2048];
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
        buffer[length] = '\0';
        printf("%s", buffer);
    }
    assert(clErr == CL_SUCCESS);
    
    kernel = clCreateKernel(program, "mandelbrot", &clErr);
    assert(clErr == CL_SUCCESS);
}

void ocl_cleanup(void)
{
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseContext(context);
}

void framebuffer_drawCPU(t_double camx, t_double camy, t_double camz, t_uint camw, t_uint camh, t_uint camm, uint32_t *pixels)
{
    for (typeof(camh) i = 0; i < camh; i++)
    {
        for (typeof(camw) j = 0; j < camw; j++)
        {
            t_double x = j;
            t_double y = i;
            
            t_double ca = map(x, 0, camw, camx - camz * 0.5, camx + camz * 0.5);
            t_double cb = map(y, 0, camh, camy - camz * 0.5, camy + camz * 0.5);
            
            t_double za = 0;
            t_double zb = 0;
            
            t_uint n = 0;
            while (n < camm) {
                t_double a = za * za - zb * zb;
                t_double b = 2 * za * zb;
                
                za = a + ca;
                zb = b + cb;
                
                if (za * za + zb * zb > 10) {
                    break;
                }
                n += 1;
            }
            
            t_double b = map(n, 0, camm, 0, 255);
            if (n == camm) {
                b = 0;
            }
            
            sfUint32 *c = pixels + (i * camw + j);
            *c = 0;
            *c |= ((sfInt32) (b / 2.0)) << (8 * 0);
            *c |= ((sfInt32) (b * 3.0)) << (8 * 1);
            *c |= ((sfInt32) (b / 2.0)) << (8 * 2);
            *c |= ((sfInt32) (    255)) << (8 * 3);
        }
    }
}

void framebuffer_drawGPU(t_double camx, t_double camy, t_double camz, t_uint camw, t_uint camh, t_uint camm, uint32_t *pixels)
{
    cl_int clErr;
    cl_device_id device_ids[1024];
    
    size_t global_work_size[2];
    global_work_size[0] = camw;
    global_work_size[1] = camh;
    
    cl_mem buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(*pixels) * camw * camh, NULL, &clErr);
    assert(clErr == CL_SUCCESS);
    
    clErr = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device_ids), device_ids, NULL);
    assert(clErr == CL_SUCCESS);
    
    cl_command_queue queue = clCreateCommandQueue(context, device_ids[0], 0, &clErr);
    assert(clErr == CL_SUCCESS);
    
    clErr = clSetKernelArg(kernel, 0, sizeof(camx), &camx); assert(clErr == CL_SUCCESS);
    clErr = clSetKernelArg(kernel, 1, sizeof(camy), &camy); assert(clErr == CL_SUCCESS);
    clErr = clSetKernelArg(kernel, 2, sizeof(camz), &camz); assert(clErr == CL_SUCCESS);
    clErr = clSetKernelArg(kernel, 3, sizeof(camw), &camw); assert(clErr == CL_SUCCESS);
    clErr = clSetKernelArg(kernel, 4, sizeof(camh), &camh); assert(clErr == CL_SUCCESS);
    clErr = clSetKernelArg(kernel, 5, sizeof(camm), &camm); assert(clErr == CL_SUCCESS);
    clErr = clSetKernelArg(kernel, 6, sizeof(buffer), &buffer); assert(clErr == CL_SUCCESS);

    clErr = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);
    assert(clErr == CL_SUCCESS);
    
    clErr = clEnqueueReadBuffer(queue, buffer, CL_FALSE, 0, sizeof(*pixels) * camw * camh, pixels, 0, NULL, NULL);
    assert(clErr == CL_SUCCESS);
    
    clErr = clFinish(queue);
    assert(clErr == CL_SUCCESS);
    
    clErr = clReleaseCommandQueue(queue);
    assert(clErr == CL_SUCCESS);
    
    clErr = clReleaseMemObject(buffer);
    assert(clErr == CL_SUCCESS);
}

#include <time.h>

void framebuffer_draw(t_framebuffer *self)
{
    t_double camx = camera.x;
    t_double camy = camera.y;
    t_double camz = camera.z;
    t_uint camw = camera.w;
    t_uint camh = camera.h;
    t_uint camm = camera.m;
    uint32_t *pixels = (typeof(pixels)) self->pixels;
    
    clock_t t0 = clock() / (CLOCKS_PER_SEC / 1000);
    
    if (camera.keyG == sfTrue)
        framebuffer_drawGPU(camx, camy, camz, camw, camh, camm, pixels);
    else
        framebuffer_drawCPU(camx, camy, camz, camw, camh, camm, pixels);
    
    clock_t t1 = clock() / (CLOCKS_PER_SEC / 1000);
    
    if(camera.keyD == sfTrue)
        printf("GPU enabled: %s | Time : %lu ms\n", camera.keyG ? "true " : "false", t1 - t0);
}


const char *source_str(void)
{
    return
    "                                                                \n"
    "#pragma OPENCL EXTENSION cl_khr_fp64 : enable                   \n"
    "                                                                \n"
    "double map                                                      \n"
    "    ( double x                                                  \n"
    "    , double ymin                                               \n"
    "    , double ymax                                               \n"
    "    , double zmin                                               \n"
    "    , double zmax                                               \n"
    "    );                                                          \n"
    "                                                                \n"
    "double map                                                      \n"
    "    ( double x                                                  \n"
    "    , double ymin                                               \n"
    "    , double ymax                                               \n"
    "    , double zmin                                               \n"
    "    , double zmax                                               \n"
    "    ) {                                                         \n"
    "    return ((x - ymin) * (zmax - zmin) / (ymax - ymin) + zmin); \n"
    "}                                                               \n"
    "                                                                \n"
    "__kernel void mandelbrot                                        \n"
    "    ( const double camx                                         \n"
    "    , const double camy                                         \n"
    "    , const double camz                                         \n"
    "    , const uint camw                                           \n"
    "    , const uint camh                                           \n"
    "    , const uint camm                                           \n"
    "    , __global int* pixels                                      \n"
    "    ) {                                                         \n"
    "    const uint x = get_global_id(0);                            \n"
    "    const uint y = get_global_id(1);                            \n"
    "                                                                \n"
    "    const uint idx = x + y * camw;                              \n"
    "                                                                \n"
    "    double ca = map                                             \n"
    "        (x, 0, camw, camx - camz * 0.5, camx + camz * 0.5);     \n"
    "                                                                \n"
    "    double cb = map                                             \n"
    "        (y, 0, camh, camy - camz * 0.5, camy + camz * 0.5);     \n"
    "                                                                \n"
    "    double za = 0;                                              \n"
    "    double zb = 0;                                              \n"
    "                                                                \n"
    "    uint n = 0;                                                 \n"
    "    while (n < camm) {                                          \n"
    "        double a = za * za - zb * zb;                           \n"
    "        double b = 2 * za * zb;                                 \n"
    "                                                                \n"
    "        za = a + ca;                                            \n"
    "        zb = b + cb;                                            \n"
    "                                                                \n"
    "        if (za * za + zb * zb > 10) {                           \n"
    "            break;                                              \n"
    "        }                                                       \n"
    "        n += 1;                                                 \n"
    "    }                                                           \n"
    "                                                                \n"
    "    double b = map(n, 0, camm, 0, 255);                         \n"
    "    if (n == camm) {                                            \n"
    "        b = 0;                                                  \n"
    "    }                                                           \n"
    "                                                                \n"
    "    uint pa = 255;                                              \n"
    "    uint pb = b / 2.0;                                          \n"
    "    uint pg = b * 3.0;                                          \n"
    "    uint pr = b / 2.0;                                          \n"
    "                                                                \n"
    "    pixels[idx] = 0;                                            \n"
    "    pixels[idx] |= pa << (8 * 3);                               \n"
    "    pixels[idx] |= pb << (8 * 2);                               \n"
    "    pixels[idx] |= pg << (8 * 1);                               \n"
    "    pixels[idx] |= pr << (8 * 0);                               \n"
    "}                                                               \n"
    "";
}
