<div align="center">
  <img src="docs/logo.png" alt="Fighter Jet PS3 Demo Logo" width="320"/>
  <h1>✈️ Fighter Jet PS3 Demo</h1>
  <p><strong>Demo técnico de vuelo arcade en pixel-art 3D nativo para PlayStation 3</strong></p>
  <p>Desarrollado en C con <strong>PSL1GHT</strong> y aceleración por hardware RSX mediante <strong>Tiny3D</strong>.</p>
  <img src="docs/banner.png" alt="Fighter Jet PS3 Demo Banner" width="100%"/>
</div>

---

## 🚀 Estado Actual y Logros (v0.2)
El proyecto se encuentra en su **versión 0.2**, funcionando de manera fluida en el emulador **RPCS3** e incorporando los siguientes avances técnicos:

* **Menú Principal con Piloto Automático:** Pantalla de título interactiva (*PRESIONE START*) donde el caza sobrevuela el archipiélago ejecutando giros y maniobras de forma autónoma hasta que el jugador toma el control.
* **Sistema de 3 Cámaras / Vistas en Tiempo Real (`Triángulo`):**
  1. **Vista Cenital (Top-Down):** Renderizado por capas (*sprite stacking*) de 7 niveles para el caza, 6 niveles de relieve para islas procedurales, nubes flotantes, sombra dinámica proyectada según la altitud y doble estela aerodinámica en las puntas de las alas.
  2. **Vista 3D de Persecución:** Proyección en perspectiva con cielo degradado, horizonte marítimo, islas y nubes en el plano 3D y propulsores traseros animados.
  3. **Vista de Cabina (Primera Persona):** Tablero de instrumentos con radar, horizonte artificial dinámico, marco estructural y mira HUD central sensible al alabeo.
* **Física de Vuelo Arcade:** Controles suavizados con inercia de giro, control de altitud, aceleración con postcombustión (`X` / `R2`) y tonel volado de 360° (*barrel roll* con `Cuadrado`).
* **Personalización XMB y Diagnóstico:** Paquete `.pkg` personalizado con icono `ICON0.PNG`, fondo `PIC1.PNG` a 1080p y sistema de registro (`/dev_hdd0/ps3_fighter.log`) con marcas de tiempo para depuración en hardware real.

## 📸 Capturas de Pantalla

| Vista Cenital (Top-Down) | Vista 3D Persecución | Vista de Cabina (HUD) |
| :---: | :---: | :---: |
| <img src="docs/zenital.png" width="260"/> | <img src="docs/persecucion3d.png" width="260"/> | <img src="docs/cabina.png" width="260"/> |

## 🕹️ Controles (DualShock 3)
* **Stick Izquierdo / Cruceta:** Girar y controlar la altitud de vuelo.
* **`X` o `R2`:** Activar propulsores (postcombustión).
* **`Cuadrado`:** Realizar tonel volado (*barrel roll* 360°).
* **`Triángulo`:** Alternar entre las 3 cámaras (Cenital / 3D Persecución / Cabina).
* **`Start`:** Iniciar vuelo desde el menú / Salir al XMB.
* **`Select`:** Volver al menú principal (modo demo).

## 🙏 Créditos e Inspiración
* **Concepto original:** Inspirado en [*fighter · Forge*](https://ui8.ai) (UI8 Forge).
* **Librerías Homebrew:** Desarrollado sobre [PSL1GHT](https://github.com) y [Tiny3D](https://github.com) de Hermes.
