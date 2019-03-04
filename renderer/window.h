#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "vulkancontext.h"
#include "event.h"

#define MAXSHADERFILESIZE 8192 


typedef struct Window
{
  VulkanContext context;
  GLFWwindow * window;
  Event events[5];
  uint32_t numEvents;
} Window;

typedef struct WindowSize
{
  int width, height;
} WindowSize;

typedef struct WasdKeyStatus
{
  int w, a, s, d, up, down, left, right;
} WasdKeyStatus;


void InitWindowEvents(Window * window);
void UpdateWindow(Window * window);
int ShouldClose(Window * window);
int CreateWindow(Window * window);
void DestroyWindow(Window * window);

#endif
