# Movimientos por centímetros

Las funciones nuevas son independientes de `setTask()`, `updateCM()`,
`driveAtPWM()` y `driveAtSpeed()`. Las funciones existentes conservan su código.
No se ha cambiado el `loop` de obstáculos.

## Usar el objeto que ya tiene Robot

Después de inicializar el motor y el servo, enlaza la dirección una vez:

```cpp
robot.move.setSteeringController(robot.ackermann);
```

Inicia cada movimiento **una sola vez**, al entrar en un estado:

```cpp
robot.move.moveByCm(30.0f, 15.0f);          // Avanzar 30 cm a 15 cm/s.
robot.move.moveByCm(-10.0f, 8.0f);          // Retroceder 10 cm a 8 cm/s.
robot.move.turnByCm(25.0f, 12.0f, -30.0f); // Avanzar 25 cm con dirección a -30°.
```

Son ejemplos alternativos: una orden nueva reemplaza la anterior; no se encolan.
El segundo argumento es **velocidad en cm/s**, no PWM. La distancia de un giro
es la recorrida según el encoder, no el desplazamiento recto ni un ángulo de yaw.
`moveByCm()` centra la dirección si está enlazada. `turnByCm()` necesita que lo esté.

Cuando quieras integrarlo, después de actualizar sensores y antes de la máquina
de estados, añade este callback al `loop`:

```cpp
if (!robot.move.executeMoving()) {
    return;
}
robot.executeTaskObstacles();
```

`executeMoving()` comprueba la distancia en cada llamada y calcula el PID cada
20 ms por defecto. Devuelve `false` durante el movimiento y `true` cuando termina
o no hay una orden pendiente. Al terminar frena una vez; las llamadas posteriores
no sobrescriben las órdenes de otros estados. Avanza el estado al iniciar una
orden para no volver a iniciarla al terminar.

## Constructor y ajustes

Para una instancia nueva de la misma clase `Move`:

```cpp
AckermannController steering; // Debe vivir tanto como Move.
MovePIDConfig pid;
Move movement(steering, pid);
// En setup(): steering.begin(); movement.begin();
```

También existen `Move()` y `Move(const MovePIDConfig&)`. Usa una sola instancia
para controlar el motor físico; si ya usas `Robot`, configura `robot.move`.

Para ajustar el objeto existente, después de `begin()`:

```cpp
MovePIDConfig pid;
pid.kp = 5.0f;  // PWM / (cm/s)
pid.ki = 2.0f;  // PWM / cm
pid.kd = 0.02f; // PWM / (cm/s²)
pid.maxPWM = MAX_PWM;
pid.pwmForwardSign = -1;     // Coincide con el avance actual de obstáculos.
pid.encoderForwardSign = -1; // Usar +1 si el encoder aumenta al avanzar.
robot.move.configureMotionPID(pid);
```

Las ganancias son valores iniciales para calibrar en el robot. El signo del
encoder debe comprobarse en el hardware. El PID mide velocidad con los ticks y
el tiempo transcurrido; la integral aumenta el PWM si el encoder sigue quieto
por falta de fuerza o durante la toma de holgura. Limita la integral al saturar
el PWM y la reinicia al cancelar o iniciar otra orden. No compensa la distancia
de holgura que el encoder pueda registrar sin desplazamiento real del robot.

Las órdenes devuelven `false` si sus parámetros son inválidos y dejan el motor
frenado; una distancia cero termina inmediatamente. `cancelMoving()` cancela y
frena. Antes de entregar el control a las funciones antiguas, cancela la orden
pendiente: esas funciones no conocen el estado del nuevo controlador. No hay un
timeout automático; sin alcanzar la distancia, el callback sigue devolviendo
`false`. La odometría relativa existente no se reinicia.

Pruebas simuladas: `python3 tests/check_move_cm.py` desde la raíz de `src`.
