"""Controller port + explicitly idealized bicycle/camera model. No robot I/O."""
from dataclasses import dataclass, field
from pathlib import Path
import math
import re

ROOT = Path(__file__).resolve().parents[1]
PREFIX = "TAN_EVASION_"
CONTROLS = {
    "SECURITY_RADIUS_PX": ("Radio de seguridad (px)", 0, 500, 1),
    "ACTIVATION_DISTANCE_PX": ("Activación (px)", 1, 600, 1),
    "FULL_EVASION_DISTANCE_PX": ("Evasión completa (px)", 0, 599, 1),
    "ORIENTATION_GAIN": ("Ganancia de orientación", 0, 10, .1),
    "EVASION_GAIN": ("Ganancia de evasión", 0, 10, .1),
    "KP": ("Kp", 0, 10, .1),
    "KD": ("Kd", 0, 2, .01),
    "DERIVATIVE_FILTER": ("Filtro derivada α", 0, 1, .01),
}


def read_defines(path):
    return {name: float(number) for name, number in re.findall(
        r"^\s*#define\s+(\w+)\s+(-?\d+(?:\.\d+)?)(?:f)?\b",
        path.read_text(), re.MULTILINE)}


def firmware_defaults():
    values = read_defines(ROOT / "include/ControlValues.h")
    steering = read_defines(ROOT / "lib/AckermannController/AckermannController.h")
    params = {key: values[PREFIX + key] for key in CONTROLS}
    return params, {"limit": steering["MAX_ACKERMANN_ANGLE"],
                    "no_obstacle_gain": values["NO_OBSTACLE_IMU_GAIN"],
                    "recovery_distance": values["OBSTACLE_CLOSE_RECOVERY_DISTANCE_PX"],
                    "width": values["VISION_WIDTH"], "height": values["VISION_HEIGHT"]}


def clamp(value, low, high):
    return max(low, min(high, value))


def wrap(angle):
    while angle > 180:
        angle -= 360
    while angle < -180:
        angle += 360
    return angle


def validate(params):
    for key, (_, low, high, _) in CONTROLS.items():
        if not math.isfinite(params[key]) or not low <= params[key] <= high:
            raise ValueError(f"Valor fuera de rango: {key}")
    if params["ACTIVATION_DISTANCE_PX"] <= params["FULL_EVASION_DISTANCE_PX"]:
        raise ValueError("Activación debe ser mayor que evasión completa.")


@dataclass
class TangentialController:
    previous: float = 0
    filtered: float = 0
    initialized: bool = False

    def reset(self):
        self.previous = self.filtered = 0
        self.initialized = False

    def update(self, p, imu_error, direction, angle, distance, dt):
        if distance <= 0:
            self.reset()
            return dict(raw=imu_error, error=imu_error, weight=0, tangent=0, derivative=0)
        tangent = math.degrees(math.asin(clamp(p["SECURITY_RADIUS_PX"] / distance, 0, 1)))
        w = clamp((p["ACTIVATION_DISTANCE_PX"] - distance) /
                  (p["ACTIVATION_DISTANCE_PX"] - p["FULL_EVASION_DISTANCE_PX"]), 0, 1)
        w = w * w * (3 - 2 * w)
        error = p["ORIENTATION_GAIN"] * imu_error + w * p["EVASION_GAIN"] * (
            angle + (1 if direction >= 0 else -1) * tangent)
        derivative = wrap(error - self.previous) / dt if self.initialized and dt > .001 else 0
        self.filtered += p["DERIVATIVE_FILTER"] * (derivative - self.filtered)
        self.previous, self.initialized = error, True
        return dict(raw=p["KP"] * error + p["KD"] * self.filtered,
                    error=error, weight=w, tangent=tangent, derivative=self.filtered)


@dataclass
class Simulation:
    controller: TangentialController = field(default_factory=TangentialController)
    x: float = 0
    y: float = 0
    heading: float = 0  # radians, positive left; zero points up the canvas
    time: float = 0
    trail: list = field(default_factory=lambda: [(0, 0)])
    history: list = field(default_factory=list)
    obstacles: list = field(default_factory=list)

    def reset(self, scenario):
        self.controller.reset()
        self.x = self.y = self.heading = self.time = 0
        self.trail = [(0, 0)]
        self.history = []
        self.obstacles = {
            "Verde central": [(0, 800, 1)],
            "Rojo central": [(0, 800, -1)],
            "Alternados": [(0, 800, 1), (-150, 1600, -1), (150, 2400, 1)],
            "Sin obstáculos": [],
        }[scenario]

    def camera(self, scale, cfg):
        visible = []
        for x, y, direction in self.obstacles:
            dx, dy = x - self.x, y - self.y
            right = dx * math.cos(self.heading) + dy * math.sin(self.heading)
            forward = -dx * math.sin(self.heading) + dy * math.cos(self.heading)
            px, py = cfg["width"] / 2 + right * scale, forward * scale
            if 0 <= px < cfg["width"] and 0 < py < cfg["height"]:
                visible.append((math.hypot(px-cfg["width"]/2, py), px, py, direction))
        return min(visible) if visible else None

    def step(self, params, cfg, settings, manual=None):
        dt = settings["dt"] / 1000
        imu_error = wrap(settings["setpoint"] - math.degrees(self.heading))
        detection = self.camera(settings["scale"], cfg)
        if manual is not None:
            imu_error = manual["imu"]
            px, py = manual["x"], manual["y"]
            detection = (math.hypot(px-cfg["width"]/2, py), px, py, manual["direction"]) if manual["valid"] else None
        angle = distance = 0
        if detection:
            distance, px, py, direction = detection
            angle = math.degrees(math.atan2(px-cfg["width"]/2, py))
            out = self.controller.update(params, imu_error, direction, angle, distance, dt)
        else:
            self.controller.reset()
            out = dict(raw=imu_error * cfg["no_obstacle_gain"], error=imu_error,
                       weight=0, tangent=0, derivative=0)
        out.update(steering=clamp(out["raw"], -cfg["limit"], cfg["limit"]),
                   imu=imu_error, distance=distance, angle=angle, detection=detection,
                   stop="")
        if manual is None:
            if detection and distance < cfg["recovery_distance"]:
                out["stop"] = "Umbral de recuperación: el firmware cambia de maniobra aquí."
            else:
                speed = settings["speed"]
                turn = speed / settings["wheelbase"] * math.tan(math.radians(out["steering"]))
                mid = self.heading + turn * dt / 2
                self.x -= speed * math.sin(mid) * dt
                self.y += speed * math.cos(mid) * dt
                self.heading += turn * dt
                self.trail.append((self.x, self.y))
                if any(math.hypot(self.x-x, self.y-y) < settings["body_radius"] + 35
                       for x, y, _ in self.obstacles):
                    out["stop"] = "Contacto en el modelo geométrico."
                elif self.time >= 30:
                    out["stop"] = "Fin de la ventana de 30 segundos."
        self.time += dt
        self.history.append((self.time, out["raw"], out["steering"], imu_error))
        self.history = self.history[-600:]
        return out
