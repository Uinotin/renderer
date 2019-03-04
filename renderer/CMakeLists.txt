add_library(renderer window.c vulkancontext.c nodetree.c node.c vulkanswapchain.c programs.c event.c rootnode.c)
target_link_libraries(renderer glfw)
