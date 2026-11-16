# ✈️ Jet Fighter PS3 Demo

Demo técnico de vuelo arcade en *pixel-art* pseudo-3D desarrollado de forma nativa para **PlayStation 3** mediante la técnica de **sprite stacking**, utilizando el SDK abierto **PSL1GHT** y aceleración por hardware con **Tiny3D**.

## 🎮 Características
* **Renderizado por capas (Sprite Stacking):** Caza de combate de 7 capas, islas procedurales con 6 niveles de relieve y nubes flotantes.
* **Efectos visuales en tiempo real:** Sombra dinámica proyectada según la altitud y estelas aerodinámicas en ambas alas.
* **Controles suaves:** Inercia de giro, alabeo (*barrel roll*) y aceleración con propulsores.

## 🕹️ Controles (DualShock 3)
* **Stick Izquierdo / Cruceta:** Girar y controlar la altitud de vuelo.
* **`X` o `R2`:** Activar propulsores (aumentar velocidad).
* **`Cuadrado`:** Realizar tonel volado (*barrel roll*).
* **`Start`:** Salir al menú principal de la consola (XMB).

## 🙏 Créditos e Inspiración
* **Concepto original:** Inspirado en el fragmento interactivo [*fighter · Forge*](https://ui8.ai) creado en la plataforma **Forge (UI8)**.
* **Librerías Homebrew:** Desarrollado sobre [PSL1GHT](https://github.com/ps3dev/PSL1GHT) y [Tiny3D](https://github.com) de Hermes.

## 🛠️ Cómo compilar desde el código fuente

### Requisitos previos
* Entorno Linux o **WSL (Ubuntu)** con **ps3toolchain** y **PSL1GHT** configurados.
* Librerías de `portlibs` instaladas (específicamente `tiny3d`).

### Pasos de compilación
1. Clona este repositorio y entra en la carpeta del proyecto:
   ```bash
   git clone https://github.com/animekai/Jet-Fighter-PS3-Demo.git
   cd Jet-Fighter-PS3-Demo
   ```
2. Compila el binario ejecutando `make` (o genera directamente el instalador `.pkg`):
   ```bash
   make clean && make pkg
   ```
3. Se generarán los archivos `Jet-Fighter-PS3-Demo.self` y `Jet-Fighter-PS3-Demo.pkg` en la raíz del proyecto.

## 📦 Instalación y Ejecución
* **En PS3 (CFW / HEN):** Copia `Jet-Fighter-PS3-Demo.pkg` a una memoria USB en formato FAT32 e instálalo desde *Administrar archivos PKG*.
* **En PC (Emulador RPCS3):** Arrastra `Jet-Fighter-PS3-Demo.self` o `Jet-Fighter-PS3-Demo.pkg` directamente a la ventana del emulador.
