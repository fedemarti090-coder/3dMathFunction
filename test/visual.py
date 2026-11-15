import matplotlib.pyplot as plt
import numpy as np

# 1. Crea la griglia da -1 a 1 con step 0.1
x = np.arange(-1, 1.1, 0.1)
y = np.arange(-1, 1.1, 0.1)
X, Y = np.meshgrid(x, y)

# 2. Calcola Z in base alla funzione z = x^3 - 3xy^2
Z = X**3 - 3 * X * Y**2

# 3. Inizializza la figura 3D
fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(projection='3d')

# 4. Disegna la superficie
surf = ax.plot_surface(X, Y, Z, cmap='plasma', edgecolor='none')

# 5. Personalizza il grafico
ax.set_title('Funzione 3D: $z = x^3 - 3xy^2$ (Dominio [-1, 1], step 0.1)', fontsize=13)
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
fig.colorbar(surf, shrink=0.5, aspect=5, label='Valore Z')

# Mostra il grafico
plt.show()