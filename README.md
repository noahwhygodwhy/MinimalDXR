I wanted to see what the reasonabily minimum number of lines it takes to do DXR is. This is not a good example of how to do DirectX Raytracing. DO NOT follow this as an exmaple. Instead I would follow the lovely tutorial at https://landelare.github.io/2023/02/18/dxr-tutorial.html which this implementation shamelessly copies code from.

The rules I followed:
  1. No double semi-colon'd lines. ie `fn(a) {b;}` is ok, but `fn(a) {b; c;}` is not
  2. Includes in another file as that's preprocessor stuff and I don't take responsibility for that. Also geometry data cause I could have put that inline but that just feels wrong.

There is more that could be done, ie void pointer shenanigans, to save lines but I felt this was a good point between (un)readability and fewer lines.

BE CAREFUL running this. You can't move the window, and it doesn't respect the "X" button to close. You have to either close it via terminating in visual studio or task manager.
