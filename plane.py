import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

point  = np.array([0, -.5, 2])
normal = np.array([0, 4, 0])

point1  = np.array([0, -.5, 2])
point2  = np.array([2, -.5, 0])
point3  = np.array([0, -.5, 0])

point4  = np.array([-1, -.5, 0])

# a plane is a*x+b*y+c*z+d=0
# [a,b,c] is the normal. Thus, we have to calculate
# d and we're set
d = -point.dot(normal)

# create x,z
xx, zz = np.meshgrid(range(10), range(10))

# calculate corresponding y
y = (-normal[0] * xx - normal[2] * zz - d) * 1. /normal[1]

# plot the surface
plt3d = plt.figure().gca(projection='3d')
plt3d.plot_surface(xx, y, zz)

plt3d.plot([point1[0]], [point1[1]], [point1[2]], color='yellow', marker='o', markersize=10, alpha=0.8);
plt3d.plot([point2[0]], [point2[1]], [point2[2]], color='yellow', marker='o', markersize=10, alpha=0.8);
plt3d.plot([point3[0]], [point3[1]], [point3[2]], color='yellow', marker='o', markersize=10, alpha=0.8);

plt3d.plot([point4[0]], [point4[1]], [point4[2]], color='red', marker='o', markersize=10, alpha=0.8);

plt.show()