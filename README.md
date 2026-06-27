This is the renderer code extracted out of space-nerds-in-space so that
it may be used in other projects.  An attempt was made to preserve the git
history.

I ran the following on space-nerds-in-space repo on Sat 27 Jun 2026 03:09:55 PM EDT

git-filter-repo --paths-from-file ~/renderer-files.txt
git-filter-repo --path-rename :renderer/

The contents of renderer-files.txt was:

```
vec4.c
fallthrough.h
liang-barsky.h
arraysize.h
mikktspace.c
snis_graph.c
matrix.h
main.c
shader.c
ui_colors.c
triangle.h
vertex.h
mathutils.h
workqueue.h
ui_colors.h
bline.h
entity.c
matrix.c
share/snis/textures/AreaTex.h
share/snis/textures/SearchTex.h
snis_alloc.c
my_point.h
snis_font.h
build_bug_on.h
liang-barsky.c
graph_dev_opengl.c
snis_cardinal_colors.h
shader.h
opengl_cap.c
entity.h
string-utils.h
mtwist.h
vec4.h
string-utils.c
mikktspace/mikktspace.h
mikktspace/mikktspace.c
mikktspace/Makefile
mikktspace/README
opengl_cap.h
snis_typeface.c
open-simplex-noise.c
material.h
graph_dev.h
open-simplex-noise.h
mathutils.c
bline.c
png_utils.h
snis_graph.h
stacktrace.c
mtwist.c
snis_typeface.h
pthread_util.h
entity_private.h
mesh.h
png_utils.c
stacktrace.h
snis_alloc.h
stl_parser.h
workqueue.c
pthread_util.c
quat.h
mesh.c
quat.c
```


