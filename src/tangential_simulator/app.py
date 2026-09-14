"""Desktop simulator. Run with .venv/bin/python app.py or start.command."""
import csv
import math
import sys
from pathlib import Path

from PySide6.QtCore import QPointF, QRectF, Qt, QTimer
from PySide6.QtGui import QColor, QPainter, QPainterPath, QPen
from PySide6.QtWidgets import (
    QApplication, QCheckBox, QComboBox, QDoubleSpinBox, QFileDialog,
    QFormLayout, QGroupBox, QHBoxLayout, QLabel, QMainWindow, QMessageBox,
    QPushButton, QScrollArea, QSplitter, QVBoxLayout, QWidget,
)
from model import CONTROLS, PREFIX, Simulation, firmware_defaults, validate


class View(QWidget):
    def __init__(self, app, kind):
        super().__init__()
        self.app, self.kind = app, kind
        self.setMinimumSize(300, 210 if kind != "map" else 330)

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        p.fillRect(self.rect(), QColor("#101d2c"))
        p.setPen(QColor("#d6e4ef"))
        title = {"map": "TRAYECTORIA · vista superior / mm", "camera": "CÁMARA IDEAL · píxeles",
                 "plot": "DIRECCIÓN · pedido / aplicado / error IMU"}[self.kind]
        p.drawText(16, 26, title)
        getattr(self, "draw_" + self.kind)(p)

    def line(self, p, a, b, color, width=1):
        p.setPen(QPen(QColor(color), width))
        p.drawLine(QPointF(*a), QPointF(*b))

    def draw_map(self, p):
        sim, settings = self.app.sim, self.app.settings()
        points = sim.trail + [(x, y) for x, y, _ in sim.obstacles]
        xmin = min(-600, min(x for x, y in points) - 200)
        xmax = max(600, max(x for x, y in points) + 200)
        ymin = min(-200, min(y for x, y in points) - 200)
        ymax = max(1600, max(y for x, y in points) + 300)
        scale = min((self.width()-70)/(xmax-xmin), (self.height()-85)/(ymax-ymin))
        ox = (self.width()-(xmax-xmin)*scale)/2
        def pos(x, y):
            return (ox+(x-xmin)*scale, self.height()-35-(y-ymin)*scale)
        for x in range(math.ceil(xmin/500)*500, int(xmax)+1, 500):
            self.line(p, pos(x, ymin), pos(x, ymax), "#203144")
            p.setPen(QColor("#7e96aa"))
            p.drawText(QPointF(*pos(x, ymin)), str(x))
        for y in range(math.ceil(ymin/500)*500, int(ymax)+1, 500):
            self.line(p, pos(xmin, y), pos(xmax, y), "#203144")
            p.setPen(QColor("#7e96aa"))
            p.drawText(QPointF(*pos(xmin, y)), str(y))
        p.setBrush(Qt.NoBrush)
        for x, y, direction in sim.obstacles:
            color = QColor("#4cdda9" if direction > 0 else "#ff777c")
            p.setPen(QPen(color, 1, Qt.DashLine))
            radius = self.app.params()["SECURITY_RADIUS_PX"] / settings["scale"] * scale
            p.drawEllipse(QPointF(*pos(x, y)), radius, radius)
            p.setBrush(color)
            p.drawEllipse(QPointF(*pos(x, y)), max(4, 35*scale), max(4, 35*scale))
            p.setBrush(Qt.NoBrush)
        path = QPainterPath()
        path.moveTo(QPointF(*pos(*sim.trail[0])))
        for xy in sim.trail[1:]:
            path.lineTo(QPointF(*pos(*xy)))
        p.setPen(QPen(QColor("#64caff"), 2.5))
        p.drawPath(path)
        center = pos(sim.x, sim.y)
        radius = max(7, settings["body_radius"] * scale)
        p.setBrush(QColor("#64caff"))
        p.drawEllipse(QPointF(*center), radius, radius)
        tip = (center[0]-math.sin(sim.heading)*radius*2, center[1]-math.cos(sim.heading)*radius*2)
        self.line(p, center, tip, "#ffffff", 3)
        sp = math.radians(settings["setpoint"])
        self.line(p, center, (center[0]-math.sin(sp)*55, center[1]-math.cos(sp)*55), "#ffca70", 2)
        p.setPen(QColor("#9eb4c6"))
        p.drawText(16, self.height()-12, "Azul: recorrido    Amarillo: setpoint    Círculos: radio de seguridad ideal")

    def draw_camera(self, p):
        cfg = self.app.cfg
        scale = min((self.width()-50)/cfg["width"], (self.height()-65)/cfg["height"])
        left = (self.width()-cfg["width"]*scale)/2
        top = 40
        p.setPen(QColor("#3e566e"))
        p.setBrush(Qt.NoBrush)
        p.drawRect(QRectF(left, top, cfg["width"]*scale, cfg["height"]*scale))
        origin = (left+cfg["width"]*scale/2, top)
        self.line(p, origin, (origin[0], top+cfg["height"]*scale), "#3e566e")
        detection = self.app.output.get("detection")
        if detection:
            distance, x, y, direction = detection
            end = (left+x*scale, top+y*scale)
            self.line(p, origin, end, "#ffca70", 2)
            p.setBrush(QColor("#4cdda9" if direction > 0 else "#ff777c"))
            p.drawEllipse(QPointF(*end), 7, 7)
            p.setPen(QColor("#d6e4ef"))
            p.drawText(16, self.height()-10, f"x={x:.1f}   y={y:.1f}   distancia={distance:.1f} px")
        else:
            p.setPen(QColor("#9eb4c6"))
            p.drawText(16, self.height()-10, "Sin obstáculo válido")

    def draw_plot(self, p):
        data = self.app.sim.history
        if not data:
            return
        peak = max(30, max(abs(v) for row in data for v in row[1:])) * 1.1
        left, top, width, height = 55, 50, self.width()-80, self.height()-95
        start, end = data[0][0], max(data[-1][0], data[0][0]+.02)
        def xy(t, angle):
            return left+(t-start)/(end-start)*width, top+height/2-angle/peak*height/2
        for value in (-self.app.cfg["limit"], 0, self.app.cfg["limit"]):
            self.line(p, xy(start, value), xy(end, value), "#3e566e")
            p.setPen(QColor("#9eb4c6"))
            p.drawText(3, int(xy(start, value)[1])+4, f"{value:.1f}°")
        for index, color in [(1, "#ffca70"), (2, "#64caff"), (3, "#cf9fff")]:
            path = QPainterPath()
            path.moveTo(QPointF(*xy(data[0][0], data[0][index])))
            for row in data[1:]:
                path.lineTo(QPointF(*xy(row[0], row[index])))
            p.setPen(QPen(QColor(color), 2))
            p.drawPath(path)
        p.setPen(QColor("#9eb4c6"))
        p.drawText(16, self.height()-12, f"{start:.2f}–{end:.2f} s  ·  Amarillo: pedido   Azul: aplicado   Violeta: IMU")


class App(QMainWindow):
    def __init__(self):
        super().__init__()
        self.defaults, self.cfg = firmware_defaults()
        self.sim, self.output = Simulation(), {}
        self.fields = {}
        self.setWindowTitle("chaBots · Laboratorio de evasión tangencial")
        self.resize(1280, 900)
        root = QWidget()
        self.setCentralWidget(root)
        layout = QVBoxLayout(root)
        title = QLabel("Laboratorio de evasión tangencial")
        title.setStyleSheet("font-size:24px; font-weight:700; color:#64caff")
        layout.addWidget(title)
        note = QLabel("Ecuaciones del firmware · Física y cámara ideales · No se conecta al robot ni modifica su programa")
        note.setWordWrap(True)
        layout.addWidget(note)
        split = QSplitter()
        layout.addWidget(split, 1)
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        panel = QWidget()
        controls = QVBoxLayout(panel)
        scroll.setWidget(panel)
        split.addWidget(scroll)
        self.mode = QComboBox()
        self.mode.addItems(["Trayectoria simulada", "Entradas manuales de cámara / IMU"])
        controls.addWidget(self.mode)
        self.scene = QComboBox()
        self.scene.addItems(["Verde central", "Rojo central", "Alternados", "Sin obstáculos"])
        controls.addWidget(self.scene)
        group = QGroupBox("Controlador · valores de ControlValues.h")
        form = QFormLayout(group)
        for key, (label, lo, hi, step) in CONTROLS.items():
            self.add_field(form, key, label, self.defaults[key], lo, hi, step)
        controls.addWidget(group)
        group = QGroupBox("Modelo físico aproximado")
        form = QFormLayout(group)
        for row in [
            ("speed", "Velocidad (mm/s)", 300, 0, 2000, 10),
            ("wheelbase", "Entre ejes (mm)", 160, 50, 500, 5),
            ("body_radius", "Radio del robot (mm)", 80, 10, 250, 5),
            ("scale", "Cámara ideal (px/mm)", .25, .01, 2, .01),
            ("setpoint", "Setpoint IMU (°)", 0, -180, 180, 5),
            ("dt", "Periodo de control (ms)", 20, 1, 100, 1),
        ]:
            self.add_field(form, *row)
        controls.addWidget(group)
        self.manual_group = QGroupBox("Entradas manuales · sin movimiento")
        form = QFormLayout(self.manual_group)
        self.add_field(form, "x", "Obstáculo X (px)", 160, 0, self.cfg["width"]-1, 1)
        self.add_field(form, "y", "Obstáculo Y (px)", 130, 0, self.cfg["height"]-1, 1)
        self.add_field(form, "imu", "Error IMU (°)", 0, -180, 180, 1)
        self.valid = QCheckBox("Detección válida y UART reciente")
        self.valid.setChecked(True)
        form.addRow(self.valid)
        self.color = QComboBox()
        self.color.addItems(["Verde (+1)", "Rojo (−1)"])
        form.addRow("Color", self.color)
        controls.addWidget(self.manual_group)
        for label, callback in [("Restaurar valores del firmware", self.reload),
                                ("Exportar parámetros .h…", self.export_params),
                                ("Exportar gráfica a CSV…", self.export_csv)]:
            button = QPushButton(label)
            button.clicked.connect(callback)
            controls.addWidget(button)
        controls.addStretch()
        right = QWidget()
        charts = QVBoxLayout(right)
        self.map = View(self, "map")
        charts.addWidget(self.map, 3)
        row = QHBoxLayout()
        self.camera = View(self, "camera")
        self.plot = View(self, "plot")
        row.addWidget(self.camera, 1)
        row.addWidget(self.plot, 2)
        charts.addLayout(row, 2)
        self.telemetry = QLabel()
        self.telemetry.setWordWrap(True)
        self.telemetry.setStyleSheet("font-family:monospace; padding:10px; background:#101d2c")
        charts.addWidget(self.telemetry)
        split.addWidget(right)
        split.setSizes([350, 930])
        buttons = QHBoxLayout()
        self.play = QPushButton("▶ Iniciar")
        self.play.clicked.connect(self.toggle)
        buttons.addWidget(self.play)
        for label, callback in [("Un paso", self.single_step), ("Reiniciar", self.reset)]:
            button = QPushButton(label)
            button.clicked.connect(callback)
            buttons.addWidget(button)
        self.status = QLabel()
        self.status.setWordWrap(True)
        buttons.addWidget(self.status, 1)
        layout.addLayout(buttons)
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.step)
        self.mode.currentIndexChanged.connect(self.reset)
        self.scene.currentIndexChanged.connect(self.reset)
        self.reset()

    def add_field(self, form, key, label, value, low, high, step):
        field = QDoubleSpinBox()
        field.setDecimals(2 if step < 1 else 0)
        field.setRange(low, high)
        field.setSingleStep(step)
        field.setValue(value)
        field.setKeyboardTracking(False)
        self.fields[key] = field
        form.addRow(label, field)

    def params(self):
        return {key: self.fields[key].value() for key in CONTROLS}

    def settings(self):
        return {key: self.fields[key].value() for key in
                ("speed", "wheelbase", "body_radius", "scale", "setpoint", "dt")}

    def pause(self):
        self.timer.stop()
        self.play.setText("▶ Continuar")

    def toggle(self):
        if self.timer.isActive():
            self.pause()
        elif not self.output.get("stop"):
            self.timer.start(int(self.fields["dt"].value()))
            self.play.setText("Ⅱ Pausar")

    def single_step(self):
        self.pause()
        self.step()

    def reset(self, *_):
        self.pause()
        self.sim.reset(self.scene.currentText())
        self.output = {}
        manual = self.mode.currentIndex() == 1
        self.manual_group.setEnabled(manual)
        self.scene.setEnabled(not manual)
        self.play.setText("▶ Iniciar")
        self.status.setText("Listo. + dirección = izquierda en este modelo.")
        self.refresh()

    def step(self):
        if self.output.get("stop"):
            return
        params = self.params()
        try:
            validate(params)
        except ValueError as error:
            self.pause()
            self.status.setText(str(error))
            return
        manual = None
        if self.mode.currentIndex() == 1:
            manual = {key: self.fields[key].value() for key in ("x", "y", "imu")}
            manual.update(valid=self.valid.isChecked() and not (manual["x"] == 250 and manual["y"] == 250),
                          direction=1 if self.color.currentIndex() == 0 else -1)
        self.output = self.sim.step(params, self.cfg, self.settings(), manual)
        if self.output["stop"]:
            self.pause()
            self.status.setText(self.output["stop"] + " Reinicia para repetir.")
        else:
            self.status.setText("Entradas manuales" if manual else "Simulando evasión; sin paredes ni giros de ronda.")
        if self.timer.isActive():
            self.timer.setInterval(int(self.fields["dt"].value()))
        self.refresh()

    def refresh(self):
        out = self.output
        self.telemetry.setText(
            f"t {self.sim.time:.2f} s   ·   IMU {out.get('imu', 0):+.1f}°   ·   "
            f"error combinado {out.get('error', 0):+.1f}°\n"
            f"Pedido {out.get('raw', 0):+.2f}°   →   Aplicado {out.get('steering', 0):+.2f}° "
            f"(límite ±{self.cfg['limit']:.2f}°)\n"
            f"Peso evasión {out.get('weight', 0):.3f}   ·   Tangente {out.get('tangent', 0):.1f}°   ·   "
            f"Derivada filtrada {out.get('derivative', 0):+.1f}°/s")
        for widget in (self.map, self.camera, self.plot):
            widget.update()

    def reload(self):
        try:
            defaults, cfg = firmware_defaults()
            validate(defaults)
        except (OSError, KeyError, ValueError) as error:
            QMessageBox.warning(self, "No se pudo leer el firmware", str(error))
            return
        self.defaults, self.cfg = defaults, cfg
        for key, value in defaults.items():
            self.fields[key].setValue(value)
        self.reset()

    def export_params(self):
        try:
            validate(self.params())
            filename, _ = QFileDialog.getSaveFileName(self, "Guardar copia de parámetros", "tangential_values.h", "Header (*.h)")
            if filename:
                # Export only: never overwrite live firmware through this interface.
                target = Path(filename).resolve()
                firmware_root = Path(__file__).resolve().parents[1]
                if target.is_relative_to(firmware_root / "include") or target.is_relative_to(firmware_root / "lib") or target.is_relative_to(firmware_root / "src"):
                    raise ValueError("Guarda la copia fuera de include/, lib/ y src/ del firmware.")
                target.write_text("// Parámetros exportados del simulador; copiar manualmente al firmware.\n" +
                    "\n".join(f"#define {PREFIX}{key} {value:.6f}f" for key, value in self.params().items()) + "\n")
                self.status.setText("Copia exportada. El firmware no cambió.")
        except (OSError, ValueError) as error:
            QMessageBox.warning(self, "Exportación", str(error))

    def export_csv(self):
        filename, _ = QFileDialog.getSaveFileName(self, "Guardar datos de la gráfica", "tangential_trace.csv", "CSV (*.csv)")
        if filename:
            try:
                with open(filename, "w", newline="") as stream:
                    writer = csv.writer(stream)
                    writer.writerow(["time_s", "requested_deg", "applied_deg", "imu_error_deg"])
                    writer.writerows(self.sim.history)
                self.status.setText("Datos visibles de la gráfica exportados.")
            except OSError as error:
                QMessageBox.warning(self, "Exportación", str(error))


def main():
    application = QApplication(sys.argv)
    application.setStyle("Fusion")
    application.setStyleSheet("""
        QWidget { background:#152435; color:#d6e4ef; font-size:12px; }
        QGroupBox { border:1px solid #344b61; border-radius:6px; margin-top:14px; padding-top:14px; }
        QGroupBox::title { subcontrol-origin:margin; left:10px; color:#64caff; }
        QPushButton { background:#25465f; padding:9px; border-radius:5px; }
        QPushButton:hover { background:#315e7f; }
        QDoubleSpinBox, QComboBox { background:#0f1c2a; padding:5px; border:1px solid #344b61; }
        QWidget:disabled { color:#64788b; }
        QScrollArea { border:0; }
    """)
    try:
        window = App()
    except (OSError, KeyError, ValueError) as error:
        QMessageBox.critical(None, "No se pudo cargar el firmware", str(error))
        return 1
    window.show()
    return application.exec()


if __name__ == "__main__":
    sys.exit(main())
