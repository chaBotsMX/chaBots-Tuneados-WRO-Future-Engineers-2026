# Monitor de telemetría WRO

Aplicación de depuración en tiempo real para macOS. Recibe por Wi-Fi la
telemetría que el XIAO ESP32-C6 obtiene de la Teensy y presenta las lecturas y
gráficas en un dashboard local.

No instala paquetes, no necesita internet y no envía comandos de movimiento al
robot. Usa únicamente la biblioteca estándar de Python y las capacidades del
navegador.

## Inicio rápido

1. En la Mac, conéctate a la red Wi-Fi que crea el XIAO:
   `WRO-Debug-C6`.
2. Abre `start.command` con doble clic.
3. La interfaz se abrirá en `http://127.0.0.1:8765/`.
4. Para terminar, vuelve a la ventana de Terminal y presiona `Control-C`.

Si macOS no permite abrir el lanzador por primera vez, haz clic derecho sobre
`start.command`, elige **Abrir** y confirma. También se puede iniciar desde una
terminal:q

```sh
cd debug_app
python3 robot_debug.py
```

Se requiere Python 3.10 o posterior. No hay `requirements.txt` porque no hay
dependencias de `pip`.

## Probar sin hardware

El modo demostración genera todas las señales a 100 Hz y permite revisar tanto
los indicadores como las gráficas:

```sh
cd debug_app
python3 robot_debug.py --demo
```

Opciones útiles:

```text
--robot DIRECCION   IP del XIAO; predeterminada: 192.168.4.1
--udp-port PUERTO   Puerto de telemetría; predeterminado: 3333
--http-port PUERTO  Puerto de la interfaz local; predeterminado: 8765
--no-browser        No abre automáticamente el navegador
--demo              Usa datos sintéticos en lugar del XIAO
--demo-rate HZ      Frecuencia de la demostración, entre 1 y 500 Hz
```

## Qué muestra

- Velocidad en cm/s y mm/s.
- Presencia de obstáculo y vigencia de la lectura de visión.
- Número de vuelta, tarea y dirección de recorrido.
- Distancias frontal, izquierda, derecha y trasera.
- Distancia y ángulo del obstáculo.
- Ticks y distancia acumulada del encoder.
- Salidas Stanley, tangencial y comando final de dirección.
- PWM del motor y tiempo fuente de la Teensy.
- Frecuencia recibida, pérdida de paquetes, edad de la última muestra,
  secuencia, errores CRC y muestras omitidas por la transmisión de la Teensy.

Un valor inválido se muestra como `—`; nunca se confunde con cero. **Pausar
gráficas** congela solamente el historial visual: las tarjetas y el receptor UDP
continúan al día. **Limpiar** vacía el historial de las gráficas.

## Flujo de datos y latencia

La aplicación abre un socket UDP local y envía `SUB1` una vez por segundo a
`192.168.4.1:3333`. El XIAO responde con un ACK de estado y publica las tramas
al mismo puerto de origen. Al cerrar, la aplicación manda `UNS1`.

El ACK actual mide 36 bytes y usa el layout little-endian `<4sBBH7I>`: `ACK1`,
versión `1`, tamaño de telemetría `64`, tamaño de estado `36` y siete contadores
`u32`: tramas UART válidas, errores CRC UART, errores de envoltura, tramas
coalescidas, errores de envío UDP, errores de recepción UDP y errores de hardware
UART. La aplicación conserva compatibilidad con los ACK anteriores de 4 y 6
bytes, aunque esos no incluyen los contadores del puente.

El receptor guarda solamente la trama válida más reciente. El stream SSE hacia
el navegador también se limita a 50 Hz y salta estados intermedios si la interfaz
se atrasa. Así no se acumula una cola de datos viejos. Las gráficas se dibujan en
Canvas a un máximo de 25 cuadros por segundo y conservan una ventana circular de
10, 20, 30 o 60 segundos.

El servidor web escucha exclusivamente en `127.0.0.1`: otros equipos de la red
no pueden abrir el dashboard de la Mac.

## Protocolo WRO v1

Cada datagrama contiene exactamente 64 bytes en little-endian. El CRC se
transmite little-endian y se calcula con CRC-16/CCITT-FALSE, polinomio `0x1021`
e inicialización `0xFFFF`, sobre los bytes 0 a 61.

| Offset | Tamaño | Tipo | Campo |
|---:|---:|---|---|
| 0 | 4 | bytes | Magic `WRO1` |
| 4 | 1 | `u8` | Versión = 1 |
| 5 | 1 | `u8` | Tamaño = 64 |
| 6 | 2 | `u16` | Flags de validez/estado |
| 8 | 4 | `u32` | Secuencia |
| 12 | 4 | `u32` | Tiempo Teensy, µs |
| 16 | 4 | `f32` | Velocidad, mm/s |
| 20 | 1 | `u8` | Vuelta |
| 21 | 1 | `u8` | Tarea |
| 22 | 1 | `i8` | Dirección |
| 23 | 1 | `u8` | Controlador activo |
| 24 | 2 | `u16` | TOF frontal, mm |
| 26 | 2 | `u16` | TOF izquierdo, mm |
| 28 | 2 | `u16` | TOF derecho, mm |
| 30 | 2 | `u16` | TOF trasero, mm |
| 32 | 4 | `f32` | Distancia aparente al obstáculo, píxeles (hasta calibrar la cámara) |
| 36 | 4 | `f32` | Ángulo del obstáculo, grados |
| 40 | 4 | `i32` | Ticks del encoder |
| 44 | 4 | `f32` | Distancia del encoder, mm |
| 48 | 4 | `f32` | Salida Stanley, grados |
| 52 | 4 | `f32` | Salida tangencial, grados |
| 56 | 4 | `f32` | Comando final, grados |
| 60 | 2 | `i16` | PWM del motor |
| 62 | 2 | `u16` | CRC16 |

Flags:

| Bit | Significado |
|---:|---|
| 0 | Obstáculo detectado |
| 1 | Visión fresca |
| 2 | Velocidad válida |
| 3 | Encoder válido |
| 4–7 | Frontal / izquierdo / derecho / trasero válidos |
| 8 | Distancia de obstáculo válida |
| 9 | Ángulo de obstáculo válido |
| 10 | Salida Stanley válida |
| 11 | Salida tangencial válida |
| 12 | Comando de dirección válido |
| 13 | La muestra anterior no cupo en el TX UART de la Teensy y fue omitida |

Los controladores se identifican como: `0` ninguno, `1` IMU, `2` Stanley y `3`
tangencial. La dirección usa `-1` para horario, `1` para antihorario y `0` para
sin definir.

## Pruebas

Desde la raíz del proyecto:

```sh
python3 -m unittest discover -s debug_app/tests -v
```

Para una prueba funcional rápida sin abrir el navegador:

```sh
python3 debug_app/robot_debug.py --demo --no-browser
```

Abre manualmente `http://127.0.0.1:8765/` y termina con `Control-C`.

## Solución de problemas

- **XIAO conectado · esperando Teensy:** llegó el ACK del C6, pero todavía no
  ha recibido una trama UART válida desde la Teensy.
- **Buscando XIAO:** revisa que la Mac esté conectada a la red del robot y que
  el XIAO use `192.168.4.1:3333`.
- **CRC inválido:** normalmente indica ruido, pérdida de bytes o una diferencia
  entre el layout de la Teensy y el protocolo de esta aplicación.
- **TX Teensy · muestra omitida:** la muestra anterior se descartó porque no
  cabía completa en `Serial5`. Es preferible perder una muestra a frenar el lazo
  principal del robot. No indica un desborde del receptor del C6.
- **El puerto 8765 está ocupado:** inicia con otro, por ejemplo
  `python3 robot_debug.py --http-port 8766`.
