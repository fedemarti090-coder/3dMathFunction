import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('../data/outputDebug.csv', delimiter=',', skiprows=1)

x_vals = data[:, 0]
y_vals = data[:, 1]
z_vals = data[:, 2]

x_unique = np.unique(x_vals)
y_unique = np.unique(y_vals)

side_x = len(x_unique)
side_y = len(y_unique)

Z = z_vals.reshape((side_y, side_x))

X, Y = np.meshgrid(x_unique, y_unique)

fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(projection='3d')

surf = ax.plot_surface(X, Y, Z, cmap='plasma', edgecolor='none', alpha=0.9)

ax.set_title("Debug CSV MathEngine", fontsize=14, color='white')
ax.set_xlabel("Asse X")
ax.set_ylabel("Asse Y")
ax.set_zlabel("Asse Z")

fig.patch.set_facecolor('#1e1e1e')
ax.set_facecolor('#1e1e1e')
ax.xaxis.label.set_color('white')
ax.yaxis.label.set_color('white')
ax.zaxis.label.set_color('white')
ax.tick_params(colors='white')

fig.colorbar(surf, shrink=0.5, aspect=5)
plt.show()