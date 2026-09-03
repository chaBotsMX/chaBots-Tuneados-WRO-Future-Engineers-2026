"use strict";

const element = (id) => document.getElementById(id);
const isValue = (value) => typeof value === "number" && Number.isFinite(value);
const show = (value, decimals = 0) => isValue(value) ? value.toFixed(decimals) : "—";
const showInteger = (value) => Number.isInteger(value) ? value.toLocaleString("es-MX") : "—";

const TASK_NAMES = {
  0: "Sin definir",
  1: "Recta a borde",
  2: "Acercarse al borde",
  3: "Seguir pared",
  4: "Seguir hasta borde",
  5: "Giro abierto",
  6: "Tramo final",
  7: "Finalizado",
  8: "Evasión",
  9: "Giro con obstáculos",
};

const CONTROLLER_NAMES = {
  0: "Ninguno",
  1: "IMU",
  2: "Stanley",
  3: "Tangencial",
};

const SERIES_COLORS = {
  cyan: "#40d9ff",
  blue: "#5b8cff",
  lime: "#a8ee54",
  amber: "#ffbe55",
  red: "#ff6577",
  purple: "#bd8cff",
  gray: "#9ba7b4",
};

class TimeSeriesChart {
  constructor(canvasId, series, options = {}) {
    this.canvas = element(canvasId);
    this.context = this.canvas.getContext("2d");
    this.series = series;
    this.samples = [];
    this.windowSeconds = 20;
    this.step = Boolean(options.step);
    this.includeZero = options.includeZero !== false;
    this.decimals = options.decimals ?? 0;
    this.dirty = true;
  }

  setWindow(seconds) {
    this.windowSeconds = seconds;
    this.prune(performance.now() / 1000);
    this.dirty = true;
  }

  clear() {
    this.samples.length = 0;
    this.dirty = true;
  }

  append(timestamp, values) {
    this.samples.push({ timestamp, values });
    this.prune(timestamp);
    this.dirty = true;
  }

  prune(now) {
    const cutoff = now - this.windowSeconds - 2;
    let removeCount = 0;
    while (removeCount < this.samples.length && this.samples[removeCount].timestamp < cutoff) {
      removeCount += 1;
    }
    if (removeCount > 0) this.samples.splice(0, removeCount);
  }

  resize() {
    const rect = this.canvas.getBoundingClientRect();
    const ratio = Math.min(window.devicePixelRatio || 1, 2);
    const width = Math.max(260, Math.round(rect.width));
    const height = Math.max(150, Math.round(rect.height));
    if (this.canvas.width !== Math.round(width * ratio) || this.canvas.height !== Math.round(height * ratio)) {
      this.canvas.width = Math.round(width * ratio);
      this.canvas.height = Math.round(height * ratio);
      this.context.setTransform(ratio, 0, 0, ratio, 0, 0);
      this.dirty = true;
    }
    return { width, height };
  }

  draw(now) {
    const { width, height } = this.resize();
    if (!this.dirty) return;
    this.dirty = false;
    this.prune(now);

    const ctx = this.context;
    ctx.clearRect(0, 0, width, height);
    const plot = { left: 45, top: 30, right: width - 10, bottom: height - 23 };
    const visibleStart = now - this.windowSeconds;
    const visible = this.samples.filter((sample) => sample.timestamp >= visibleStart);

    let minimum = Infinity;
    let maximum = -Infinity;
    for (const sample of visible) {
      for (const definition of this.series) {
        const value = sample.values[definition.key];
        if (isValue(value)) {
          minimum = Math.min(minimum, value);
          maximum = Math.max(maximum, value);
        }
      }
    }
    if (this.includeZero && Number.isFinite(minimum)) {
      minimum = Math.min(0, minimum);
      maximum = Math.max(0, maximum);
    }

    ctx.lineWidth = 1;
    ctx.font = "10px SFMono-Regular, Consolas, monospace";
    ctx.textBaseline = "middle";
    for (let index = 0; index <= 4; index += 1) {
      const fraction = index / 4;
      const y = plot.top + fraction * (plot.bottom - plot.top);
      ctx.strokeStyle = "rgba(255,255,255,.065)";
      ctx.beginPath();
      ctx.moveTo(plot.left, y);
      ctx.lineTo(plot.right, y);
      ctx.stroke();
    }
    for (let index = 0; index <= 4; index += 1) {
      const x = plot.left + (index / 4) * (plot.right - plot.left);
      ctx.strokeStyle = "rgba(255,255,255,.035)";
      ctx.beginPath();
      ctx.moveTo(x, plot.top);
      ctx.lineTo(x, plot.bottom);
      ctx.stroke();
    }

    if (!Number.isFinite(minimum) || !Number.isFinite(maximum)) {
      ctx.fillStyle = "#6f7a86";
      ctx.textAlign = "center";
      ctx.font = "11px -apple-system, BlinkMacSystemFont, sans-serif";
      ctx.fillText("Esperando datos válidos…", width / 2, height / 2);
      this.drawLegend(ctx, plot.left, 12);
      return;
    }

    if (Math.abs(maximum - minimum) < 1e-9) {
      const padding = Math.max(1, Math.abs(maximum) * 0.1);
      minimum -= padding;
      maximum += padding;
    } else {
      const padding = (maximum - minimum) * 0.10;
      minimum -= padding;
      maximum += padding;
    }

    const xFor = (timestamp) => plot.left + ((timestamp - visibleStart) / this.windowSeconds) * (plot.right - plot.left);
    const yFor = (value) => plot.bottom - ((value - minimum) / (maximum - minimum)) * (plot.bottom - plot.top);

    ctx.fillStyle = "#77828e";
    ctx.textAlign = "right";
    ctx.font = "9px SFMono-Regular, Consolas, monospace";
    ctx.fillText(this.axisLabel(maximum), plot.left - 7, plot.top);
    ctx.fillText(this.axisLabel(minimum), plot.left - 7, plot.bottom);
    ctx.textAlign = "left";
    ctx.fillText(`−${this.windowSeconds} s`, plot.left, plot.bottom + 14);
    ctx.textAlign = "right";
    ctx.fillText("ahora", plot.right, plot.bottom + 14);

    const zeroY = yFor(0);
    if (zeroY >= plot.top && zeroY <= plot.bottom) {
      ctx.strokeStyle = "rgba(255,255,255,.14)";
      ctx.beginPath();
      ctx.moveTo(plot.left, zeroY);
      ctx.lineTo(plot.right, zeroY);
      ctx.stroke();
    }

    ctx.save();
    ctx.beginPath();
    ctx.rect(plot.left, plot.top, plot.right - plot.left, plot.bottom - plot.top);
    ctx.clip();
    for (const definition of this.series) {
      ctx.strokeStyle = definition.color;
      ctx.lineWidth = definition.width || 1.7;
      ctx.lineJoin = "round";
      ctx.lineCap = "round";
      ctx.beginPath();
      let drawing = false;
      let previousX = 0;
      let previousY = 0;
      for (const sample of visible) {
        const value = sample.values[definition.key];
        if (!isValue(value)) {
          drawing = false;
          continue;
        }
        const x = xFor(sample.timestamp);
        const y = yFor(value);
        if (!drawing) {
          ctx.moveTo(x, y);
          drawing = true;
        } else if (this.step) {
          ctx.lineTo(x, previousY);
          ctx.lineTo(x, y);
        } else {
          ctx.lineTo(x, y);
        }
        previousX = x;
        previousY = y;
      }
      ctx.stroke();
    }
    ctx.restore();
    this.drawLegend(ctx, plot.left, 12);
  }

  axisLabel(value) {
    const magnitude = Math.abs(value);
    if (magnitude >= 1000000) return `${(value / 1000000).toFixed(1)}M`;
    if (magnitude >= 1000) return `${(value / 1000).toFixed(1)}k`;
    return value.toFixed(this.decimals);
  }

  drawLegend(ctx, x, y) {
    ctx.textAlign = "left";
    ctx.textBaseline = "middle";
    ctx.font = "9px -apple-system, BlinkMacSystemFont, sans-serif";
    let cursor = x;
    for (const definition of this.series) {
      ctx.fillStyle = definition.color;
      ctx.fillRect(cursor, y - 3, 10, 2);
      cursor += 14;
      ctx.fillStyle = "#8994a1";
      ctx.fillText(definition.label, cursor, y);
      cursor += ctx.measureText(definition.label).width + 14;
    }
  }
}

const charts = [
  new TimeSeriesChart("speedChart", [
    { key: "speed", label: "Velocidad", color: SERIES_COLORS.cyan, width: 2 },
  ], { decimals: 1 }),
  new TimeSeriesChart("sensorsChart", [
    { key: "front", label: "Frontal", color: SERIES_COLORS.cyan },
    { key: "left", label: "Izq.", color: SERIES_COLORS.lime },
    { key: "right", label: "Der.", color: SERIES_COLORS.amber },
    { key: "rear", label: "Trasero", color: SERIES_COLORS.purple },
  ]),
  new TimeSeriesChart("obstacleDistanceChart", [
    { key: "obstacleDistance", label: "Distancia", color: SERIES_COLORS.red, width: 2 },
  ]),
  new TimeSeriesChart("obstacleAngleChart", [
    { key: "obstacleAngle", label: "Ángulo", color: SERIES_COLORS.amber, width: 2 },
  ], { decimals: 1 }),
  new TimeSeriesChart("controllersChart", [
    { key: "stanley", label: "Stanley", color: SERIES_COLORS.cyan },
    { key: "tangential", label: "Tangencial", color: SERIES_COLORS.amber },
    { key: "steering", label: "Mando", color: SERIES_COLORS.lime, width: 2.2 },
  ], { decimals: 1 }),
  new TimeSeriesChart("encoderChart", [
    { key: "encoderTicks", label: "Lectura", color: SERIES_COLORS.blue, width: 2 },
  ], { includeZero: false }),
  new TimeSeriesChart("raceChart", [
    { key: "lap", label: "Vuelta", color: SERIES_COLORS.purple, width: 2 },
    { key: "obstacle", label: "Obstáculo", color: SERIES_COLORS.red },
  ], { step: true }),
];

let pendingSnapshot = null;
let paused = false;
let lastDrawAt = 0;
let lastSampleSequence = null;

function updateText(id, value) {
  element(id).textContent = value;
}

function updateConnection(snapshot) {
  const pill = element("connectionPill");
  const stats = snapshot.stats;
  pill.classList.remove("is-online", "is-offline", "is-waiting", "is-error");

  if (snapshot.mode === "demo" && stats.connected) {
    pill.classList.add("is-online");
    updateText("connectionText", "Demostración activa");
  } else if (stats.connected) {
    pill.classList.add("is-online");
    updateText("connectionText", "Telemetría en vivo");
  } else if (stats.c6_acknowledged) {
    pill.classList.add("is-waiting");
    updateText("connectionText", "XIAO conectado · esperando Teensy");
  } else if (stats.last_error) {
    pill.classList.add("is-error");
    updateText("connectionText", "Error de enlace");
  } else {
    pill.classList.add("is-offline");
    updateText("connectionText", "Buscando XIAO…");
  }
}

function updateDashboard(snapshot) {
  const stats = snapshot.stats;
  const data = snapshot.telemetry;
  const bridge = snapshot.bridge;
  updateConnection(snapshot);
  updateText("modeValue", snapshot.mode === "demo" ? "DEMO" : "UDP");
  updateText("rateValue", stats.connected ? show(stats.rate_hz, 1) : "—");
  updateText("lossValue", stats.received_frames ? show(stats.loss_percent, 2) : "—");
  updateText("ageValue", stats.age_ms === null ? "—" : show(stats.age_ms, 0));
  updateText("receivedFramesValue", showInteger(stats.received_frames));
  updateText("lostFramesValue", showInteger(stats.lost_frames));
  updateText("duplicateFramesValue", showInteger(stats.duplicate_frames));
  updateText("outOfOrderValue", showInteger(stats.out_of_order_frames));
  updateText("sourceResetsValue", showInteger(stats.source_resets));
  updateText("crcErrorsValue", showInteger(stats.crc_errors));
  updateText("malformedValue", showInteger(stats.malformed_frames));
  updateText("lastError", stats.last_error || "");
  updateText("bridgeValidFramesValue", bridge ? showInteger(bridge.valid_uart_frames) : "—");
  updateText("bridgeCrcErrorsValue", bridge ? showInteger(bridge.uart_crc_errors) : "—");
  updateText("bridgeEnvelopeErrorsValue", bridge ? showInteger(bridge.envelope_errors) : "—");
  updateText("bridgeCoalescedValue", bridge ? showInteger(bridge.coalesced_frames) : "—");
  updateText("bridgeUdpSendErrorsValue", bridge ? showInteger(bridge.udp_send_errors) : "—");
  updateText("bridgeUdpReceiveErrorsValue", bridge ? showInteger(bridge.udp_receive_errors) : "—");
  updateText("bridgeUartHardwareErrorsValue", bridge ? showInteger(bridge.uart_hardware_errors) : "—");

  if (!data) return;

  updateText("sequenceValue", showInteger(data.sequence));
  updateText("speedValue", isValue(data.speed_mm_s) ? show(data.speed_mm_s / 10, 1) : "—");
  updateText("speedDetail", isValue(data.speed_mm_s) ? `${show(data.speed_mm_s, 0)} mm/s` : "— mm/s");
  updateText("lapValue", showInteger(data.lap));
  updateText("taskValue", `Tarea · ${TASK_NAMES[data.task] || `Código ${data.task}`}`);
  updateText("encoderTicksValue", showInteger(data.encoder_ticks));
  updateText("encoderDistanceValue", `${show(data.encoder_distance_mm, 1)} mm recorridos`);

  updateText("frontValue", show(data.front_mm));
  updateText("leftValue", show(data.left_mm));
  updateText("rightValue", show(data.right_mm));
  updateText("rearValue", show(data.rear_mm));
  updateText("obstacleDistanceValue", show(data.obstacle_distance_mm, 1));
  updateText("obstacleAngleValue", show(data.obstacle_angle_deg, 1));
  updateText("visionFreshValue", data.vision_fresh ? "Sí" : "No");

  const obstacleCard = element("obstacleCard");
  obstacleCard.classList.toggle("is-detected", data.obstacle_seen);
  obstacleCard.classList.toggle("is-clear", !data.obstacle_seen);
  updateText("obstacleValue", data.obstacle_seen ? "DETECTADO" : "LIBRE");
  updateText("obstacleDetail", data.obstacle_seen
    ? `${show(data.obstacle_distance_mm, 0)} mm · ${show(data.obstacle_angle_deg, 1)}°`
    : "Sin obstáculo en la trayectoria");
  const visionBadge = element("visionBadge");
  visionBadge.classList.toggle("is-fresh", data.vision_fresh);
  updateText("visionBadge", data.vision_fresh ? "VISIÓN FRESCA" : "VISIÓN VENCIDA");

  updateText("controllerModeValue", CONTROLLER_NAMES[data.controller_mode] || `Modo ${data.controller_mode}`);
  updateText("stanleyValue", show(data.stanley_output_deg, 2));
  updateText("tangentialValue", show(data.tangential_output_deg, 2));
  updateText("steeringValue", show(data.steering_command_deg, 2));
  updateText("motorPwmValue", showInteger(data.motor_pwm));
  const directionName = data.direction === 1 ? "Antihorario" : data.direction === -1 ? "Horario" : data.direction === 0 ? "Sin definir" : `Código ${data.direction}`;
  updateText("directionValue", directionName);
  updateText("teensyTimeValue", show(data.teensy_time_us / 1_000_000, 3));

  const txDropped = Boolean(data.teensy_tx_dropped);
  element("txDropStat").classList.toggle("is-warning", txDropped);
  updateText("txDropValue", txDropped ? "Muestra omitida" : "Normal");
}

function appendCharts(data) {
  if (!data || data.sequence === lastSampleSequence) return;
  lastSampleSequence = data.sequence;
  const timestamp = performance.now() / 1000;
  const values = {
    speed: isValue(data.speed_mm_s) ? data.speed_mm_s / 10 : null,
    front: data.front_mm,
    left: data.left_mm,
    right: data.right_mm,
    rear: data.rear_mm,
    obstacleDistance: data.obstacle_distance_mm,
    obstacleAngle: data.obstacle_angle_deg,
    stanley: data.stanley_output_deg,
    tangential: data.tangential_output_deg,
    steering: data.steering_command_deg,
    encoderTicks: data.encoder_ticks,
    lap: data.lap,
    obstacle: data.obstacle_seen ? 1 : 0,
  };
  for (const chart of charts) chart.append(timestamp, values);
}

function animationLoop(timestampMs) {
  if (pendingSnapshot) {
    const snapshot = pendingSnapshot;
    pendingSnapshot = null;
    updateDashboard(snapshot);
    if (!paused) appendCharts(snapshot.telemetry);
  }
  if (timestampMs - lastDrawAt >= 40) {
    const now = performance.now() / 1000;
    for (const chart of charts) chart.draw(now);
    lastDrawAt = timestampMs;
  }
  requestAnimationFrame(animationLoop);
}

function connectEvents() {
  const events = new EventSource("/events");
  events.addEventListener("telemetry", (event) => {
    try {
      pendingSnapshot = JSON.parse(event.data);
      updateText("footerState", "Datos recibidos; la interfaz no envía comandos de control.");
    } catch (error) {
      updateText("footerState", `Respuesta local inválida: ${error.message}`);
    }
  });
  events.onerror = () => {
    updateText("footerState", "Reconectando con la aplicación Python…");
  };
}

element("pauseButton").addEventListener("click", () => {
  paused = !paused;
  element("pauseButton").classList.toggle("is-active", paused);
  updateText("pauseButton", paused ? "Reanudar gráficas" : "Pausar gráficas");
});

element("clearButton").addEventListener("click", () => {
  for (const chart of charts) chart.clear();
  lastSampleSequence = null;
});

element("windowSelect").addEventListener("change", (event) => {
  const seconds = Number(event.target.value);
  for (const chart of charts) chart.setWindow(seconds);
});

window.addEventListener("resize", () => {
  for (const chart of charts) chart.dirty = true;
});

connectEvents();
requestAnimationFrame(animationLoop);
