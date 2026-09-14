# Simulador tangencial de chaBots

Aplicación Python de escritorio, independiente del robot. En macOS abre `start.command` con doble clic. La primera ejecución instala PySide6 en un entorno local si hace falta.

En otros sistemas, desde esta carpeta:

```sh
python3 -m venv .venv
# Linux:
.venv/bin/python -m pip install -r requirements.txt
.venv/bin/python app.py
# Windows: usa .venv\Scripts\python.exe en lugar de .venv/bin/python
```

## Uso

- Elige un escenario y pulsa **Iniciar**. Ajusta las ocho constantes del controlador durante la ejecución; pausa, avanza un paso o reinicia para comparar.
- **Entradas manuales** permite introducir X/Y, color, error IMU y validez de detección directamente, sin el modelo físico. Pulsa **Un paso** o **Iniciar** para evaluar los cambios. El primer paso tras reiniciar no aplica derivada, igual que el firmware.
- La gráfica muestra dirección solicitada, dirección limitada por Ackermann y error IMU. Se conservan las últimas 600 muestras; puedes exportarlas a CSV.
- **Restaurar valores del firmware** relee los encabezados actuales. **Exportar parámetros** guarda una copia `.h` para copiar las constantes manualmente; no actualiza el robot.

## Qué reproduce

`model.py` reproduce `TrajectoryController::tangentEvasion`: tangente con `asin`, peso smoothstep, ganancias de orientación y evasión, PD con derivada angular envuelta y filtro exponencial. Los valores iniciales se leen de `include/ControlValues.h` y el límite de dirección de `lib/AckermannController/AckermannController.h`. Sin detección se aplica `NO_OBSTACLE_IMU_GAIN` y se reinicia la memoria del controlador, como en `Robot::executeEvadeUntilEdge`.

La aplicación usa un periodo fijo configurable (milisegundos), independiente de retrasos de la interfaz. Ajustar ganancias durante la marcha puede producir picos de derivada; reinicia para comparar desde las mismas condiciones.

## Alcance y aproximaciones

La trayectoria utiliza un modelo de bicicleta: velocidad angular = velocidad / distancia entre ejes × tan(dirección). Dirección positiva gira a la izquierda. La velocidad en mm/s es configurable: no se deduce del PWM. No se incluyen inercia, deslizamiento, latencia del servo ni ruido de sensores.

La cámara es una proyección lineal ideal: X = ancho/2 + distancia lateral × escala; Y = distancia frontal × escala. **No es una calibración de la OpenMV**. Se conserva el cálculo del firmware `atan2(X−160, Y)` y su signo, sin corregirlo según la trayectoria que parezca deseable. La imagen muestra las coordenadas usadas por el controlador, con Y hacia abajo. El radio punteado convierte píxeles a mm únicamente con esa escala ideal. Obstáculos físicos de radio supuesto 35 mm y robot circular configurable.

Se modela el tramo de evasión, no toda la máquina de estados de obstáculos: no hay paredes/ToF, líneas azules, estacionamiento ni maniobra de recuperación. Al llegar al umbral de recuperación del firmware o a un contacto geométrico, la simulación se pausa e indica la causa. La validez UART se controla manualmente; no hay conexión con la cámara real. Por ello, usa el modo manual para estudiar las ecuaciones y la trayectoria para explorar tendencias, no para predecir posiciones reales sin calibración.
